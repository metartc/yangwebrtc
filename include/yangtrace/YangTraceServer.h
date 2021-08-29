
#ifndef TRACE_YangTraceServer_H_
#define TRACE_YangTraceServer_H_

#include <iostream>
#include "YangCameraControl.h"
#include "YangTraceHandle.h"
#include "yangutil/sys/YangThread.h"
#include "YangTraceIni.h"
class YangTraceServer:public YangThread
{
public:
    YangTraceServer(YangTraceIni* pini);
    virtual ~YangTraceServer(void);

    void init();
    void startLoop();

    int m_isStart;
    int m_isConvert;
    void stop();
    void run();
private:
    YangTraceHandle *m_trace;
    YangCameraControl m_camCtl;
    YangTraceIni* m_ini;

};

#endif
