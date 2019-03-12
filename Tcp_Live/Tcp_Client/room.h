#ifndef ROOM_H
#define ROOM_H

#include <QWidget>
#include <QUdpSocket>
#include <QCamera>
#include "VideoSuface.h"
#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>
#include "screen.h"
#include <QTcpSocket>
#include <QMap>
#include "package.h"
#include <QCloseEvent>
#include "danmu.h"

//typedef struct Account{
//    QString     roomName;
//    QTcpSocket *socket;
//}Account_t;

namespace Ui {
class Room;
}


class Room : public QWidget
{
    Q_OBJECT

public:
    explicit Room(QWidget *parent = 0);
    ~Room();

    void cleanTheRoom();

signals:
    void toUpdateList();
    void sendMsgToIndex(QString);
    void toKillUserInRoom();
    void toTellThePort(int);
//    void toScreenTimer();

private slots:
    void frameChangedSlot(QVideoFrame);
    void captureDataFromDevice();
    void onReadyReadAudio();
    void enterrRoom(pack_t);
    void updateList(pack_t);
    void reciveChatFromIndex(pack_t);
    void closeEvent(QCloseEvent *event);
    void tellThePort(int);

    void readyReadSlot();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::Room     *ui;

    QUdpSocket   *m_readSocket;
    QCamera      *m_camera;
    VideoSuface  *m_vs;
    QUdpSocket   *m_writeSocket;
    Screen       *m_screen;

    QUdpSocket   *m_socketAudioRead;
    QAudioInput  *m_audioInput;
    QIODevice    *m_audioInputIODevice;
    QAudioOutput *m_audioOutput;
    QIODevice    *m_audioOutputIODevice;
    QUdpSocket   *m_socketAudioWrite;

    int           m_myPort;
    int           m_hostPort;

    Danmu        *danmu;
    QRect         screenRect;
};

#endif // ROOM_H
