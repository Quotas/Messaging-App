#include "networklistener.h"

NetworkListener::NetworkListener(QObject *parent) : QObject(parent)
{

    //have our crypto device no generate a static key
    //cryptodevice->genKey();

    cryptodevice.setKey(0x0c2ad4a4acb9f015);

    //Our multicast group address this just needs to be in a certain range - this is just so the router routes our packets correctly
    groupAddress = QHostAddress("238.38.38.38");

    udpSocketOut = new QUdpSocket(this);

    udpSocketIn = new QUdpSocket(this);
    udpSocketIn->bind(QHostAddress::AnyIPv4, 45454, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    //udpSocketIn->joinMulticastGroup(groupAddress);


    //Because windows is shit we basically need to try every single interface we have and see if multicast is enabled
    //No there isnt a better way to do this
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

    //Because our interface will only show our LAN address if we want to filter packets sent from ourselves we have to create
    //some bullshit TCP socket and check our IP so we do that below
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

    //Does something I dont really understand
    udpSocketIn->setSocketOption(QAbstractSocket::MulticastLoopbackOption, QVariant(1));
    udpSocketOut->setSocketOption(QAbstractSocket::MulticastLoopbackOption, QVariant(1));
    bool bOptVal = TRUE;

    //Call setsockopt from winsock to increase the size of data our UDP socket can receive - I think this is part of
    //fixing the bug where UDP packets were not be sent but I am unsure
    setsockopt(udpSocketIn->socketDescriptor(), SOL_SOCKET, SO_RCVBUF, (char*) &bOptVal, sizeof(int));

    //connect is a QT macro to basically do event listeners so here we connect readyread() to a function that processes our datagram
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

        //Send our datagram to the cryptodevice to be decrypted
        QByteArray plaintext = cryptodevice.decryptToByteArray(datagram);
        if (!cryptodevice.lastError() == SimpleCrypt::ErrorNoError) {
            //we have an error
            return;
        }

        //Open a datastream so we can stream our Byte array into a QString
        //There is probably a better way to do this like calling the QString constructor directly
        //But if we did that then there is no way to garuntee that our bytearray dosent get changed
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

        //When we get sent a handshake we also send a reply
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

            }
            qDebug() << "Message is sent from sender";
            return;

        }

        //Process our disconnect packet
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

        //Process default loopback packets
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

//Grab a client by name from our clientList
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

//Remove a client by name from our clientList
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

//Send a handshake packet when we connect to everyone available
void NetworkListener::sendHandShake(QString name){

    QString message = name + "HANDSHAKE";
    sendDatagram(message);



}

//Send a reply packet to everyone we have received a handshake from
void NetworkListener::sendHandShakeReply(){

    QString message = "HANDSHAKE_REPLY" + name;
    sendDatagram(message, getMostRecentClient());

}

//Send a disconnect packet upon leaving
void NetworkListener::sendDisconnect(QString name){

    QString message = name + "DISCONNECT";
    sendDatagram(message);
}


//General send packet function can send to a client if specified - default is client = NULL
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
    //But we dont cuz I dunno we just dont added to TODO list






}
