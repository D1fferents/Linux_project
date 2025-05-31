#ifndef SERVERMAINWINDOW_H
#define SERVERMAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <sqlite3.h>

namespace Ui {
class ServerMainWindow;
}

class ServerMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ServerMainWindow(QWidget *parent = nullptr);
    ~ServerMainWindow();

private slots:
    void onNewConnection();
    void onSocketReadyRead();
    void onSocketDisconnected();

private:
    Ui::ServerMainWindow *ui;
    QTcpServer *tcpServer;
    sqlite3 *db;

    void setupDatabase();
    // 读取以 '\0' 结束的字符串
    QString receiveString(QTcpSocket *socket);
    void processClient(QTcpSocket *clientSocket);
    void logMessage(const QString &message);
    // SM4 加密函数（示例中直接返回原数据）
    QByteArray sm4_encrypt(const QByteArray &plainData);
};

#endif // SERVERMAINWINDOW_H

