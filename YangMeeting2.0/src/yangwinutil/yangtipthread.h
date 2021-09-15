#ifndef YANGTIPTHREAD_H
#define YANGTIPTHREAD_H
#include <yangutil/sys/YangTimer.h>
#include "yangdesktip.h"
#include <yangmeeting/yangmeetingtype.h>
class YangTipThread:public YangTimerTask,public YangMeetingMessage
{

public:
    YangTipThread();
    virtual ~YangTipThread();
    void doTask(int taskId);
    void setShowMessage(QString str);
    void start();
    void stop();
    int32_t m_threadState;
    int32_t m_flashtime;
    YangDeskTip *m_tip;


    void startMessage();
     void stopMessage();
     int32_t getMessageState();
private:
    YangTimer *m_timer;
    int32_t m_waitTime;

};

#endif // YANGTIPTHREAD_H
