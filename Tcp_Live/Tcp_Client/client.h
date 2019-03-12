#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include "index.h"

namespace Ui {
class Client;
}

class Client : public QWidget
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);
    ~Client();

public slots:
    void updateRoom();
    void createRoom(QString);
    void entertRoom(pack_t);
    void updateList();
    void sendMsgFromIndex(QString);
    void killUserInClient();
    void quitUserInClient();

signals:
    void toUpdateRoom(pack_t);
    void toCreateRoom(pack_t);
    void toEntertRoom(pack_t);
    void toUpdateList(pack_t);
    void toReciveChat(pack_t);
    void toKillUserInClient();

private slots:
    void on_BtnLogin_clicked();

    void on_BtnReg_clicked();
    void readyReadSlot();

private:
    Ui::Client *ui;
    QTcpSocket *m_clientSocket;
    Index m_IndexUi;
};

#endif // CLIENT_H
