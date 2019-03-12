#include "index.h"
#include "ui_index.h"
#include <QListView>
#include <QStringListModel>
#include <package.h>

Index::Index(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Index)
{
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette pal = this->palette();
    pal.setBrush(QPalette::Background, QBrush(QPixmap("D:/QT_DOCUMENTS/Project_2018_11_07/Tcp_KeHuDuan/icon/image_02.jpg")));
    setPalette(pal);

    connect(this,SIGNAL(toEnterrRoom(pack_t)),
            &m_room,SLOT(enterrRoom(pack_t)));

    connect(&m_room,SIGNAL(toUpdateList()),
            this,SLOT(updateListFromRoom()));
    connect(this,SIGNAL(toUpdateListToRoom(pack_t)),
            &m_room,SLOT(updateList(pack_t)));

    connect(&m_room,SIGNAL(sendMsgToIndex(QString)),
            this,SLOT(sendMsgFromRoom(QString)));
    connect(this,SIGNAL(reciveChatToRoom(pack_t)),
            &m_room,SLOT(reciveChatFromIndex(pack_t)));

    connect(&m_room,SIGNAL(toKillUserInRoom()),
            this,SLOT(tellClientUserDead()));

    connect(this,SIGNAL(toTellThePort(int)),
            &m_room,SLOT(tellThePort(int)));

}

Index::~Index()
{
    delete ui;
}

//创建我的房间
void Index::on_pushButton_2_clicked()
{
    QString roomName = ui->lineEdit->text();
    emit toCreateRoom(roomName);
}

//退出房间
void Index::on_pushButton_3_clicked()
{
    emit toKillUserInClient();
    this->close();
}

//刷新房间
void Index::on_pushButton_clicked()
{
    emit toUpdateRoom();
}

void Index::updateRoom(pack_t pack)
{
    ui->listWidget->clear();
    QString caMsg = QString::fromLocal8Bit(pack.data);
    QStringList sl = caMsg.split('/');
    QStringList::iterator iter = sl.begin();
    for(;iter != sl.end();++iter){
        if(*iter != ""){
            ui->listWidget->addItem(*iter);
        }
    }
    qDebug() << "update updateRoom in index";
}

void Index::createRoom(pack_t pack)
{
    if(pack.type == TYPE_ROM_SUCCESS){
        this->hide();
        qDebug() << "pack.name:" << pack.name;
        QString myPortS = pack.name;
        int myPort(0);
        myPort = myPortS.toInt();
        qDebug() << "myPort:" << myPort;
        emit toTellThePort(myPort);
        m_room.show();
    }
}

void Index::entertRoom(pack_t pack)
{
    emit toEnterrRoom(pack);
}

void Index::updateListFromRoom()
{
    emit toUpdateListToClient();
}

void Index::updateListToRoom(pack_t pack)
{
    emit toUpdateListToRoom(pack);
}

void Index::sendMsgFromRoom(QString caMsg)
{
    emit sendMsgToClient(caMsg);
}

void Index::reciveChatFromClient(pack_t pack)
{
    emit reciveChatToRoom(pack);
}

void Index::tellClientUserDead()
{
    emit userDeadInRoom();
}

void Index::on_listWidget_doubleClicked(const QModelIndex &)
{
    QString roomName = ui->listWidget->currentItem()->text();
    pack_t pack;
    memset(&pack,0,sizeof(pack));
    pack.type = TYPE_ETR;
    QByteArray rName = roomName.toLocal8Bit();
    const char *cName = rName.data();
    qstrcpy(pack.data,cName);
    emit toEntertRoom(pack);
}
