#include "screen.h"
#include "ui_screen.h"
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <QBuffer>
#include <QDebug>

Screen::Screen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Screen)
{
    ui->setupUi(this);
    m_timer = new QTimer(this);
    m_writeSocket = new QUdpSocket(this);
    m_timer->start(100);

    connect(m_timer,SIGNAL(timeout()),
            this,SLOT(timeoutSlot()));
}


Screen::~Screen()
{
    delete ui;
}


void Screen::timeoutSlot()
{
    QPixmap pix = QPixmap::grabWindow(
                QApplication::desktop()->winId(),
                0,0,-1,-1);
    ui->label->setPixmap(pix.scaled(ui->label->size()));

    QImage img = pix.toImage();
    if(img.isNull()){
        qDebug() << "Translate failed...";
    }

    img = img.scaled(QSize(400,300));
    qDebug() << img.size();
    QByteArray byte;
    QBuffer buffer(&byte);
    img.save(&buffer,"JPEG");

    qDebug() << byte.size();

    m_writeSocket->writeDatagram(byte,QHostAddress("224.0.0.220"), m_myPort);

    qDebug() << "write...";
}

void Screen::tellThePort(int myPort)
{
    m_myPort = myPort;
    qDebug() << "m_myPort in SCREEN:" << m_myPort;
}
