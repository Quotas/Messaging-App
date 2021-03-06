#include "messagehandler.h"

extern Message* NullMessage;

std::vector<Message> MessageHandler::m_sentMessages = {};
std::vector<Message> MessageHandler::m_receivedMessages = {};

//static message interface class for accessing sent and received messages globally throughout our program

Message *MessageHandler::getMostRecentMessage(bool received){

    if (received){
        if (m_receivedMessages.size() != 0){
            return &(m_receivedMessages.back());
        }
        else {
            return NullMessage;}
    }
    else{
        if (m_sentMessages.size() != 0){
            return &(m_sentMessages.back());
        }
        else{
            return NullMessage;
        }
    }
}

void MessageHandler::handleMessage(Message message, bool received){
    if (received){
        m_receivedMessages.push_back(message);
    }
    else{
        m_sentMessages.push_back(message);
    }


}

//static instance of our class we dont ever call this contrusctor but we need to foward delcare so we can have static member vars

MessageHandler::MessageHandler(QObject *parent) : QObject(parent)
{

    m_receivedMessages = {};
    m_sentMessages = {};

}
