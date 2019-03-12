#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpServer>
#include "package.h"

namespace Ui {
class Server;
}

class Server : public QWidget
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);
    ~Server();

private slots:
    void on_ListenPB_clicked();     //监听按钮点击事件
    void newConnectionSlot();       //建立用户连接
    void sendToWindowSlot(QString);
    void writeToMainThreadSlot(QTcpSocket*,pack_t,qint64);

private:
    Ui::Server *ui;
    QTcpServer *m_tcpServer;
};

#endif // SERVER_H
