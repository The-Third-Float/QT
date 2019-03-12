#ifndef USERHANDLER_H
#define USERHANDLER_H
#include "user.h"

class UserHandler
{
public:
    UserHandler();

    bool insertUser(const User &user);      //添加记录
    bool checkUser(const User &user);       //检查用户名密码
};

#endif // USERHANDLER_H
