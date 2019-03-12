#include "sockethandler.h"
#include "userhandler.h"
#include <QDebug>
#include "networkhelper.h"      //###############

SocketHandler::SocketHandler(QObject *parent)
    : QObject(parent)
{

}


SocketHandler::SocketHandler(QTcpSocket *clientSocket, QObject *parent)
    :m_clientSocket(clientSocket),QObject(parent)
{
    //readyRead -> 当Socket有数据时,发出信号
    connect(this->m_clientSocket,SIGNAL(readyRead()),
            this,SLOT(readyReadSlot()));
}


void SocketHandler::readyReadSlot()
{
    pack_t pack;
    memset(&pack,0,sizeof(pack));
    //读取数据包
    m_clientSocket->read((char*)&pack,sizeof(pack));
    //打印读到的数据信息
    qDebug() << pack.type << "\t" << pack.name << "\t" << pack.pswd;
    switch (pack.type) {
    case TYPE_REG:      //注册包
        registUser(pack);
        break;
    case TYPE_LOG:      //登录包
        loginUser(pack);
        break;
    case TYPE_ROM:      //开房包
        createRoom(pack);
        break;
    case TYPE_UPR:      //刷房包
        updateRoom();
        break;
    case TYPE_ETR:      //进房包
        entertRoom(pack);
        break;
    case TYPE_UPL:      //刷人包
        updateList();
        break;
    case TYPE_CAT:      //聊天包
        chatInRoom(pack);
        break;
    case TYPE_DIE:      //注销包
        distoyUser();
        break;
    case TYPE_QIT:      //退房包
        quitheUser();
        break;
    default:
        break;
    }
}

//用户注册
void SocketHandler::registUser(const pack_t &pack)
{
    UserHandler uh;
    QString qName = QString::fromLocal8Bit(pack.name);
    QString qPswd = QString::fromLocal8Bit(pack.pswd);
    User user(qName,qPswd);

    bool ret = uh.insertUser(user);
    pack_t packRet;
    memset(&packRet,0,sizeof(packRet));

    if(!ret){
        qDebug() << "注册失败";
        packRet.type = TYPE_REG_FAILURE;
    }else{
        qDebug() << "注册成功";
        packRet.type = TYPE_REG_SUCCESS;
    }

    //将处理完的结果发送到主线程并写回客户端
    emit writeToMainThread(this->m_clientSocket,
                           packRet,sizeof(packRet));
}

//用户登录
void SocketHandler::loginUser(const pack_t &pack)
{
    UserHandler uh;
    QString qName = QString::fromLocal8Bit(pack.name);
    QString qPswd = QString::fromLocal8Bit(pack.pswd);
    User user(qName,qPswd);

    NetworkHelper *instance = NetworkHelper::getInstance();

    bool ret = uh.checkUser(user);
    pack_t packRet;
    memset(&packRet,0,sizeof(packRet));
    if(!ret){
        qDebug() << "登录失败";
        packRet.type = TYPE_LOG_FAILURE;
    }else if(instance->isLogin(qName)){
        qDebug() << "重复登录";
        packRet.type = TYPE_LOG_FAILURE;
    }else{
        qDebug() << "登录成功";
        packRet.type = TYPE_LOG_SUCCESS;
        Account account;
        account.flag = 0;
        account.roomName = "UNKNOWN";
        account.socket = this->m_clientSocket;

        instance->insertClientMap(qName,account);
    }

    emit writeToMainThread(this->m_clientSocket,
                           packRet,sizeof(packRet));
}

