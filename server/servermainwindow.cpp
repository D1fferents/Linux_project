#include "servermainwindow.h"
#include "ui_server.h"

#include <QMessageBox>
#include <QHostAddress>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QThread>
#include <QDebug>
#include <cstring>

#define PORT 8080

ServerMainWindow::ServerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ServerMainWindow),
    tcpServer(new QTcpServer(this)),
    db(nullptr)
{
    ui->setupUi(this);
    setupDatabase();
    if (!tcpServer->listen(QHostAddress::Any, PORT)) {
        QMessageBox::critical(this, "Error", "Unable to start server");
        close();
        return;
    }
    logMessage("Server started on port " + QString::number(PORT));
    connect(tcpServer, &QTcpServer::newConnection, this, &ServerMainWindow::onNewConnection);
}

ServerMainWindow::~ServerMainWindow()
{
    if (db)
        sqlite3_close(db);
    delete ui;
}

void ServerMainWindow::setupDatabase()
{
    if (sqlite3_open("users.db", &db) != SQLITE_OK) {
        logMessage("Unable to open database");
        return;
    }
    const char *createTableSQL = "CREATE TABLE IF NOT EXISTS users ("
                                 "username TEXT PRIMARY KEY, "
                                 "password TEXT NOT NULL);";
    char *errMsg = nullptr;
    if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        logMessage("Failed to create table: " + QString(errMsg));
        sqlite3_free(errMsg);
    }
}

void ServerMainWindow::onNewConnection()
{
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &ServerMainWindow::onSocketReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &ServerMainWindow::onSocketDisconnected);
    logMessage("New connection from: " + clientSocket->peerAddress().toString());
}

void ServerMainWindow::onSocketReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket)
        processClient(clientSocket);
}

void ServerMainWindow::onSocketDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket)
        clientSocket->deleteLater();
}

QString ServerMainWindow::receiveString(QTcpSocket *socket)
{
    QByteArray data;
    if (socket->bytesAvailable() == 0)
        socket->waitForReadyRead(3000);
    while (socket->bytesAvailable() > 0) {
        char ch;
        if (socket->read(&ch, 1) != 1)
            break;
        if (ch == '\0')
            break;
        data.append(ch);
    }
    return QString::fromUtf8(data);
}

void ServerMainWindow::processClient(QTcpSocket *clientSocket)
{
    QString operation = receiveString(clientSocket);
    logMessage("Received operation: " + operation);
    
    QString response;
    if (operation == "REGISTER" || operation == "LOGIN") {
        QString username = receiveString(clientSocket);
        QString passwordHash = receiveString(clientSocket);
        if (operation == "REGISTER") {
            const char *sql = "INSERT INTO users (username, password) VALUES (?, ?);";
            sqlite3_stmt *stmt = nullptr;
            if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
                response = "Registration failed: Database error: " + QString(sqlite3_errmsg(db));
            } else {
                sqlite3_bind_text(stmt, 1, username.toUtf8().constData(), -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 2, passwordHash.toUtf8().constData(), -1, SQLITE_STATIC);
                if (sqlite3_step(stmt) != SQLITE_DONE) {
                    response = "Registration failed: " + QString(sqlite3_errmsg(db));
                } else {
                    response = "Registration successful";
                }
                sqlite3_finalize(stmt);
            }
        } else {
            const char *sql = "SELECT password FROM users WHERE username = ?;";
            sqlite3_stmt *stmt = nullptr;
            if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
                response = "Login failed: Database error: " + QString(sqlite3_errmsg(db));
            } else {
                sqlite3_bind_text(stmt, 1, username.toUtf8().constData(), -1, SQLITE_STATIC);
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    const char *dbPassword = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                    if (dbPassword && passwordHash == QString(dbPassword))
                        response = "Login successful";
                    else
                        response = "Login failed: Incorrect password";
                } else {
                    response = "Login failed: Username does not exist";
                }
                sqlite3_finalize(stmt);
            }
        }
    }
    else if (operation == "UPLOAD") {
        QString fileName = receiveString(clientSocket);
        logMessage("Uploading file: " + fileName);
        QString fileSizeStr = receiveString(clientSocket);
        bool ok = false;
        qint64 fileSize = fileSizeStr.toLongLong(&ok);
        if (!ok || fileSize <= 0) {
            response = "Upload failed: Invalid file size";
        } else {
            QDir uploadDir("/home/roselia/project/server/uploads");
            if (!uploadDir.exists())
                uploadDir.mkpath(".");
            QString filePath = uploadDir.absoluteFilePath(fileName);
            QFile file(filePath);
            if (!file.open(QIODevice::WriteOnly)) {
                response = "Upload failed: Cannot open file for writing";
            } else {
                qint64 bytesReceived = 0;
                while (bytesReceived < fileSize) {
                    int bytesToRead = fileSize - bytesReceived;
                    if (clientSocket->waitForReadyRead(5000)) {
                        QByteArray chunk = clientSocket->read(bytesToRead);
                        int chunkSize = chunk.size();
                        logMessage(QString("Received chunk size: %1 (expected %2)").arg(chunkSize).arg(bytesToRead));
                        if (chunk.isEmpty())
                            break;
                        file.write(chunk);
                        bytesReceived += chunkSize;
                        logMessage(QString("Total received: %1/%2 bytes").arg(bytesReceived).arg(fileSize));
                    } else {
                        break;
                    }
                }
                file.close();
                if (bytesReceived == fileSize)
                    response = "Upload successful";
                else
                    response = "Upload failed: Incomplete file received. Expected " +
                               QString::number(fileSize) + " bytes, received " +
                               QString::number(bytesReceived) + " bytes.";
            }
        }
    }
    else if (operation == "DOWNLOAD") {
        // 处理下载请求
        QString fileName = receiveString(clientSocket);
        logMessage("Download request for file: " + fileName);
        // 使用固定的上传目录绝对路径
        QDir uploadDir("/home/roselia/project/server/uploads");
        QString filePath = uploadDir.absoluteFilePath(fileName);
        logMessage("Looking for file at: " + filePath);
        QFile file(filePath);
        if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
            QByteArray zero = "0";
            zero.append('\0');
            clientSocket->write(zero);
            clientSocket->waitForBytesWritten(5000);
            clientSocket->disconnectFromHost();
            return;
        }
        QByteArray fileData = file.readAll();
        file.close();
        QByteArray encryptedData = sm4_encrypt(fileData);
        QString sizeStr = QString::number(encryptedData.size());
        QByteArray sizeBytes = sizeStr.toUtf8();
        sizeBytes.append('\0');
        clientSocket->write(sizeBytes);
        clientSocket->waitForBytesWritten(5000);
        clientSocket->write(encryptedData);
        clientSocket->waitForBytesWritten(5000);
        clientSocket->flush();
        QThread::msleep(1500);  // 延时1.5秒，确保发送完毕
        clientSocket->disconnectFromHost();
        return;
    }
    else {
        response = "Unknown operation";
    }
    
    logMessage("Response: " + response);
    QByteArray respData = response.toUtf8();
    respData.append('\0');
    clientSocket->write(respData);
    clientSocket->disconnectFromHost();
}

QByteArray ServerMainWindow::sm4_encrypt(const QByteArray &plainData)
{
    // 直接返回原数据作为示例
    return plainData;
}

void ServerMainWindow::logMessage(const QString &message)
{
    ui->textBrowserLog->append(message);
    qDebug() << message;
}

