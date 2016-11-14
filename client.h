#ifndef CLIENT_H
#define CLIENT_H


struct Client{

    Client(QString name, QHostAddress ip) : name(name), clientip(ip) {}
    QString name;
    QHostAddress clientip;




};

static Client* NullClient = NULL;

#endif // CLIENT_H
