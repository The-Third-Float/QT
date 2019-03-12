#include "room.h"
#include "ui_room.h"
#include <QDebug>
#include <QBuffer>
#include <QImageReader>
#include <QCameraInfo>
#include <QAudioFormat>
#include <QHostAddress>
#include "package.h"
#include <QMessageBox>
#include <QDesktopWidget>
#include <QTime>

Room::Room(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Room)
{
    ui->setupUi(this);
    //设置背景
    setAutoFillBackground(true);
    QPalette pal = this->palette();
    pal.setBrush(QPalette::Background, QBrush(QPixmap("D:/QT_DOCUMENTS/Project_2018_11_07/Tcp_KeHuDuan/icon/image_03.jpg")));
    setPalette(pal);
    //接收数据
    m_readSocket = new QUdpSocket(this);
    connect(this->m_readSocket, SIGNAL(readyRead()),
            this, SLOT(readyReadSlot()));

    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->pushButton_4->setEnabled(false);
    ui->pushButton_7->setEnabled(false);
    ui->pushButton_8->setEnabled(false);

    //摄像头
    QCameraInfo info = QCameraInfo::defaultCamera();
    m_camera = new QCamera(info, this);

    m_writeSocket = new QUdpSocket(this);

    m_vs = new VideoSuface(this);
    m_camera->setViewfinder(m_vs);

    connect(m_vs, SIGNAL(frameChanged(QVideoFrame)),
            this, SLOT(frameChangedSlot(QVideoFrame)));

    //麦克风
    m_socketAudioRead = new QUdpSocket(this);
    m_socketAudioRead->bind(QHostAddress::AnyIPv4,m_myPort,
                            QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    connect(m_socketAudioRead,SIGNAL(readyRead()),
            this,SLOT(onReadyReadAudio()));

    QAudioFormat format;
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audip/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);
    m_audioInput = new QAudioInput(format);
    m_audioOutput = new QAudioOutput(format);
    m_audioOutputIODevice = m_audioOutput->start();

    //弹幕
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        QDesktopWidget* desktopWidget;                             //获取桌面设备
        desktopWidget = QApplication::desktop();                   //获取桌面设备
        screenRect = desktopWidget->screenGeometry();
//    screenRect = ui->label->geometry();
}

Room::~Room()
{
    delete ui;
}

void Room::cleanTheRoom()
{
    ui->label_2->hide();
    ui->label_3->hide();
    ui->label_6->hide();
    ui->pushButton->hide();
    ui->pushButton_2->hide();
    ui->pushButton_3->hide();
    ui->pushButton_4->hide();
    ui->pushButton_7->hide();
    ui->pushButton_8->hide();
}

void Room::readyReadSlot()
{
    qDebug() << "............";
    quint64 size = m_readSocket->pendingDatagramSize();
    //直接读取异步监听时接收到的数据报
    QByteArray buff;
    // 字节矩阵，大小用qint64描述，适合操作1大量数据2字符串数组3二进制01数据，节省内存，更加安全
    //一般用const char*初始化
    // 默认采用深拷贝，可指定为浅拷贝
    buff.resize(size);
    //为缓冲区规定一个maxsize（没必要用那么大，刚好装下图像就够了）
    m_readSocket->readDatagram(buff.data(),buff.size());
    // 数据报只支持一维操作
    QBuffer buffer(&buff);

    //提供用Qbytearray读写内存中IO缓存区的接口，若无传参内部默认自动创建一个Qbytearray；

    //对IO缓存区读写操作等效于像IO设备读写数据

    //操作QBuffer像在操作文件（其实原理都差不多，各自都是内存中一块特殊区域嘛）
    QImageReader reader(&buffer,"JPEG");
    //可读入磁盘文件、设备文件中的图像、以及其他图像数据如pixmap和image，相比较更加专业。
    //buffer属于设备文件一类，
    //专业地读取设备文件的图像数据。
    QImage image = reader.read();
    //read()方法用来读取设备图像，也可读取视频，读取成功返回QImage*，否则返回NULL
    //格式转换
    QPixmap pix = QPixmap::fromImage(image);
    ui->label->setPixmap(pix.scaled(ui->label->size()));
}


