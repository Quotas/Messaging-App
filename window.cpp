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
    connect(saveAct, SIGNAL(triggered()), this, SLOT(newFile()));

    fileMenu->addAction(saveAct);


}


Window::~Window()
{
    delete ui;
}

bool Window::event(QEvent *event)
{

    switch(event->type()){
        case DATAGRAM_PROCCESSED_SENT :
            ui->messageField->append(name + ": " + MessageHandler::getMostRecentMessage(false)->message);
            saveMessage(MessageHandler::getMostRecentMessage(false));
            return true;
        case DATAGRAM_PROCCESSED_RECEIVED :
            ui->messageField->append(MessageHandler::getMostRecentMessage()->senderip_string + ": " + MessageHandler::getMostRecentMessage()->message);
            return true;
        case CLIENT_CONNECTED :
            //grab our recently connceted client and update the client list in the ui
            ui->clientList->addItem(m_networkListener->getMostRecentClient()->name);
            return true;

}

return QWidget::event(event);

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
