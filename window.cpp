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
    m_networkListener->sendHandShake(name);
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
    QString message = ui->inputField->text();
    m_networkListener->sendDatagram(message);




}
