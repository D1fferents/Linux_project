#include "mainwindow.h"
#include "ui_client.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QInputDialog>
#include <QByteArray>
#include <QDebug>
#include <QThread>
#include <QSharedPointer>
#include <gmssl/sm3.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define SALT "random_salt"

// 模拟 SM4 解密函数（实际工程中请调用真正的 SM4 解密接口）
QByteArray sm4_decrypt(const QByteArray &encryptedData)
{
    // 此处直接返回原数据作为示例
    return encryptedData;
}

// 辅助函数：给定目标文件夹和文件名，返回唯一的文件名
QString MainWindow::getUniqueFileName(const QString &folder, const QString &fileName)
{
    QFileInfo info(fileName);
    QString baseName = info.completeBaseName();  // 包含所有的基础名
    QString extension = info.suffix();             // 后缀，如 "c"
    QString newName = fileName;
    int count = 1;
    while (QFile::exists(folder + "/" + newName)) {
        newName = baseName + "(" + QString::number(count) + ")";
        if (!extension.isEmpty())
            newName += "." + extension;
        count++;
    }
    return newName;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientMainWindow),
    tcpSocket(new QTcpSocket(this)),
    m_expectedFileSize(0)
{
    ui->setupUi(this);
    connect(tcpSocket, &QTcpSocket::connected, this, &MainWindow::onSocketConnected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
    // 下载按钮连接槽（确保 UI 中存在 pushButtonDownload）
    connect(ui->pushButtonDownload, &QPushButton::clicked, this, &MainWindow::on_pushButtonDownload_clicked);
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
    qDebug() << "File size:" << fileSize;
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(fileSize);
    ui->progressBar->setValue(0);
    
    QTcpSocket *uploadSocket = new QTcpSocket(this);
    bool uploadFinished = false;
    
    connect(uploadSocket, &QTcpSocket::connected, this, [=, &uploadFinished]() {
        qDebug() << "Upload socket connected.";
        if (!sendStringAndWait(uploadSocket, "UPLOAD"))
            return;
        if (!sendStringAndWait(uploadSocket, fi.fileName()))
            return;
        if (!sendStringAndWait(uploadSocket, QString::number(fileSize)))
            return;
        
        qint64 bytesSent = 0;
        while (!uploadFile->atEnd()) {
            QByteArray chunk = uploadFile->read(4096);
            qDebug() << "Read chunk size:" << chunk.size();
            if (chunk.isEmpty())
                break;
            int written = uploadSocket->write(chunk);
            qDebug() << "Attempted to write chunk, written:" << written;
            if (written <= 0) {
                qDebug() << "Write failed:" << uploadSocket->errorString();
                break;
            }
            bytesSent += written;
            ui->progressBar->setValue(bytesSent);
            if (!uploadSocket->waitForBytesWritten(5000)) {
                qDebug() << "waitForBytesWritten failed:" << uploadSocket->errorString();
                break;
            }
        }
        qDebug() << "Total bytes sent:" << bytesSent;
        uploadSocket->flush();
        if (!uploadSocket->waitForBytesWritten(5000))
            qDebug() << "Flush wait error:" << uploadSocket->errorString();
        
        uploadFile->close();
        delete uploadFile;
    
        uploadFinished = true;
        
        if (uploadSocket->waitForReadyRead(5000)) {
            QByteArray resp = uploadSocket->readAll();
            QMessageBox::information(this, "Upload Response", resp);
            qDebug() << "Server response:" << resp;
        } else {
            qDebug() << "No response from server.";
        }
        uploadSocket->disconnectFromHost();
    });
    
    connect(uploadSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this, [=, &uploadFinished](QAbstractSocket::SocketError error) {
                if (uploadFinished)
                    return;
                if (error == QAbstractSocket::RemoteHostClosedError ||
                    error == QAbstractSocket::UnknownSocketError) {
                    qDebug() << "Ignored upload error:" << uploadSocket->errorString();
                    return;
                }
                QMessageBox::critical(this, "Upload Error", uploadSocket->errorString());
            });
    
    uploadSocket->connectToHost(SERVER_IP, PORT);
}

void MainWindow::on_pushButtonDownload_clicked()
{
    // 用户输入要下载的文件名
    bool ok;
    QString fileName = QInputDialog::getText(this, tr("Download File"),
                                             tr("Enter file name:"), QLineEdit::Normal, "", &ok);
    if (!ok || fileName.isEmpty())
        return;
    
    // 获取当前用户名，并在客户端当前目录下创建以该用户名命名的文件夹
    QString currentUsername = ui->lineEditUsername->text();
    QString basePath = QDir::currentPath();
    QString userFolder = basePath + "/" + currentUsername;
    QDir dir;
    if (!dir.exists(userFolder)) {
        if (!dir.mkpath(userFolder)) {
            QMessageBox::warning(this, tr("Download Error"), tr("Failed to create user folder."));
            return;
        }
    }
    // 自动生成保存路径，如果文件已存在，生成唯一名称
    QString saveFileName = fileName;
    if(QFile::exists(userFolder + "/" + fileName))
        saveFileName = getUniqueFileName(userFolder, fileName);
    QString savePath = userFolder + "/" + saveFileName;
    qDebug() << "Download file will be saved to:" << savePath;

    // 清空前次下载数据
    m_downloadData.clear();
    m_expectedFileSize = 0;

    QTcpSocket *downloadSocket = new QTcpSocket(this);

    // 在连接建立后发送下载命令及文件名
    connect(downloadSocket, &QTcpSocket::connected, this, [=]() {
        qDebug() << "Download socket connected.";
        if (!sendStringAndWait(downloadSocket, "DOWNLOAD"))
            return;
        if (!sendStringAndWait(downloadSocket, fileName))
            return;
    });

    // 异步接收数据
    connect(downloadSocket, &QTcpSocket::readyRead, this, [=]() {
        QByteArray chunk = downloadSocket->readAll();
        m_downloadData.append(chunk);
        qDebug() << "Received data, total bytes so far:" << m_downloadData.size();
        // 第一次接收时，提取以 '\0' 分隔的文件大小信息
        if (m_expectedFileSize == 0) {
            int pos = m_downloadData.indexOf('\0');
            if (pos != -1) {
                QString sizeStr = QString::fromUtf8(m_downloadData.left(pos));
                bool ok;
                m_expectedFileSize = sizeStr.toLongLong(&ok);
                if (!ok || m_expectedFileSize <= 0) {
                    QMessageBox::warning(this, tr("Download Error"),
                                         tr("Invalid file size received or file not found."));
                    downloadSocket->disconnectFromHost();
                    downloadSocket->deleteLater();
                    return;
                }
                qDebug() << "Parsed expected file size:" << m_expectedFileSize;
                // 移除已读取的文件大小信息及结束符
                m_downloadData.remove(0, pos + 1);
                ui->progressBar->setMinimum(0);
                ui->progressBar->setMaximum(m_expectedFileSize);
            }
        }
        // 可以选择实时更新
        ui->progressBar->setValue(m_downloadData.size());
    });

    // 断开时处理下载结果
    connect(downloadSocket, &QTcpSocket::disconnected, this, [=]() {
        if (m_downloadData.size() != m_expectedFileSize) {
            QMessageBox::warning(this, tr("Download Error"),
                                 tr("Incomplete file received. Expected %1 bytes, received %2 bytes.")
                                 .arg(m_expectedFileSize).arg(m_downloadData.size()));
        } else {
            ui->progressBar->setValue(m_expectedFileSize);
            QByteArray decryptedData = sm4_decrypt(m_downloadData);
            QFile outFile(savePath);
            if (!outFile.open(QIODevice::WriteOnly)) {
                QMessageBox::warning(this, tr("Download Error"), tr("Cannot open file for writing."));
            } else {
                outFile.write(decryptedData);
                outFile.close();
                QMessageBox::information(this, tr("Download"),
                                         tr("File downloaded and decrypted successfully.\nSaved to: %1").arg(savePath));
            }
        }
        downloadSocket->deleteLater();
    });

    connect(downloadSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this, [=](QAbstractSocket::SocketError error) {
                if (error == QAbstractSocket::RemoteHostClosedError ||
                    error == QAbstractSocket::UnknownSocketError) {
                    qDebug() << "Ignored download error:" << downloadSocket->errorString();
                    return;
                }
                QMessageBox::critical(this, tr("Download Error"), downloadSocket->errorString());
                downloadSocket->deleteLater();
            });

    downloadSocket->connectToHost(SERVER_IP, PORT);
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

QString MainWindow::receiveString(QTcpSocket *socket)
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

QString MainWindow::hashPassword(const QString &password, const QString &salt)
{
    QByteArray combined = password.toUtf8();
    combined.append(salt.toUtf8());

    SM3_CTX ctx;
    unsigned char digest[32];
    sm3_init(&ctx);
    sm3_update(&ctx, reinterpret_cast<const uint8_t*>(combined.data()), combined.size());
    sm3_finish(&ctx, digest);

    QString hashString;
    for (int i = 0; i < 32; i++) {
        hashString.append(QString("%1").arg(digest[i], 2, 16, QLatin1Char('0')));
    }
    return hashString;
}

