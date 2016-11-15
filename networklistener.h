#pragma once
#ifndef NETWORKLISTENER_H
#define NETWORKLISTENER_H

#include <QMainWindow>
#include <QtNetwork/QtNetwork>
#include <QObject>
#include <vector>
#include <WinSock2.h>
#include <stdio.h>

#include "message.h"
#include "simplecrypt.h"
#include "client.h"
#include "messagehandler.h"
#include "customevents.h"

#pragma comment(lib, "Ws2_32.lib")


extern Client* NullClient;


extern MessageHandler messageHandler;

class NetworkListener: public QObject
{
    Q_OBJECT
public:
    explicit NetworkListener(QObject *parent = 0);
    Client* getMostRecentClient() { if (m_clientList.size() != 0) {return &(m_clientList.back()); } else { return NullClient; } }
    Client* getMostRecentDisconnected() { if (m_disconnected.size() != 0) {return &(m_disconnected.back()); } else { return NullClient; } }
    void sendHandShake(QString name);
    void sendDisconnect(QString name);

private:
    QHostAddress groupAddress;
    QHostAddress localAddress;
    QUdpSocket *udpSocketOut;
    QUdpSocket *udpSocketIn;
    std::vector<Client> m_clientList;
    std::vector<Client> m_disconnected;
    SimpleCrypt cryptodevice;





public slots:
    void processPendingDatagrams();
    void sendDatagram(QString a_message, Client *a_client = 0);


   // QString decryptDatagram(Packet datagram);


};

#endif // NETWORKLISTENER_H