//开启录屏模式
void Room::on_pushButton_clicked()
{
    m_camera->stop();
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_7->setEnabled(true);
    ui->pushButton->setEnabled(false);
    //录屏
    m_screen = new Screen();
    m_screen->hide();

    connect(this,SIGNAL(toTellThePort(int)),
            m_screen,SLOT(tellThePort(int)));
    emit toTellThePort(m_myPort);
    //接受录屏信号
    if(this->m_readSocket->state() == QAbstractSocket::BoundState){
        m_readSocket->leaveMulticastGroup(QHostAddress("224.0.0.220"));
        m_readSocket->close();
    }
    m_readSocket->bind(QHostAddress::AnyIPv4, m_myPort,
                     QUdpSocket::ShareAddress
                     | QUdpSocket::ReuseAddressHint);

    m_readSocket->joinMulticastGroup(QHostAddress("224.0.0.220"));
}

//开启麦克风
void Room::on_pushButton_3_clicked()
{
    if(ui->pushButton_3->isChecked()){
        m_audioInputIODevice = m_audioInput->start();
        connect(m_audioInputIODevice,SIGNAL(readyRead()),
                this,SLOT(captureDataFromDevice()));
    }else{
        m_audioInput->stop();
    }

    if(this->m_readSocket->state() == QAbstractSocket::BoundState){
        m_readSocket->leaveMulticastGroup(QHostAddress("224.0.0.220"));
        m_readSocket->close();
    }
    m_readSocket->bind(QHostAddress::AnyIPv4, m_myPort,
                     QUdpSocket::ShareAddress
                     | QUdpSocket::ReuseAddressHint);

    m_readSocket->joinMulticastGroup(QHostAddress("224.0.0.220"));

    ui->pushButton_3->setEnabled(false);
    ui->pushButton_4->setEnabled(true);
}

//关闭录屏模式
void Room::on_pushButton_2_clicked()
{
    m_camera->stop();
    m_readSocket->leaveMulticastGroup(QHostAddress("224.0.0.220"));
    if(m_screen != NULL){
        delete m_screen;
        m_screen = NULL;
    }
    ui->label->clear();
//    ui->label->setStyleSheet("QLabel{border-image: url(D:/QT_DOCUMENTS/Project_2018_11_07/Tcp_KeHuDuan/icon/image_04.jpg);}");

    QPixmap mPix;
    mPix.load("D:/QT_DOCUMENTS/Project_2018_11_02/Tcp_KeHuDuan/icon/image_04.jpg");
    ui->label->setPixmap(mPix);
    QByteArray byte;
    QBuffer buffer(&byte);           //缓存
    QImage img;
    img.load("D:/QT_DOCUMENTS/Project_2018_11_02/Tcp_KeHuDuan/icon/image_04.jpg");
    img.save(&buffer, "JPEG");
    m_writeSocket->writeDatagram(byte,
          QHostAddress("224.0.0.220"), m_myPort);

    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->pushButton_4->setEnabled(false);
    ui->pushButton_7->setEnabled(false);
    ui->pushButton_8->setEnabled(false);
}

//开启摄像头
void Room::on_pushButton_7_clicked()
{
    m_readSocket->leaveMulticastGroup(QHostAddress("224.0.0.220"));
    ui->label->clear();
    if(m_screen != NULL){
        delete m_screen;
        m_screen = NULL;
    }
    m_camera->start();
    ui->pushButton_7->setEnabled(false);
    ui->pushButton_8->setEnabled(true);
}

//预置摄像头
void Room::frameChangedSlot(QVideoFrame frame)
{
    frame.map(QAbstractVideoBuffer::ReadOnly);

    QImage img(
        frame.bits(),
        frame.width(),
        frame.height(),
        frame.bytesPerLine(),
        QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat())
    );

    //旋转图片180
    QMatrix matrix;
    matrix.rotate(180);
    img = img.transformed(matrix);

    //QImage--->QPixmap
    QPixmap pixMap = QPixmap::fromImage(img);
    //让图片适应label的大小
    pixMap = pixMap.scaled(ui->label->size());
    //将图片显示到label上
    ui->label->setPixmap(pixMap);

    qDebug() << img.size();
    //将图片信息以广播组的形式发送出去
    QByteArray byte;
    QBuffer buffer(&byte);           //缓存
    img.save(&buffer, "JPEG");

    qDebug() << byte.size();
    //将图片信息写入到制定端口
    m_writeSocket->writeDatagram(byte,
          QHostAddress("224.0.0.220"), m_myPort);
    qDebug() << "write....";

}

