#ifndef YangVideoContext_H1
#define YangVideoContext_H1
#include <QMainWindow>
#include "yangmeeting/YangMeetingHandle.h"
#include "../video/yangvideothread.h"
#include "../login/yangUserInfo.h"

class YangVideoContext
{
public:
    YangVideoContext();
    virtual ~YangVideoContext();
    YangMeetingHandle *videoMeeting;
    YangVideoThread *videoThread;
    YangMeetingContext *context;
    YangUserInfo *userinfo;
    YangMeeting *meeting;
    QMainWindow *mainWin;
};

#endif // YANGCONTEXT_H
