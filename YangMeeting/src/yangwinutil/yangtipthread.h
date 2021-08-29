#ifndef YANGTIPTHREAD_H
#define YANGTIPTHREAD_H
#include <QThread>
#include "yangdesktip.h"
class YangTipThread:public QThread
{

public:
    YangTipThread();
    virtual ~YangTipThread();
    void run();
    void setShowMessage(QString str);
    int m_threadState;
    YangDeskTip *m_tip;

    int m_loop;
};

#endif // YANGTIPTHREAD_H
