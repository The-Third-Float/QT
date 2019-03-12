#ifndef USER_H
#define USER_H
#include <QString>

class User
{
public:
    User(){}
    User(const QString &name,const QString &pswd)
        :m_name(name),m_pswd(pswd){}
    User(int id,const QString &name,const QString &pswd)
        :m_id(id),m_name(name),m_pswd(pswd){}
    ~User(){}

    int  getId()const{return this->m_id;}
    void setId(int id){this->m_id = id;}

    const QString &getName()const{return this->m_name;}
    void setName(const QString &name){this->m_name = name;}

    const QString &getPswd()const{return this->m_pswd;}
    void setPswd(const QString &pswd){this->m_pswd = pswd;}

private:
    int     m_id;
    QString m_name;
    QString m_pswd;
};

#endif // USER_H
