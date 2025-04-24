#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

namespace Ui {
class ClientMainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonSubmit_clicked();  // 注册/登录
    void on_pushButtonUpload_clicked();    // 文件上传
    void onSocketConnected();
    void onReadyRead();

private:
    Ui::ClientMainWindow *ui;
    QTcpSocket *tcpSocket;  // 用于注册/登录

    QString hashPassword(const QString &password, const QString &salt);
    // 发送字符串到 socket（附加 '\0'），并等待写入成功
    bool sendStringAndWait(QTcpSocket *socket, const QString &str);
};

#endif // MAINWINDOW_H

