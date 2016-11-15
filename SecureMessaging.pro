#-------------------------------------------------
#
# Project created by QtCreator 2016-09-15T14:33:48
#
#-------------------------------------------------

QT       += core gui
QT += network
QT += multimedia
CONFIG += c++11

RC_FILE = myapp.rc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SecureMessaging
TEMPLATE = app


SOURCES += main.cpp\
        window.cpp \
    simplecrypt.cpp \
    networklistener.cpp \
    messagehandler.cpp

HEADERS  += window.h \
    simplecrypt.h \
    networklistener.h \
    client.h \
    messagehandler.h \
    message.h \
    customevents.h

FORMS    += window.ui

