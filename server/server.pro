QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Server
TEMPLATE = app

SOURCES += main.cpp \
           servermainwindow.cpp

HEADERS += servermainwindow.h

FORMS += server.ui

LIBS += -lsqlite3

