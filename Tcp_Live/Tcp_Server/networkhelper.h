#ifndef NETWORKHELPER_H
#define NETWORKHELPER_H
#include <QMutex>
#include <QVector>
#include <QMap>
#include "sockethandler.h"

typedef struct Account{
    int         flag;
    QString 	roomName;
    QTcpSocket *socket;
}Account_t;

class NetworkHelper
{
public:
    static NetworkHelper *getInstance();
    const QVector<SocketHandler*> &getClients() const;

    void insertClient(SocketHandler *sh){this->m_clients.push_back(sh);}

    bool isLogin(QString name);

    QMap<QString,Account_t> m_clientsMap;
    const QMap<QString,Account_t> &getClientsMap() const;
    void insertClientMap(QString name,Account_t sh){this->m_clientsMap.insert(name,sh);}

private:
    NetworkHelper();
    NetworkHelper(const NetworkHelper&) = delete;
    NetworkHelper &operator =(NetworkHelper&) = delete;

    static NetworkHelper *m_instance;
    static QMutex m_mutex;
    QVector<SocketHandler*> m_clients;
};

#endif // NETWORKHELPER_H
