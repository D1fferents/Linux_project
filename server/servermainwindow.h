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
    // 以 '\0' 为结束符读取字符串
    QString receiveString(QTcpSocket *socket);
    void processClient(QTcpSocket *clientSocket);
    void logMessage(const QString &message);
};

#endif // SERVERMAINWINDOW_H

