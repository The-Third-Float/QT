#include "userhandler.h"
#include "sqlite.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>        //lastError().text();
#include <QVariant>         //###################

UserHandler::UserHandler()
{

}


bool UserHandler::insertUser(const User &user)
{
    Sqlite *instance = Sqlite::getInstance();
    instance->createConnection();

    QSqlQuery query;
    query.prepare("insert into user(name,pswd) "
                  " values(:name, :pswd)");
    query.bindValue(":name",user.getName());
    query.bindValue(":pswd",user.getPswd());

    bool ret = query.exec();
    if(!ret){
        qDebug() << query.lastError().text();
    }

    instance->removeConnection();
    return ret;
}


bool UserHandler::checkUser(const User &user)
{
    Sqlite *instance = Sqlite::getInstance();
    instance->createConnection();

    QSqlQuery query;    //有动态参数时的执行方法
    query.prepare("select name, pswd from user "
                  " where name = :name and pswd = :pswd");
    query.bindValue(":name",user.getName());
    query.bindValue(":pswd",user.getPswd());

    bool ret = query.exec();    //11 9jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj exec  ->  执行程序
    if(!ret){
        qDebug() << query.lastError().text();
    }

    //检测结果集是否存在记录
    //执行完query.exec()这行代码时,query是指向结果集以外的，
    //我们可以利用query.next(),使得 query指向结果集的第一条记录。
    ret = query.next();

    instance->removeConnection();
    return ret;
}
