#ifndef INDEX_H
#define INDEX_H

#include <QWidget>
#include "room.h"
#include "package.h"

namespace Ui {
class Index;
}

class Index : public QWidget
{
    Q_OBJECT

public:
    explicit Index(QWidget *parent = 0);
    ~Index();

public slots:
    void updateRoom(pack_t);
    void createRoom(pack_t);
    void entertRoom(pack_t);
    void updateListFromRoom();
    void updateListToRoom(pack_t);
    void sendMsgFromRoom(QString);
    void reciveChatFromClient(pack_t);
    void tellClientUserDead();

signals:
    void toUpdateRoom();
    void toCreateRoom(QString);
    void toEntertRoom(pack_t);  //进入房间 -> 主页
    void toEnterrRoom(pack_t);  //进入房间 -> 房间
    void toUpdateListToClient();
    void toUpdateListToRoom(pack_t);
    void sendMsgToClient(QString);
    void reciveChatToRoom(pack_t);
    void toKillUserInClient();
    void userDeadInRoom();
    void toTellThePort(int);

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_listWidget_doubleClicked(const QModelIndex &index);

private:
    Ui::Index *ui;
    Room m_room;
};

#endif // INDEX_H