//用户开房
void SocketHandler::createRoom(const pack_t &pack)
{
    qDebug() << "准备开房";
    pack_t packRom;
    memset(&packRom,0,sizeof(packRom));
//    QString caMsg = QString::fromLocal8Bit(pack.data);
    QString caMsg = pack.data;
    QStringList sl = caMsg.split('/');
    QString pack_roomName = sl.at(0);
    if(pack_roomName != NULL){
        int flag = 0;
        NetworkHelper *instance = NetworkHelper::getInstance();
        QMap<QString,Account_t>::iterator iter;
        iter = instance->m_clientsMap.begin();
        for(;iter != instance->m_clientsMap.end();++iter){
            if(iter.value().roomName == pack_roomName){
                flag = 1;
                qDebug() << "开房失败";
                packRom.type = TYPE_ROM_FAILURE;
                break;
            }
        }
        if(!flag){
            iter = instance->m_clientsMap.begin();
            for(;iter != instance->m_clientsMap.end();++iter){
                if(iter.value().socket == this->m_clientSocket){
                    iter.value().flag = 1;
                    iter.value().roomName = pack_roomName;
                    QString pot = QString::number(iter.value().socket->peerPort());
                    QByteArray QBytePot = pot.toLocal8Bit();
                    const char *charPot = QBytePot.data();
                    qstrcpy(packRom.name,charPot);
                    packRom.type = TYPE_ROM_SUCCESS;
                    break;
                }
            }
            //进入聊天室后发送房间内的成员列表
            iter = instance->m_clientsMap.begin();
            QString data = "";
            for(;iter != instance->m_clientsMap.end();++iter){
                if(iter.value().roomName == pack_roomName){
                    QString msg = iter.key() + '/';
                    if(!msg.isEmpty()){
                        data += msg;
                    }
                }
            }
            QByteArray QByteMsg = data.toLocal8Bit();
            const char* charMsg = QByteMsg.data();
            qstrcpy(packRom.data,charMsg);
        }
    }

    emit writeToMainThread(this->m_clientSocket,
                           packRom,sizeof(packRom));
}

//刷新房间
void SocketHandler::updateRoom()
{
    qDebug() << "recive 刷新开房请求";
    NetworkHelper *instance = NetworkHelper::getInstance();

    pack_t packRet;
    memset(&packRet,0,sizeof(packRet));
    packRet.type = TYPE_UPR_SUCCESS;

    QMap<QString,Account_t>::iterator iter;
    iter = instance->m_clientsMap.begin();
    QString data = "";
    for(;iter != instance->m_clientsMap.end();++iter){
        if(iter.value().flag){
            QString msg = iter.value().roomName + '/';
            if(!msg.isEmpty()){
                data += msg;
            }
        }
    }

    QByteArray QByteMsg = data.toLocal8Bit();
    const char* charMsg = QByteMsg.data();
    qstrcpy(packRet.data,charMsg);

    emit writeToMainThread(this->m_clientSocket,
                           packRet,sizeof(packRet));
}

void SocketHandler::entertRoom(const pack_t &pack)
{
    qDebug() << "群众查房";
    NetworkHelper *instance = NetworkHelper::getInstance();
    pack_t packRet;
    memset(&packRet,0,sizeof(packRet));

//    QString caMsg = QString::fromLocal8Bit(pack.data);
    QString caMsg = pack.data;

    QStringList sl = caMsg.split('/');
    QString pack_roomName = sl.at(0);

    packRet.type = TYPE_ETR_SUCCESS;

    QMap<QString,Account_t>::iterator iter;
    iter = instance->m_clientsMap.begin();
    //在列表中更新用户信息中的房间信息
    for(;iter != instance->m_clientsMap.end();++iter){
        if(this->m_clientSocket == iter.value().socket){
            iter.value().roomName = pack_roomName;
            break;
        }
    }
    //把在房间中的所有用户列出来
    iter = instance->m_clientsMap.begin();
    QString data = "";
    QString dota = "";
    for(;iter != instance->m_clientsMap.end();++iter){
        if(iter.value().roomName == pack_roomName){
            QString msg = iter.key() + '/';
            if(!msg.isEmpty()){
                data += msg;
            }
            if(iter.value().flag == 1){
                QString pot = QString::number(iter.value().socket->peerPort());
                QByteArray QBytePot = pot.toLocal8Bit();
                const char *charPot = QBytePot.data();
                qstrcpy(packRet.pswd,charPot);
                QString msg = iter.value().roomName;
                if(!msg.isEmpty()){
                    dota += msg;
                }
            }
        }
    }

    QByteArray QByteNme = dota.toLocal8Bit();
    QByteArray QByteMsg = data.toLocal8Bit();

    const char *charNme = QByteNme.data();
    const char *charMsg = QByteMsg.data();

    qstrcpy(packRet.name,charNme);
    qstrcpy(packRet.data,charMsg);

    qDebug() << "packRet.name" << packRet.name << "\t"
             << "packRet.pswd" << packRet.pswd << "\t"
             << "packRet.data" << packRet.data;

    emit writeToMainThread(this->m_clientSocket,
                           packRet,sizeof(packRet));
}

