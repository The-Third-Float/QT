#include "client.h"
#include "ui_client.h"
#include <QHostAddress>
#include "package.h"
#include <QMessageBox>

Client::Client(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);
    setAutoFillBackground(true);
    QPalette pal = this->palette();
    pal.setBrush(QPalette::Background, QBrush(QPixmap("D:/QT_DOCUMENTS/Project_2018_11_07/Tcp_KeHuDuan/icon/image_01.jpg")));
    setPalette(pal);
    m_clientSocket = new QTcpSocket(this);
    m_clientSocket->connectToHost(QHostAddress("192.168.75.1"),10000);
    connect(this->m_clientSocket,SIGNAL(readyRead()),
            this,SLOT(readyReadSlot()));

    connect(&m_IndexUi,SIGNAL(toUpdateRoom()),
            this,SLOT(updateRoom()));
    connect(this,SIGNAL(toUpdateRoom(pack_t)),
            &m_IndexUi,SLOT(updateRoom(pack_t)));

    connect(&m_IndexUi,SIGNAL(toCreateRoom(QString)),
            this,SLOT(createRoom(QString)));
    connect(this,SIGNAL(toCreateRoom(pack_t)),
            &m_IndexUi,SLOT(createRoom(pack_t)));

    connect(&m_IndexUi,SIGNAL(toEntertRoom(pack_t)),
            this,SLOT(entertRoom(pack_t)));
    connect(this,SIGNAL(toEntertRoom(pack_t)),
            &m_IndexUi,SLOT(entertRoom(pack_t)));

    connect(&m_IndexUi,SIGNAL(toUpdateListToClient()),
            this,SLOT(updateList()));
    connect(this,SIGNAL(toUpdateList(pack_t)),
            &m_IndexUi,SLOT(updateListToRoom(pack_t)));

    connect(&m_IndexUi,SIGNAL(sendMsgToClient(QString)),
            this,SLOT(sendMsgFromIndex(QString)));
    connect(this,SIGNAL(toReciveChat(pack_t)),
            &m_IndexUi,SLOT(reciveChatFromClient(pack_t)));

    connect(this,SIGNAL(toKillUserInClient()),
            this,SLOT(killUserInClient()));
    connect(&m_IndexUi,SIGNAL(toKillUserInClient()),
            this,SLOT(killUserInClient()));
    connect(&m_IndexUi,SIGNAL(userDeadInRoom()),
            this,SLOT(quitUserInClient()));
}


Client::~Client()
{
    emit toKillUserInClient();
    delete ui;
}

//更新房间信息
void Client::updateRoom()
{
    qDebug() << "send updateRoom in Client";
    pack_t packLog;
    memset(&packLog,0,sizeof(packLog));
    packLog.type = TYPE_UPR;
    m_clientSocket->write((const char*)&packLog,sizeof(packLog));
}

void Client::createRoom(QString roomName)
{
    qDebug() << "send createRoom in Client";
    pack_t packLog;
    memset(&packLog,0,sizeof(packLog));

    packLog.type = TYPE_ROM;
    QString m_roomName = roomName + '/';
//    QByteArray str = m_roomName.toLatin1();
    QByteArray str = m_roomName.toUtf8();

    strcpy(packLog.data, str.data());
    qDebug() << packLog.data << "\t" << str << "\t" << str.data();
    m_clientSocket->write((const char*)&packLog,sizeof(packLog));
}

void Client::entertRoom(pack_t pack)
{
    qDebug() << "send enterRoom in Client";
    m_clientSocket->write((const char*)&pack,sizeof(pack));
}

void Client::updateList()
{
    qDebug() << "update chatroom information";
    pack_t packLog;
    memset(&packLog,0,sizeof(packLog));

    packLog.type = TYPE_UPL;

    m_clientSocket->write((const char*)&packLog,sizeof(packLog));
}

void Client::sendMsgFromIndex(QString caMsg)
{
    qDebug() << "send massage to server from user";
    pack_t packRet;
    memset(&packRet,0,sizeof(packRet));

    packRet.type = TYPE_CAT;
    QString hcaMsg = caMsg + '/';

    QByteArray str = hcaMsg.toUtf8();

    strcpy(packRet.data,str.data());

    m_clientSocket->write((const char*)&packRet,sizeof(packRet));
}

void Client::killUserInClient()
{
    qDebug() << "start killing user";

    pack_t packLog;
    memset(&packLog,0,sizeof(packLog));

    packLog.type = TYPE_DIE;

    m_clientSocket->write((const char*)&packLog,sizeof(packLog));
}

void Client::quitUserInClient()
{
    qDebug() << "start quitting user";

    pack_t packLog;
    memset(&packLog,0,sizeof(packLog));

    packLog.type = TYPE_QIT;

    m_clientSocket->write((const char*)&packLog,sizeof(packLog));
}

void Client::on_BtnLogin_clicked()
{
    QString qName = ui->nameLE->text();
    QString qPswd = ui->pswdLE->text();

    pack_t packLog;
    memset(&packLog,0,sizeof(packLog));

    packLog.type = TYPE_LOG;

    QByteArray name = qName.toLocal8Bit();
    QByteArray pswd = qPswd.toLocal8Bit();

    const char *cName = name.data();
    const char *cPswd = pswd.data();

    qstrcpy(packLog.name,cName);
    qstrcpy(packLog.pswd,cPswd);

    m_clientSocket->write((const char*)&packLog,sizeof(packLog));
}


void Client::on_BtnReg_clicked()
{
    QString qName = ui->nameLE->text();
    QString qPswd = ui->pswdLE->text();

    pack_t packReg;
    memset(&packReg,0,sizeof(packReg));

    packReg.type = TYPE_REG;

    QByteArray name = qName.toLocal8Bit();
    QByteArray pswd = qPswd.toLocal8Bit();

    const char *cName = name.data();
    const char *cPswd = pswd.data();

    qstrcpy(packReg.name,cName);
    qstrcpy(packReg.pswd,cPswd);

    m_clientSocket->write((const char*)&packReg,sizeof(packReg));
}


void Client::readyReadSlot()
{
    pack_t pack;
    memset(&pack,0,sizeof(pack));
    this->m_clientSocket->read((char*)&pack,sizeof(pack));
    switch (pack.type) {
    case TYPE_LOG_FAILURE:
        QMessageBox::critical(this,"登录信息","登录失败\t\t");
        break;
    case TYPE_LOG_SUCCESS:
        QMessageBox::information(this,"登录信息","登录成功\t\t");
        m_IndexUi.show();
        this->hide();
        break;
    case TYPE_REG_FAILURE:
        QMessageBox::critical(this,"注册信息","注册失败\t\t");
        break;
    case TYPE_REG_SUCCESS:
        QMessageBox::information(this,"注册信息","注册成功\t\t");
        break;
    case TYPE_ROM_FAILURE:
        QMessageBox::critical(this,"开房信息","开房失败\t\t");
        break;
    case TYPE_ROM_SUCCESS:
        QMessageBox::information(this,"开房信息","开房成功\t\t");
        emit toCreateRoom(pack);
        break;
    case TYPE_UPR_SUCCESS:
        emit toUpdateRoom(pack);
        break;
    case TYPE_ETR_SUCCESS:
        emit toEntertRoom(pack);
        break;
    case TYPE_UPL_SUCCESS:
        emit toUpdateList(pack);
        break;
    case TYPE_CAT_SUCCESS:
        emit toReciveChat(pack);
        break;
    default:
        break;
    }
}
