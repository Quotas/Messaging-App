#pragma once
#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <QtNetwork>



enum MessageType { HANDSHAKE, IMPORTANT, DEFAULT, HANDSHAKE_REPLY, DISCONNECT };

struct Message{

    QByteArray data;
    QString message;
    QHostAddress senderip;
    QString senderip_string;
    quint16 senderinfo;
    QTime timestamp;
    QDate date;
    MessageType type;

    Message();
    ~Message() {}
    Message(QByteArray data, QString message, QHostAddress senderip, quint16 senderinfo) :
        data(data), message(message), senderip(senderip), senderinfo(senderinfo)
    {
        timestamp = QTime::currentTime();
        date = QDate::currentDate();

        if (message.indexOf("HANDSHAKE") > 1){
            type = MessageType::HANDSHAKE;
        }
        else if(message.indexOf("IMPORTANT") == 0){
            type = MessageType::IMPORTANT;
            this->message.remove(QString("IMPORTANT"), Qt::CaseSensitive);
            qDebug() << "Important Message Created";
        }
        else if(message.indexOf("HANDSHAKE_REPLY") == 0){
            type = MessageType::HANDSHAKE_REPLY;
        }
        else if(message.indexOf("DISCONNECT") > 1){
            type = MessageType::DISCONNECT;
        }
        else{
            type = MessageType::DEFAULT;
        }

        senderip_string = senderip.toString();

    }

};

static Message* NullMessage = NULL;

#endif // DATAGRAM_H



