#include "server.h"
#include "ui_server.h"
#include <QHostAddress>
#include <QTcpSocket>
#include "sockethandler.h"
#include "networkhelper.h"
#include <QThread>
#include <QDebug>

Server::Server(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);
    //注册元类型用于信号槽的参数通信
    qRegisterMetaType<pack_t>("pack_t");

    m_tcpServer = new QTcpServer(this);
    //新用户链接
    connect(m_tcpServer,SIGNAL(newConnection()),
            this,SLOT(newConnectionSlot()));

}


Server::~Server()
{
    delete ui;
}

//监听按钮
void Server::on_ListenPB_clicked()
{
    if(m_tcpServer->isListening()){
        ui->textEdit->append("已经启动监听");     //APPEND -> 换行插入
        ui->ListenPB->setEnabled(false);        //SETENABLED -> 不可点击
        return;
    }
    if(m_tcpServer->listen(QHostAddress("192.168.75.1"), 10000)){
        ui->textEdit->append("监听成功");
    }else{
        ui->textEdit->append("监听失败");
    }
}

//用户链接
void Server::newConnectionSlot()
{
    ui->textEdit->append("有新的客户端链接...");

    QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();
    QString ip = clientSocket->peerAddress().toString();
    QString port = QString::number(clientSocket->peerPort());

    QString adr;
    adr = adr + "ip: " + ip + "\tport: " + port;
    ui->textEdit->append(adr);

    //与客户端的通信对象
    SocketHandler *sh = new SocketHandler(clientSocket);
    connect(sh,SIGNAL(sendToWindow(QString)),
            this,SLOT(sendToWindowSlot(QString)));

    connect(sh,SIGNAL(writeToMainThread(QTcpSocket*,pack_t,qint64)),
            this,SLOT(writeToMainThreadSlot(QTcpSocket*,pack_t,qint64)));

    //开启一个新的线程来处理与客户端的数据交互
    QThread *th = new QThread(this);
    sh->moveToThread(th);
    th->start();

    //客户端连接断开时，销毁Socket对象
    connect(clientSocket,SIGNAL(disconnected()),
            sh,SLOT(deleteLater()));
    //Scoket对象销毁时，回收线程
    connect(sh,SIGNAL(destroyed(QObject*)),
            th,SLOT(terminate()));

    //将所有的客户端Socket加入到容器中管理
    NetworkHelper *instance = NetworkHelper::getInstance();
    instance->insertClient(sh);
//    instance->insertClientMap(sh);

}


void Server::sendToWindowSlot(QString text)
{
    ui->textEdit->append(text);
}


void Server::writeToMainThreadSlot(QTcpSocket *socket,
                                   pack_t pack,qint64 len)
{
    qDebug() << "writeToMainThreadSlot" ;
    socket->write((const char*)&pack, len);
}
