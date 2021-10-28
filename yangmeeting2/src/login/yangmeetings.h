#ifndef YANGMEETINGS_H
#define YANGMEETINGS_H
#include <QList>
#include "../yangwinutil/yangqInterfaces.h"
#include "yangmeeting.h"
class YangMeetingsHandle:public YangHttpI
{
public:
    YangMeetingsHandle();
    ~YangMeetingsHandle();
public:
    void success(QByteArray pa);
    void failure(QString pa);
    int32_t ret=0;
    QList<YangMeetingInfo*> *meetings;

};

#endif // YANGMEETINGS_H
