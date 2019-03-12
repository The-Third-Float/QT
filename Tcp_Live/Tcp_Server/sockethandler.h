#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include "package.h"

class SocketHandler : public QObject
{
    Q_OBJECT
public:
    explicit SocketHandler(QObject *parent = 0);
    explicit SocketHandler(QTcpSocket*, QObject *parent = 0);
    QTcpSocket *getSocket(){return this->m_clientSocket;}

    void registUser(const pack_t &pack);                //用户注册
    void loginUser(const pack_t &pack);                 //用户登录

    void createRoom(const pack_t &pack);                //创建房间
    void updateRoom();                                  //刷新房间
    void entertRoom(const pack_t &pack);                //进入房间

    void updateList();                                  //刷新用户
    void chatInRoom(const pack_t &pack);                //用户聊天

    void distoyUser();                                  //用户注销
    void quitheUser();                                  //用户退房

signals:
    void sendToWindow(QString);
    void writeToMainThread(QTcpSocket*,pack_t,qint64);  //qint64 -> long int

public slots:
    void readyReadSlot();

private:
    QTcpSocket *m_clientSocket;                         //客户端
};

#endif // SOCKETHANDLER_H
