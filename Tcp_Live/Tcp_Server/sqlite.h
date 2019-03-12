#ifndef SQLITE_H
#define SQLITE_H
#include <QMutex>
#include <QSqlDatabase>

class Sqlite                                //数据库
{
public:
    static Sqlite *getInstance();

    void createConnection();                //创建数据库文件链接(SQL_user.db)
    void removeConnection();                //移除数据库链接文件(SQL_user.db)

private:
    Sqlite();
    Sqlite(const Sqlite&) = delete;         //不生成默认复制构造函数
    Sqlite &operator = (const Sqlite&) = delete;

    static Sqlite   *m_instance;
    static QMutex    m_mutex;               //阻塞函数
    QSqlDatabase     m_db;                  //数据库链接
};

#endif // SQLITE_H
