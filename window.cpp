#include "window.h"
#include "ui_window.h"



Window::Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Window)
{
    //lol regular expressions
    re.setPattern("^(?:(?:31(\/|-|\.)(?:0?[13578]|1[02]))\1|(?:(?:29|30)(\/|-|\.)(?:0?[1,3-9]|1[0-2])\2))(?:(?:1[6-9]|[2-9]\d)?\d{2})$|^(?:29(\/|-|\.)0?2\3(?:(?:(?:1[6-9]|[2-9]\d)?(?:0[48]|[2468][048]|[13579][26])|(?:(?:16|[2468][048]|[3579][26])00))))$|^(?:0?[1-9]|1\d|2[0-8])(\/|-|\.)(?:(?:0?[1-9])|(?:1[0-2]))\4(?:(?:1[6-9]|[2-9]\d)?\d{2})$");

    ui->setupUi(this);

    //Create our networkListener
    m_networkListener = new NetworkListener(this);

    //Text box to set our client name
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("User name:"), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty())
        setName(text);

    //Do qt stuff
    connect(ui->submitButton, SIGNAL(clicked()), this, SLOT(sendMessage()));
    connect(ui->clientList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(updateCurrentClient(QListWidgetItem*)));
    connect(ui->checkBox, SIGNAL(stateChanged(int)), this, SLOT(toggleImportant()));

    //Send our handshake when we first start
    m_networkListener->sendHandShake(name);


    //Create our menu bar and bind our actions to it
    fileMenu = menuBar()->addMenu(tr("&File"));
    editMenu = menuBar()->addMenu(tr("&Edit"));
    helpMenu = menuBar()->addMenu(tr("&Help"));

    saveAct = new QAction(tr("&Save All"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save all messages."));

    clearAct = new QAction(tr("&Clear All"), this);
    clearAct->setStatusTip(tr("Clear all messages"));

    openHelp = new QAction(tr("&Documentation"), this);
    openHelp->setStatusTip(tr("Open help documentation."));

    fileMenu->addAction(saveAct);
    editMenu->addAction(clearAct);
    helpMenu->addAction(openHelp);


    connect(openHelp, SIGNAL(triggered(bool)), this, SLOT(openHelpDoc()));

    //Delete all our meesages over 6 months old
    tidyMessages();


}


Window::~Window()
{
    delete ui;
}

void Window::playAlert(){



    //player = new QMediaPlayer;
    //player->setMedia(QUrl::fromLocalFile("Alert.mp3"));
    //player->setVolume(15);
    //player->play();

    //Really janky fix for making our mp3s stop playing and not getting static from our mediaplayer object
    //connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), player, SLOT(stop()));

    //must more elegant just to play a wav
    QSound::play("chime.wav");

}


void Window::openHelpDoc(){

    //QString path = QDir::currentPath() + "/html/index.html";
    QDesktopServices::openUrl(QUrl("file:C:\\Users\\seamu\\Desktop\\SoftwareDevelopment\\MessagingApp\\html\\index.html"));

}


void Window::tidyMessages(){

    QFile file("MessageLog.txt");
    if(!file.open(QIODevice::ReadWrite| QIODevice::Text)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QString s;
    QTextStream in(&file);

    //Read line by line grab a slice of each line using a regular exp and then check to see if that date + 6 months is greater than the current date
    //If it isnt we delete it otherwise we add it back to the file.
    while(!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatch match = re.match(line);
        if(match.hasMatch()){
            QString matched = match.captured(0);
            QDate date = QDate::fromString(matched);
            QDate curdate = QDate::currentDate();
            if (date.addMonths(6) >= curdate){
                s.append(line + "\n");

            }

        }

    }

    //Resize our file and then stream our string back to it
    file.resize(0);
    in << s;

    file.close();






}

bool Window::event(QEvent *event)
{
    QString message;
    switch(event->type()){

    //Process our events here sent from our child widgets haHAA

        case DATAGRAM_PROCCESSED_SENT :
            if(MessageHandler::getMostRecentMessage(false)->type == MessageType::IMPORTANT){
                playAlert();
                message += "IMPORTANT: ";
            }
            message += name + ": " + MessageHandler::getMostRecentMessage(false)->message;
            ui->messageField->append(message);
            saveMessage(MessageHandler::getMostRecentMessage(false));
            return true;
        case DATAGRAM_PROCCESSED_RECEIVED :
            ui->messageField->append(MessageHandler::getMostRecentMessage()->senderip_string + ": " + MessageHandler::getMostRecentMessage()->message);
            return true;
        case CLIENT_CONNECTED :
            //grab our recently connceted client and update the client list in the ui
            ui->clientList->addItem(m_networkListener->getMostRecentClient()->name);
            return true;
        case CLIENT_DISCONNECTED :
            QList<QListWidgetItem *> items = ui->clientList->findItems(m_networkListener->getMostRecentDisconnected()->name,  Qt::MatchContains);

            QListWidgetItem* item = ui->clientList->takeItem(ui->clientList->row(items.at(0)));
            m_networkListener->removeClient(item->text());
            delete item;
            return true;
}

return QWidget::event(event);

}

void Window::closeEvent (QCloseEvent *event)
{

    //Handle our close event here so we can send a disconnect packet when we click our close button
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Secure Messaging"
                                                                      "",
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        m_networkListener->sendDisconnect(name);
        event->accept();
    }
}

void Window::sendMessage(){
    //Polymorphism?!
    //Simple function for interfacing with our
    //grab input from input field not message field
    QString message;
    if(important){
        message = "IMPORTANT" + ui->inputField->text();
    }else{
        message = ui->inputField->text();
    }
    //Send to a specific client if we have a client currently selected
    if (m_currentClient != NULL){
        m_networkListener->sendDatagram(message, m_networkListener->getClient(m_currentClient->text()));

        //Currently there is no way to unselect a client
        //As a work around just set the current client back to 0 after we process a private message
        m_currentClient = 0;
    }
    m_networkListener->sendDatagram(message);

    ui->inputField->clear();




}

void Window::toggleImportant(){
    if(ui->checkBox->isChecked()){
        important = true;
        qDebug() << "Checked";
    }else{
        important = false;
        qDebug() << "Unchecked";
    }

}

void Window::saveMessage(Message *message){
    QFile file("MessageLog.txt");
    if (!file.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text))
        return;

    //Maybe we also need to add error logging in some way?
    //Just write our bytearray into our file by casting it into a QString - it dosent have to be pretty
    QTextStream out(&file);

    if (message->type == MessageType::IMPORTANT){
        out << "***IMPORTANT*** ";

    }
    out << message->date.toString("dd/MM/yyyy")
        << " "
        << message->timestamp.toString("H:m:sa")
        << ": "
        << QString(message->data)
        << "\n";

    file.close();


}
