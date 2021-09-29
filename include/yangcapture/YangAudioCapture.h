#ifndef YANGAUDIOCAPTURE_H
#define YANGAUDIOCAPTURE_H

#include <yangavutil/audio/YangAecBase.h>
#include <yangavutil/audio/YangPreProcess.h>
//#include <yangavutil/audio/YangResample.h>
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/buffer/YangAudioBuffer.h"
#include <vector>
#include "yangutil/sys/YangThread.h"
#include "yangutil/sys/YangLog.h"
using namespace std;

class YangAudioCapture:public YangThread
{
    public:
        YangAudioCapture();
        virtual ~YangAudioCapture();
    public:

        int32_t aIndex;
        int32_t m_isStart;
        virtual int32_t init()=0;
        virtual void setCatureStart()=0;
        virtual void setCatureStop()=0;
        virtual void setOutAudioBuffer(YangAudioBuffer *pbuffer)=0;
        virtual void setPlayAudoBuffer(YangAudioBuffer *pbuffer)=0;
        virtual void setAec(YangAecBase *paec)=0;
        virtual void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuffer)=0;
        virtual void setPreProcess(YangPreProcess *pp)=0;

        void stop();
    protected:
        void run();
        YangAudioInfo *m_para;
        virtual void startLoop()=0;
        virtual  void stopLoop()=0;

};

#endif // YANGAUDIOCAPTURE_H
