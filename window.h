#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QtNetwork/QtNetwork>
#include <QInputDialog>
#include <QListWidget>

#include <QUuid>
#include "customevents.h"
#include "message.h"
#include "messagehandler.h"
#include "networklistener.h"



extern const QEvent::Type DATAGRAM_PROCCESSED;
extern const QEvent::Type CLIENT_CONNECTED;

namespace Ui {


class Window;
class QAction;

}

class Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = 0);
    ~Window();

    //void handleMessage(Message message){ m_messageHandler->handleMessage(message); }
    bool event(QEvent *event);

    void setName(QString pname){ name = pname;}


private:    
    NetworkListener* m_networkListener;
    Ui::Window* ui;
    QListWidgetItem * m_currentClient;
    QString name;



private slots:
    void sendMessage();
    void updateCurrentClient(QListWidgetItem* item){ m_currentClient = item; qDebug() << m_currentClient->text(); }
};

#endif // WINDOW_H
