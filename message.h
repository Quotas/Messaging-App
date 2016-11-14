#pragma once
#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <QtNetwork>



enum MessageType { HANDSHAKE, IMPORTANT, DEFAULT, HANDSHAKE_REPLY };

struct Message{

    QByteArray data;
    QString message;
    QHostAddress senderip;
    QString senderip_string;
    quint16 senderinfo;
    QTime timestamp;
    MessageType type;

    Message();
    ~Message() {}
    Message(QByteArray data, QString message, QHostAddress senderip, quint16 senderinfo) :
        data(data), message(message), senderip(senderip), senderinfo(senderinfo)
    {
        timestamp = QTime::currentTime();

        if (message.indexOf("HANDSHAKE") > 1){
            type = MessageType::HANDSHAKE;
        }
        else if(message.indexOf("***") == 1){
            type = MessageType::IMPORTANT;
        }
        else if(message.indexOf("HANDSHAKE_REPLY") >= 1){
            type = MessageType::HANDSHAKE_REPLY;
        }
        else{
            type = MessageType::DEFAULT;
        }

        senderip_string = senderip.toString();

    }

};

static Message* NullMessage = NULL;

#endif // DATAGRAM_H



