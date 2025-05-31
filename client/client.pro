QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp

HEADERS += mainwindow.h

FORMS += client.ui

# 链接 gmSSL 库（假设安装在系统标准路径中）
LIBS += -lgmssl

