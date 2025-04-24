#include "mainwindow.h"
#include "ui_client.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QByteArray>
#include <QDebug>
#include <gmssl/sm3.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define SALT "random_salt"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientMainWindow),
    tcpSocket(new QTcpSocket(this))
{
    ui->setupUi(this);
    connect(tcpSocket, &QTcpSocket::connected, this, &MainWindow::onSocketConnected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonSubmit_clicked()
{
    tcpSocket->abort();
    tcpSocket->connectToHost(SERVER_IP, PORT);
}

void MainWindow::onSocketConnected()
{
    QString username = ui->lineEditUsername->text();
    QString password = ui->lineEditPassword->text();
    QString operation = ui->radioButtonRegister->isChecked() ? "REGISTER" : "LOGIN";
    QString hashedPass = hashPassword(password, SALT);
    if (!sendStringAndWait(tcpSocket, operation))
        return;
    if (!sendStringAndWait(tcpSocket, username))
        return;
    if (!sendStringAndWait(tcpSocket, hashedPass))
        return;
}

void MainWindow::onReadyRead()
{
    QByteArray response = tcpSocket->readAll();
    QMessageBox::information(this, "Server Response", response);
    tcpSocket->disconnectFromHost();
}

void MainWindow::on_pushButtonUpload_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Select file to upload"), "", tr("All Files (*)"));
    if (filePath.isEmpty())
        return;
    
    QFileInfo fi(filePath);
    qDebug() << "Selected file:" << filePath;
    qDebug() << "isFile:" << fi.isFile() << ", size:" << fi.size();
    if (!fi.isFile()) {
        QMessageBox::warning(this, tr("Error"), tr("The selected item is not a valid file."));
        return;
    }
    if (fi.size() == 0) {
        QMessageBox::warning(this, tr("Error"), tr("The selected file is empty."));
        return;
    }
    
    QFile *uploadFile = new QFile(filePath);
    if (!uploadFile->open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file for reading"));
        delete uploadFile;
        return;
    }
    
    qint64 fileSize = uploadFile->size();
    qDebug() << "File size (bytes):" << fileSize;
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(fileSize);
    ui->progressBar->setValue(0);
    
    QTcpSocket *uploadSocket = new QTcpSocket(this);
    // 用于标记上传是否已经完成
    bool uploadFinished = false;
    
    connect(uploadSocket, &QTcpSocket::connected, this, [=, &uploadFinished]() {
        qDebug() << "Upload socket connected.";
        if (!sendStringAndWait(uploadSocket, "UPLOAD")) return;
        if (!sendStringAndWait(uploadSocket, fi.fileName())) return;
        if (!sendStringAndWait(uploadSocket, QString::number(fileSize))) return;
        
        qint64 bytesSent = 0;
        while (!uploadFile->atEnd()) {
            QByteArray chunk = uploadFile->read(4096);
            qDebug() << "Read chunk size:" << chunk.size();
            if (chunk.isEmpty())
                break;
            int written = uploadSocket->write(chunk);
            qDebug() << "Attempted to write chunk, written:" << written;
            if (written <= 0) {
                // 此处无需弹出 Unknown error 窗口，直接跳出即可
                qDebug() << "Write failed:" << uploadSocket->errorString();
                break;
            }
            bytesSent += written;
            ui->progressBar->setValue(bytesSent);
            if (!uploadSocket->waitForBytesWritten(5000)) {
                qDebug() << "Wait for bytes written failed:" << uploadSocket->errorString();
                break;
            }
        }
        qDebug() << "Total bytes sent:" << bytesSent;
        uploadSocket->flush();
        if (!uploadSocket->waitForBytesWritten(5000))
            qDebug() << "Flush wait error:" << uploadSocket->errorString();
        
        uploadFile->close();
        delete uploadFile;
    
        // 标记上传完成
        uploadFinished = true;
        
        if (uploadSocket->waitForReadyRead(5000)) {
            QByteArray resp = uploadSocket->readAll();
            QMessageBox::information(this, tr("Upload Response"), resp);
            qDebug() << "Server response:" << resp;
        } else {
            qDebug() << "No response from server.";
        }
        uploadSocket->disconnectFromHost();
    });
    
    // 错误处理：直接忽略 RemoteHostClosedError 和 UnknownSocketError
    connect(uploadSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this, [=, &uploadFinished](QAbstractSocket::SocketError error) {
                // 如果上传已完成，则不弹出任何错误
                if (uploadFinished)
                    return;
                // 过滤掉 RemoteHostClosedError 和 UnknownSocketError
                if (error == QAbstractSocket::RemoteHostClosedError ||
                    error == QAbstractSocket::UnknownSocketError)
                {
                    qDebug() << "Ignored error:" << uploadSocket->errorString();
                    return;
                }
                QMessageBox::critical(this, tr("Upload Error"), uploadSocket->errorString());
            });
    
    uploadSocket->connectToHost(SERVER_IP, PORT);
}

bool MainWindow::sendStringAndWait(QTcpSocket *socket, const QString &str)
{
    QByteArray data = str.toUtf8();
    data.append('\0');
    socket->write(data);
    if (!socket->waitForBytesWritten(3000)) {
        qDebug() << "sendStringAndWait failed for:" << str << socket->errorString();
        return false;
    }
    return true;
}

QString MainWindow::hashPassword(const QString &password, const QString &salt)
{
    QByteArray combined = password.toUtf8();
    combined.append(salt.toUtf8());
    
    SM3_CTX ctx;
    unsigned char digest[32];
    sm3_init(&ctx);
    sm3_update(&ctx, reinterpret_cast<const uint8_t *>(combined.data()), combined.size());
    sm3_finish(&ctx, digest);
    
    QString hashString;
    for (int i = 0; i < 32; i++) {
        hashString.append(QString("%1").arg(digest[i], 2, 16, QLatin1Char('0')));
    }
    return hashString;
}

