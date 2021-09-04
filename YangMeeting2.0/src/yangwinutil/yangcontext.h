#ifndef YangVideoContext_H
#define YangVideoContext_H
#include "yangmeeting/YangMeetingHandle.h"
#include "../video/yangvideothread.h"
#include "../../mainwindow.h"
class YangVideoContext
{
public:
    YangVideoContext();
    //static YangMeetingHandle *videoMeeting;//=NULL;
    static MainWindow *mainWin;
    static YangVideoThread *yangThread;
};

#endif // YANGCONTEXT_H
