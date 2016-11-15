#include "window.h"
#include "ui_window.h"



Window::Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);
    m_networkListener = new NetworkListener(this);

    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("User name:"), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty())
        name = text;

    connect(ui->submitButton, SIGNAL(clicked()), this, SLOT(sendMessage()));
    connect(ui->clientList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(updateCurrentClient(QListWidgetItem*)));
    connect(ui->checkBox, SIGNAL(stateChanged(int)), this, SLOT(toggleImportant()));

    m_networkListener->sendHandShake(name);

    fileMenu = menuBar()->addMenu(tr("&File"));
    editMenu = menuBar()->addMenu(tr("&Edit"));
    helpMenu = menuBar()->addMenu(tr("&Help"));

    saveAct = new QAction(tr("&Save All"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save all messages"));

    fileMenu->addAction(saveAct);


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

    //connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), player, SLOT(stop()));
    QSound::play("chime.wav");

}

bool Window::event(QEvent *event)
{
    QString message;
    switch(event->type()){

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
            delete item;
            return true;
}

return QWidget::event(event);

}

void Window::closeEvent (QCloseEvent *event)
{
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
    //grab input from input field not message field
    QString message;
    if(important){
        message = "IMPORTANT" + ui->inputField->text();
    }else{
        message = ui->inputField->text();
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
    QFile file("test.txt");
    if (!file.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text))
        return;
    //Add encryption when we are writing to the text stream
    //Need to access our crypto device in our netowork listener - maybe need to have a better interface for that



    //Maybe we also need to add error logging in some way?
    QTextStream out(&file);

    if (message->type == MessageType::IMPORTANT){
        out << "***IMPORTANT*** ";

    }
    out << message->date.toString("dd/MM/yyyy")
        << " "
        << message->timestamp.toString("H:m:sa")
        << ": "
        << message->message
        << "\n";

    file.close();


}
