#ifndef YangLoginHandle_H
#define YangLoginHandle_H

#include "../yangwinutil/yangqInterfaces.h"
struct YangUserInfo{
    int32_t userId=0; //用户编号
    QString loginName="";
    int32_t adminId=1; //是否为系统管理员 0 非管理员 1 管理员
    //public var isAdmin		  	:Boolean   =false;	//是否为管理员
    QString userName=""; //用户姓名
    int32_t departId=0; //用户企业编码
    QString roomId=""; //登入教室编号
    int32_t roomAdmin=0; //教学管理员 0不是 1是
    QString email="";
    QString mobile="";
    QString tel="";
    int32_t adminType=0;
    int32_t isPubCam0=0; //是否发布视频信息 0不发布 1发布
    int32_t isPubMic=0; //是否举手发布音频信息 0不发布 1发布
};

class YangUserHandle:public YangHttpI
{
public:
    YangUserHandle();
    ~YangUserHandle();

public:
    void success(QByteArray pa);
    void failure(QString pa);
    int32_t loginState=0;
    YangUserInfo *m_userinfo;
public:

};

#endif // YANGHTTPBASE_H
