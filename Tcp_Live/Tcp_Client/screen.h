#ifndef SCREEN_H
#define SCREEN_H

#include <QWidget>
#include <QUdpSocket>
#include <QTimer>

namespace Ui {
class Screen;
}

class Screen : public QWidget
{
    Q_OBJECT

public:
    explicit Screen(QWidget *parent = 0);
    ~Screen();

public slots:
    void timeoutSlot();
    void tellThePort(int);

private:
    Ui::Screen *ui;
    QUdpSocket *m_writeSocket;
    QTimer *m_timer;
    int     m_myPort;
};

#endif // SCREEN_H
