/********************************************************************************
** Form generated from reading UI file 'server.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVER_H
#define UI_SERVER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ServerMainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTextBrowser *textBrowserLog;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ServerMainWindow)
    {
        if (ServerMainWindow->objectName().isEmpty())
            ServerMainWindow->setObjectName(QString::fromUtf8("ServerMainWindow"));
        ServerMainWindow->resize(600, 400);
        centralwidget = new QWidget(ServerMainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        textBrowserLog = new QTextBrowser(centralwidget);
        textBrowserLog->setObjectName(QString::fromUtf8("textBrowserLog"));

        verticalLayout->addWidget(textBrowserLog);

        ServerMainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ServerMainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 600, 21));
        ServerMainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(ServerMainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        ServerMainWindow->setStatusBar(statusbar);

        retranslateUi(ServerMainWindow);

        QMetaObject::connectSlotsByName(ServerMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ServerMainWindow)
    {
        ServerMainWindow->setWindowTitle(QApplication::translate("ServerMainWindow", "Server - File Transfer System", nullptr));
        textBrowserLog->setPlaceholderText(QApplication::translate("ServerMainWindow", "Server Log Messages...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ServerMainWindow: public Ui_ServerMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVER_H
