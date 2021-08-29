#ifndef YANGMEETING_H
#define YANGMEETING_H
#include <QList>
#include "../yangwinutil/yangqInterfaces.h"
#include "yangUserInfo.h"
struct YangMeetingInfo{
    QString  meetingId=""; //教学编号
    QString  meetingName=""; //教学主题
    QString  director=""; //主持人ID
    QString  directorName; //主持人姓名
    QString  departId; //召集部门
    QString  status; //教学状态 0关闭1打开 2锁定
    QString  adminName="";

    QString statusDes; //教学状态
    QDate meetingDate; //教学时间
    QString  meetingDateDes; //教学时间
    QString  startTime; //开始时间
    QString  stopTime; //结束时间
    int   meetingTime=0; //教学时长
    QString   meetingType; //教学类型
    QString   meetingTypeName; //教学类型
    int   roomAdmin=0; //0：参与者 1：主将人 2：助理 3：主持人
    QString members;
    QString meetingDesc;
    QString mains; //主持人，主讲人
};

class YangMeetingInfoHandle:public YangHttpI
{
public:
    YangMeetingInfoHandle();
    ~YangMeetingInfoHandle();
public:
    QList<YangUserInfo*> *m_users;
    void success(QByteArray pa);
    void failure(QString pa);
    YangMeetingInfo m_meeting;
    int ret=0;
    //QList<YangMeetings*> *meetings;

};

#endif // YANGMEETINGS_H
