#include "sqlite.h"
#include <QDebug>
#include <QSqlError>        //lastError().text()

Sqlite *Sqlite::m_instance = 0;
QMutex  Sqlite::m_mutex;

Sqlite::Sqlite()
{
    //使用SQLite version 3数据库
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}


Sqlite *Sqlite::getInstance()
{
    if(m_instance == 0){
        m_mutex.lock();
        if(m_instance == 0){
            m_instance = new Sqlite();
        }
        m_mutex.unlock();
    }
    return m_instance;
}


void Sqlite::createConnection()
{
    m_db.setDatabaseName("SQL_user.db");
    if(!m_db.open()){
        qDebug() << m_db.lastError().text();
    }
}


void Sqlite::removeConnection()
{
    m_db.close();
    m_db.removeDatabase("SQL_user.db");     //移除数据库
}
