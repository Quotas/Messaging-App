#pragma once
#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QObject>
#include "message.h"
#include <vector>


extern Message* NullMessage;


class MessageHandler : public QObject
{
    Q_OBJECT
public:
    explicit MessageHandler(QObject *parent = 0);
    static Message* getMostRecentMessage(bool received = true);
    static void handleMessage(Message message, bool received);



private:

    static std::vector<Message> m_sentMessages;
    static std::vector<Message> m_receivedMessages;



signals:

public slots:
};

static MessageHandler messageHandler;


#endif // MESSAGEHANDLER_H