//关闭摄像头
void Room::on_pushButton_8_clicked()
{
    m_camera->stop();
    ui->label->clear();
    if(m_screen == NULL){
        m_screen = new Screen();
        m_screen->hide();

        connect(this,SIGNAL(toTellThePort(int)),
                m_screen,SLOT(tellThePort(int)));
        emit toTellThePort(m_myPort);
    }

    m_readSocket->joinMulticastGroup(QHostAddress("224.0.0.220"));

    ui->pushButton_7->setEnabled(true);
    ui->pushButton_8->setEnabled(false);
}

//麦克风槽函数
void Room::captureDataFromDevice()
{
    AudioPackage pack;
    memset(&pack,0,sizeof(AudioPackage));
    pack.dataLen = m_audioInputIODevice->read(pack.data,1024);
    m_socketAudioWrite->writeDatagram((char*)&pack,
                                      sizeof(AudioPackage),
                                      QHostAddress::Broadcast,
//                                      groupAddress,
                                      m_myPort);
    m_audioOutputIODevice->write(pack.data,pack.dataLen);
}

//麦克风网络传输槽函数
void Room::onReadyReadAudio()
{
    AudioPackage pack;
    memset(&pack,0,sizeof(AudioPackage));
    m_socketAudioRead->readDatagram((char*)&pack,sizeof(AudioPackage));
    if(m_audioOutputIODevice){
        m_audioOutputIODevice->write(pack.data,pack.dataLen);
    }
}

//端口号可以写在pswd里面
void Room::enterrRoom(pack_t pack)
{
    cleanTheRoom();
    this->show();
    QString caMsg = QString::fromLocal8Bit(pack.data);
    QStringList sl = caMsg.split('/');
    QStringList::iterator iter = sl.begin();
    for(;iter != sl.end();++iter){
        if(*iter != ""){
            ui->listWidget->addItem(*iter);
        }
    }
    QString myPortS = pack.pswd;
    m_hostPort = myPortS.toInt();
        //接受录屏信号
        if(this->m_readSocket->state() == QAbstractSocket::BoundState){
            m_readSocket->leaveMulticastGroup(QHostAddress("224.0.0.220"));
            m_readSocket->close();
        }
        m_readSocket->bind(QHostAddress::AnyIPv4, m_hostPort,
                         QUdpSocket::ShareAddress
                         | QUdpSocket::ReuseAddressHint);
    m_readSocket->joinMulticastGroup(QHostAddress("224.0.0.220"));
}

void Room::updateList(pack_t pack)
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
}

void Room::reciveChatFromIndex(pack_t pack)
{
    QString caMsg = QString::fromLocal8Bit(pack.data);
    QStringList sl = caMsg.split('/');
    QStringList::iterator iter = sl.begin();
    for(;iter != sl.end();++iter){
        ui->textEdit->append(*iter);
        danmu = new Danmu(NULL,*iter,"Yellow",1,screenRect);
//        danmu = new Danmu(NULL,*iter,"",1,screenRect);
    }
}

void Room::closeEvent(QCloseEvent *)
{
    m_readSocket->leaveMulticastGroup(QHostAddress("224.0.0.220"));
    emit toKillUserInRoom();
}

void Room::tellThePort(int myPort)
{
    m_myPort = myPort;
    qDebug() << "m_myPort:" << m_myPort;
    emit toTellThePort(m_myPort);
}

void Room::on_pushButton_6_clicked()
{
    emit toUpdateList();
}

//发送消息
void Room::on_pushButton_5_clicked()
{
    QString caMsg = ui->lineEdit->text();
    ui->lineEdit->clear();
    emit sendMsgToIndex(caMsg);
}

void Room::on_pushButton_4_clicked()
{
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(false);
}
