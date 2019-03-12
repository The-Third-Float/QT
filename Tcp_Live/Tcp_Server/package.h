#ifndef PACKAGE_H
#define PACKAGE_H

enum dataType{
    //注册信息
    TYPE_REG,
    TYPE_REG_FAILURE,
    TYPE_REG_SUCCESS,
    //登录信息
    TYPE_LOG,
    TYPE_LOG_FAILURE,
    TYPE_LOG_SUCCESS,
    //创建房间
    TYPE_ROM,
    TYPE_ROM_FAILURE,
    TYPE_ROM_SUCCESS,
    //刷新房间
    TYPE_UPR,
    TYPE_UPR_SUCCESS,
    //进入房间
    TYPE_ETR,
    TPYE_ETR_FAILURE,
    TYPE_ETR_SUCCESS,
    //刷新成员
    TYPE_UPL,
    TYPE_UPL_SUCCESS,
    //用户聊天
    TYPE_CAT,
    TPYE_CAT_FAILURE,
    TYPE_CAT_SUCCESS,
    //用户退出
    TYPE_DIE,
    TYPE_QIT
};


typedef struct pack{
    dataType    type;           //数据类型
    char        name[20];       //用户名
    char        pswd[20];       //密码
    char        data[1024];     //各种数据
}pack_t;

#endif // PACKAGE_H
