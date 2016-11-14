#pragma once
#ifndef DATAGRAMPROCESSED_H
#define DATAGRAMPROCESSED_H

#include <QEvent>

const QEvent::Type CLIENT_CONNECTED = static_cast<QEvent::Type>(QEvent::User + 1);
const QEvent::Type DATAGRAM_PROCCESSED_SENT = static_cast<QEvent::Type>(QEvent::User + 2);
const QEvent::Type DATAGRAM_PROCCESSED_RECEIVED = static_cast<QEvent::Type>(QEvent::User + 3);

class DatagramProccessedSent : public QEvent {
public:
    DatagramProccessedSent() : QEvent(DATAGRAM_PROCCESSED_SENT) {}


};

class DatagramProccessedReceived : public QEvent {
public:
    DatagramProccessedReceived() : QEvent(DATAGRAM_PROCCESSED_RECEIVED) {}


};



class ClientConnected : public QEvent {
public:
    ClientConnected() : QEvent(CLIENT_CONNECTED) {}

};

#endif // DATAGRAMPROCESSED_H
