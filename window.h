#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QtNetwork/QtNetwork>
#include <QInputDialog>
#include <QListWidget>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QSound>
#include <WinSock2.h>
#include <QDesktopServices>
#include <QUrl>

#include <QUuid>
#include <fstream>
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

    void setName(QString pname){ name = pname; m_networkListener->name = pname;}
    const QString getName(){ return name; }
    void playAlert();


private:    
    NetworkListener* m_networkListener;
    Ui::Window* ui;
    QListWidgetItem * m_currentClient;
    QString name;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *formatMenu;
    QMenu *helpMenu;
    QMediaPlayer *player;
    QRegularExpression re;

    QAction *saveAct;
    QAction *openHelp;
    QAction *clearAct;



    bool important = false;

    void closeEvent(QCloseEvent *event);
    void tidyMessages();



private slots:
    void sendMessage();
    void toggleImportant();
    void openHelpDoc();
    void saveMessage(Message *message);
    void updateCurrentClient(QListWidgetItem* item){ m_currentClient = item; qDebug() << m_currentClient->text(); }
};

#endif // WINDOW_H
