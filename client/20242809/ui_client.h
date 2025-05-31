/********************************************************************************
** Form generated from reading UI file 'client.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENT_H
#define UI_CLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ClientMainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBoxAuth;
    QFormLayout *formLayoutAuth;
    QLabel *labelUsername;
    QLineEdit *lineEditUsername;
    QLabel *labelPassword;
    QLineEdit *lineEditPassword;
    QHBoxLayout *horizontalLayoutAuth;
    QRadioButton *radioButtonRegister;
    QRadioButton *radioButtonLogin;
    QPushButton *pushButtonSubmit;
    QFrame *line;
    QGroupBox *groupBoxTransfer;
    QVBoxLayout *verticalLayoutTransfer;
    QSplitter *splitterFiles;
    QWidget *localWidget;
    QVBoxLayout *verticalLayoutLocal;
    QLabel *labelLocalFiles;
    QListWidget *listWidgetLocal;
    QWidget *remoteWidget;
    QVBoxLayout *verticalLayoutRemote;
    QLabel *labelRemoteFiles;
    QListWidget *listWidgetRemote;
    QHBoxLayout *horizontalLayoutButtons;
    QPushButton *pushButtonUpload;
    QPushButton *pushButtonDownload;
    QPushButton *pushButtonRefresh;
    QProgressBar *progressBar;
    QTextBrowser *textBrowserLog;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ClientMainWindow)
    {
        if (ClientMainWindow->objectName().isEmpty())
            ClientMainWindow->setObjectName(QString::fromUtf8("ClientMainWindow"));
        ClientMainWindow->resize(800, 600);
        centralwidget = new QWidget(ClientMainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBoxAuth = new QGroupBox(centralwidget);
        groupBoxAuth->setObjectName(QString::fromUtf8("groupBoxAuth"));
        formLayoutAuth = new QFormLayout(groupBoxAuth);
        formLayoutAuth->setObjectName(QString::fromUtf8("formLayoutAuth"));
        labelUsername = new QLabel(groupBoxAuth);
        labelUsername->setObjectName(QString::fromUtf8("labelUsername"));

        formLayoutAuth->setWidget(0, QFormLayout::LabelRole, labelUsername);

        lineEditUsername = new QLineEdit(groupBoxAuth);
        lineEditUsername->setObjectName(QString::fromUtf8("lineEditUsername"));

        formLayoutAuth->setWidget(0, QFormLayout::FieldRole, lineEditUsername);

        labelPassword = new QLabel(groupBoxAuth);
        labelPassword->setObjectName(QString::fromUtf8("labelPassword"));

        formLayoutAuth->setWidget(1, QFormLayout::LabelRole, labelPassword);

        lineEditPassword = new QLineEdit(groupBoxAuth);
        lineEditPassword->setObjectName(QString::fromUtf8("lineEditPassword"));
        lineEditPassword->setEchoMode(QLineEdit::Password);

        formLayoutAuth->setWidget(1, QFormLayout::FieldRole, lineEditPassword);

        horizontalLayoutAuth = new QHBoxLayout();
        horizontalLayoutAuth->setObjectName(QString::fromUtf8("horizontalLayoutAuth"));
        radioButtonRegister = new QRadioButton(groupBoxAuth);
        radioButtonRegister->setObjectName(QString::fromUtf8("radioButtonRegister"));
        radioButtonRegister->setChecked(true);

        horizontalLayoutAuth->addWidget(radioButtonRegister);

        radioButtonLogin = new QRadioButton(groupBoxAuth);
        radioButtonLogin->setObjectName(QString::fromUtf8("radioButtonLogin"));

        horizontalLayoutAuth->addWidget(radioButtonLogin);

        pushButtonSubmit = new QPushButton(groupBoxAuth);
        pushButtonSubmit->setObjectName(QString::fromUtf8("pushButtonSubmit"));

        horizontalLayoutAuth->addWidget(pushButtonSubmit);


        formLayoutAuth->setLayout(2, QFormLayout::SpanningRole, horizontalLayoutAuth);


        verticalLayout->addWidget(groupBoxAuth);

        line = new QFrame(centralwidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        groupBoxTransfer = new QGroupBox(centralwidget);
        groupBoxTransfer->setObjectName(QString::fromUtf8("groupBoxTransfer"));
        verticalLayoutTransfer = new QVBoxLayout(groupBoxTransfer);
        verticalLayoutTransfer->setObjectName(QString::fromUtf8("verticalLayoutTransfer"));
        splitterFiles = new QSplitter(groupBoxTransfer);
        splitterFiles->setObjectName(QString::fromUtf8("splitterFiles"));
        splitterFiles->setOrientation(Qt::Horizontal);
        localWidget = new QWidget(splitterFiles);
        localWidget->setObjectName(QString::fromUtf8("localWidget"));
        verticalLayoutLocal = new QVBoxLayout(localWidget);
        verticalLayoutLocal->setObjectName(QString::fromUtf8("verticalLayoutLocal"));
        verticalLayoutLocal->setContentsMargins(0, 0, 0, 0);
        labelLocalFiles = new QLabel(localWidget);
        labelLocalFiles->setObjectName(QString::fromUtf8("labelLocalFiles"));

        verticalLayoutLocal->addWidget(labelLocalFiles);

        listWidgetLocal = new QListWidget(localWidget);
        listWidgetLocal->setObjectName(QString::fromUtf8("listWidgetLocal"));

        verticalLayoutLocal->addWidget(listWidgetLocal);

        splitterFiles->addWidget(localWidget);
        remoteWidget = new QWidget(splitterFiles);
        remoteWidget->setObjectName(QString::fromUtf8("remoteWidget"));
        verticalLayoutRemote = new QVBoxLayout(remoteWidget);
        verticalLayoutRemote->setObjectName(QString::fromUtf8("verticalLayoutRemote"));
        verticalLayoutRemote->setContentsMargins(0, 0, 0, 0);
        labelRemoteFiles = new QLabel(remoteWidget);
        labelRemoteFiles->setObjectName(QString::fromUtf8("labelRemoteFiles"));

        verticalLayoutRemote->addWidget(labelRemoteFiles);

        listWidgetRemote = new QListWidget(remoteWidget);
        listWidgetRemote->setObjectName(QString::fromUtf8("listWidgetRemote"));

        verticalLayoutRemote->addWidget(listWidgetRemote);

        splitterFiles->addWidget(remoteWidget);

        verticalLayoutTransfer->addWidget(splitterFiles);

        horizontalLayoutButtons = new QHBoxLayout();
        horizontalLayoutButtons->setObjectName(QString::fromUtf8("horizontalLayoutButtons"));
        pushButtonUpload = new QPushButton(groupBoxTransfer);
        pushButtonUpload->setObjectName(QString::fromUtf8("pushButtonUpload"));

        horizontalLayoutButtons->addWidget(pushButtonUpload);

        pushButtonDownload = new QPushButton(groupBoxTransfer);
        pushButtonDownload->setObjectName(QString::fromUtf8("pushButtonDownload"));

        horizontalLayoutButtons->addWidget(pushButtonDownload);

        pushButtonRefresh = new QPushButton(groupBoxTransfer);
        pushButtonRefresh->setObjectName(QString::fromUtf8("pushButtonRefresh"));

        horizontalLayoutButtons->addWidget(pushButtonRefresh);


        verticalLayoutTransfer->addLayout(horizontalLayoutButtons);

        progressBar = new QProgressBar(groupBoxTransfer);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(0);

        verticalLayoutTransfer->addWidget(progressBar);

        textBrowserLog = new QTextBrowser(groupBoxTransfer);
        textBrowserLog->setObjectName(QString::fromUtf8("textBrowserLog"));

        verticalLayoutTransfer->addWidget(textBrowserLog);


        verticalLayout->addWidget(groupBoxTransfer);

        ClientMainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ClientMainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 22));
        ClientMainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(ClientMainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        ClientMainWindow->setStatusBar(statusbar);

        retranslateUi(ClientMainWindow);

        QMetaObject::connectSlotsByName(ClientMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ClientMainWindow)
    {
        ClientMainWindow->setWindowTitle(QApplication::translate("ClientMainWindow", "Client - File Transfer System", nullptr));
        groupBoxAuth->setTitle(QApplication::translate("ClientMainWindow", "User Authentication", nullptr));
        labelUsername->setText(QApplication::translate("ClientMainWindow", "Username:", nullptr));
        labelPassword->setText(QApplication::translate("ClientMainWindow", "Password:", nullptr));
        radioButtonRegister->setText(QApplication::translate("ClientMainWindow", "Register", nullptr));
        radioButtonLogin->setText(QApplication::translate("ClientMainWindow", "Login", nullptr));
        pushButtonSubmit->setText(QApplication::translate("ClientMainWindow", "Submit", nullptr));
        groupBoxTransfer->setTitle(QApplication::translate("ClientMainWindow", "File Transfer", nullptr));
        labelLocalFiles->setText(QApplication::translate("ClientMainWindow", "Local Files", nullptr));
        labelRemoteFiles->setText(QApplication::translate("ClientMainWindow", "Remote Files", nullptr));
        pushButtonUpload->setText(QApplication::translate("ClientMainWindow", "Upload", nullptr));
        pushButtonDownload->setText(QApplication::translate("ClientMainWindow", "Download", nullptr));
        pushButtonRefresh->setText(QApplication::translate("ClientMainWindow", "Refresh", nullptr));
        textBrowserLog->setPlaceholderText(QApplication::translate("ClientMainWindow", "Log Messages...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ClientMainWindow: public Ui_ClientMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENT_H
