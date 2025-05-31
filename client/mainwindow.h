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
    void on_pushButtonSubmit_clicked();   // 注册/登录
    void on_pushButtonUpload_clicked();     // 文件上传
    void on_pushButtonDownload_clicked();   // 文件下载
    void onSocketConnected();
    void onReadyRead();

private:
    Ui::ClientMainWindow *ui;
    QTcpSocket *tcpSocket;  // 用于注册/登录

    // 用于下载时保存数据状态
    QByteArray m_downloadData;
    qint64 m_expectedFileSize;

    QString hashPassword(const QString &password, const QString &salt);
    // 发送字符串至 socket（附加 '\0'），等待写入完成
    bool sendStringAndWait(QTcpSocket *socket, const QString &str);
    // 同步接收以 '\0' 结束的字符串（仅用于读取命令回复或文件大小）
    QString receiveString(QTcpSocket *socket);

    // 辅助函数：检查目标文件是否已存在，若存在则在末尾增加 (n)
    QString getUniqueFileName(const QString &folder, const QString &fileName);
};

#endif // MAINWINDOW_H