void SocketHandler::updateList()
{
    NetworkHelper *instance = NetworkHelper::getInstance();

    pack_t packRet;
    memset(&packRet,0,sizeof(packRet));

    packRet.type = TYPE_UPL_SUCCESS;

    QMap<QString,Account_t>::iterator iter;
    iter = instance->m_clientsMap.begin();
    QString roomName = "";
    for(;iter != instance->m_clientsMap.end();++iter){
        if(this->m_clientSocket == iter.value().socket){
            roomName = iter.value().roomName;
            break;
        }
    }

    iter = instance->m_clientsMap.begin();
    QString data = "";
    for(;iter != instance->m_clientsMap.end();++iter){
        if(iter.value().roomName == roomName){
            QString msg = iter.key() + '/';

            if(!msg.isEmpty()){
                data += msg;
            }
        }
    }
    QByteArray QByteMsg = data.toLocal8Bit();
    const char *charMsg = QByteMsg.data();
    qstrcpy(packRet.data,charMsg);

    emit writeToMainThread(this->m_clientSocket,
                           packRet,sizeof(packRet));
}

void SocketHandler::chatInRoom(const pack_t &pack)
{
    qDebug() << "recive massage from user";
    NetworkHelper *instance = NetworkHelper::getInstance();
    pack_t packRet;
    memset(&packRet,0,sizeof(packRet));

    packRet.type = TYPE_CAT_SUCCESS;

    QString senderName = "";
    QString roomName = "";
    QMap<QString,Account_t>::iterator iter;
    iter = instance->m_clientsMap.begin();
    for(;iter != instance->m_clientsMap.end();++iter){
        if(this->m_clientSocket == iter.value().socket){
            senderName = iter.key();
            roomName = iter.value().roomName;
            break;
        }
    }

    qDebug() << "Name:\t" << senderName;

    QString caMsg = senderName;
    caMsg = "【用户】 " + caMsg + " 说: " + pack.data;

    qDebug() << "caMsg:\t" << caMsg;
    QStringList sl = caMsg.split('/');
    QString pack_caMsg = sl.at(0);

    QByteArray QByteMsg = pack_caMsg.toLocal8Bit();
    const char *charMsg = QByteMsg.data();
    qstrcpy(packRet.data,charMsg);

    iter = instance->m_clientsMap.begin();
    for(;iter != instance->m_clientsMap.end();++iter){
        if(iter.value().roomName == roomName){
            emit writeToMainThread(iter.value().socket,
                                   packRet,sizeof(packRet));
        }
    }
}

void SocketHandler::distoyUser()
{
    qDebug() << "Recive distoryUser";

    NetworkHelper *instance = NetworkHelper::getInstance();
    //查询用户名 -> userName
    QString userName = "";
    QMap<QString,Account_t>::iterator iter;
    iter = instance->m_clientsMap.begin();
    for(;iter != instance->m_clientsMap.end();++iter){
        if(this->m_clientSocket == iter.value().socket){
            userName = iter.key();
            break;
        }
    }

    QMap<QString,Account_t>::iterator key = instance->m_clientsMap.find(userName);
    if(key != instance->m_clientsMap.end()){
        instance->m_clientsMap.erase(key);
        qDebug() << "DELETE";
    }
}

void SocketHandler::quitheUser()
{
    qDebug() << "Recive distoryUser";
    int flag = 0;
    NetworkHelper *instance = NetworkHelper::getInstance();
    //查询用户名 -> userName
    QString userName = "";
    QMap<QString,Account_t>::iterator iter;
    iter = instance->m_clientsMap.begin();
    for(;iter != instance->m_clientsMap.end();++iter){
        if(this->m_clientSocket == iter.value().socket){
            userName = iter.key();
            if(iter.value().flag == 1){
                flag = 1;
            }
            break;
        }
    }

    Account_t account;
    account.flag = 0;
    account.roomName = "unknown";
    account.socket = this->m_clientSocket;

    QMap<QString,Account_t>::iterator key = instance->m_clientsMap.find(userName);
    if(key != instance->m_clientsMap.end()){
        instance->m_clientsMap.erase(key);
        qDebug() << "DELETE USER IN ROOM";
    }

    if(flag = 0){
        instance->insertClientMap(userName,account);
    }
}
