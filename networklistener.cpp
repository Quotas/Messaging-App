#include "networklistener.h"

NetworkListener::NetworkListener(QObject *parent) : QObject(parent)
{
    //cryptodevice->genKey();

    cryptodevice.setKey(0x0c2ad4a4acb9f015);

    groupAddress = QHostAddress("238.38.38.38");

    udpSocketOut = new QUdpSocket(this);

    udpSocketIn = new QUdpSocket(this);
    udpSocketIn->bind(QHostAddress::AnyIPv4, 45454, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    //udpSocketIn->joinMulticastGroup(groupAddress);

    QList<QNetworkInterface> mListIfaces = QNetworkInterface::allInterfaces();

    for (int i = 0; i < mListIfaces.length(); ++i) {
        bool rez = udpSocketIn->joinMulticastGroup(groupAddress, mListIfaces.at(i));


        qDebug() << rez;
        if (rez){
            qDebug() << mListIfaces.at(i);
            udpSocketOut->bind();
            udpSocketOut->setMulticastInterface(mListIfaces.at(i));
        }
    }

    QTcpSocket socket;
    socket.connectToHost("8.8.8.8", 53); // google DNS, or something else reliable
    if (socket.waitForConnected()) {
        qDebug() << socket.localAddress().toString();
        localAddress = socket.localAddress();
    } else {
        qWarning()
                << "could not determine local IPv4 address:"
                << socket.errorString();
    }

    udpSocketIn->setSocketOption(QAbstractSocket::MulticastLoopbackOption, QVariant(1));
    udpSocketOut->setSocketOption(QAbstractSocket::MulticastLoopbackOption, QVariant(1));
    bool bOptVal = TRUE;

    setsockopt(udpSocketIn->socketDescriptor(), SOL_SOCKET, SO_RCVBUF, (char*) &bOptVal, sizeof(int));

    connect(udpSocketIn, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

}



void NetworkListener::processPendingDatagrams(){

    while (udpSocketIn->hasPendingDatagrams()) {
        qDebug() << "Incoming Message";
        QString datagram_message;
        QByteArray datagram;
        QHostAddress sender;
        quint16 senderPort;
        datagram.resize(udpSocketIn->pendingDatagramSize());
        udpSocketIn->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        QByteArray plaintext = cryptodevice.decryptToByteArray(datagram);
        if (!cryptodevice.lastError() == SimpleCrypt::ErrorNoError) {
            //we have an error
            return;
        }

        QBuffer buffer(&plaintext);
        buffer.open(QIODevice::ReadOnly);
        QDataStream s(&buffer);
        s.setVersion(QDataStream::Qt_5_7);
        s >> datagram_message;
        buffer.close();

        //construct our message from our data
        Message* message = new Message(datagram, datagram_message, sender, senderPort);

        qDebug() << datagram_message;
        qDebug() << message->type;
        qDebug() << sender;

        //Handle loopback message we dont want to handle messages we sent ourselves
        //Need a more reliable way to check sender ip vs local ip to stop loopback
        //Create some bullshit TCP socket and then grab the IP from that and compare it to the sender

        //Check to see the type of message based on our MessageType enum class
        if(message->type == MessageType::HANDSHAKE){
            QString temp = datagram_message;
            temp.chop(9); // chop the end of our message string off
            QString clientname = temp;
            Client *client = new Client(clientname, sender);
            qDebug() << client->name;
            m_clientList.push_back(*client);
            ClientConnected* event = new ClientConnected();
            parent()->event(event);
            //once we have processed that the cleint has been received we need to send a packet back to tell them who we are
            sendHandShakeReply();
            delete client;

            return;
        }

        if(message->type == MessageType::HANDSHAKE_REPLY){
            if(!(sender == localAddress)){
                QString temp = datagram_message;
                temp.chop(15); // chop the end of our message string off
                QString clientname = temp;
                Client *client = new Client(clientname, sender);
                qDebug() << client->name;
                m_clientList.push_back(*client);
                ClientConnected* event = new ClientConnected();
                parent()->event(event);
                //once we have processed that the cleint has been received we need to send a packet back to tell them who we are
                delete client;
                return;
            }
            qDebug() << "Message is sent from sender";
            return;

        }


        if(message->type == MessageType::DISCONNECT){
            QString temp = datagram_message;
            temp.chop(10); // chop the end of our message string off
            QString clientname = temp;
            Client *client = new Client(clientname, sender);
            qDebug() << client->name;
            m_disconnected.push_back(*client);
            ClientDisconnected* event = new ClientDisconnected();
            parent()->event(event);
            delete client;
            return;

        }

        if(sender == localAddress){
            MessageHandler::handleMessage(*message, false);
            DatagramProccessedSent* event = new DatagramProccessedSent();
            parent()->event(event);
            return;
        }
        //Fall through if it is just a default message
        //Having the messagetype be default instead of null is better practice
        MessageHandler::handleMessage(*message, true);

        DatagramProccessedReceived* event = new DatagramProccessedReceived();
        parent()->event(event);


    }



}

Client* NetworkListener::getClient(QString name){

    if(m_clientList.size() != 0){
        for(int i = 0; i < m_clientList.size(); i++){

            if(m_clientList.at(i).name == name){
                return &m_clientList.at(i);
            }


        }

        return NullClient;
    }
    return NullClient;

}

void NetworkListener::removeClient(QString name){

    if(m_clientList.size() != 0){
        for(int i = 0; i < m_clientList.size(); i++){

            if(m_clientList.at(i).name == name){
                if (i == 0){
                    m_clientList.erase(m_clientList.begin());
                }else{
                    m_clientList.erase(m_clientList.begin() + (i - 1));
                }

            }

        }
    }



}

void NetworkListener::sendHandShake(QString name){

    QString message = name + "HANDSHAKE";
    sendDatagram(message);



}


void NetworkListener::sendHandShakeReply(){

    QString message = "HANDSHAKE_REPLY" + name;
    sendDatagram(message, getMostRecentClient());

}

void NetworkListener::sendDisconnect(QString name){

    QString message = name + "DISCONNECT";
    sendDatagram(message);
}

void NetworkListener::sendDatagram(QString a_message, Client *a_client){


    QString message = a_message;
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QDataStream s(&buffer);
    //stream the data into our buffer
    s.setVersion(QDataStream::Qt_5_7);
    s << message;

    QByteArray datagram = cryptodevice.encryptToByteArray(buffer.data());
    if (cryptodevice.lastError() == SimpleCrypt::ErrorNoError) {
        if (a_client != nullptr){
            udpSocketOut->writeDatagram(datagram.data(), datagram.size(), a_client->clientip, 45454);// do something relevant with the cyphertext, such as storing it or sending it over a socket to another machine.
            Message messageObj = Message(datagram, message, a_client->clientip, 45454);
            //MessageHandler::handleMessage(messageObj, false);
        }
        else{
            udpSocketOut->writeDatagram(datagram.data(), datagram.size(), groupAddress, 45454);
            Message messageObj = Message(datagram, message, groupAddress, 45454);
            qDebug() << "Packet send to multicast group";
            //MessageHandler::handleMessage(messageObj, false);

        }
    }
    buffer.close();

    //Here we should send our datagram sent event






}
