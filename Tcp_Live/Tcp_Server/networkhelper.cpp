#include "networkhelper.h"

NetworkHelper *NetworkHelper::m_instance = 0;
QMutex NetworkHelper::m_mutex;

NetworkHelper *NetworkHelper::getInstance()
{
    if(m_instance == 0){
        m_mutex.lock();
        if(m_instance == 0){
            m_instance = new NetworkHelper();
        }
        m_mutex.unlock();
    }
    return m_instance;
}


NetworkHelper::NetworkHelper()
{

}


const QVector<SocketHandler*> &NetworkHelper::getClients() const
{
    return this->m_clients;
}

const QMap<QString,Account_t> &NetworkHelper::getClientsMap() const
{
    return this->m_clientsMap;
}


bool NetworkHelper::isLogin(QString name)
{
    bool flag = false;
    if(m_clientsMap.contains(name))
    {
        flag = true;
    }
    return flag;
}
