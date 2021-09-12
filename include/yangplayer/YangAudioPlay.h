#ifndef YangAudioPlay_H
#define YangAudioPlay_H
//#include "yangutil/UtilPlay.h"
#include <yangavutil/audio/YangAecBase.h>
#include <yangavutil/audio/YangAudioMix.h>
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/sys/YangThread.h"
#define YangAudioBufferMaxNum 10

#include<algorithm>
#include <vector>

using namespace std;
#define SIZE_AUDIO_FRAME 4096



class YangAudioPlay:public YangThread
{
    public:
        YangAudioPlay();
        virtual ~YangAudioPlay();
    public:
        //AudioSampleCapture *m_acs;
        int32_t aIndex;
        virtual void init()=0;
        void setAudioList(vector<YangAudioPlayBuffer*> *paudioList);
        void setAecBase(YangAecBase* pace);
        int32_t m_aecInit=0;
        int32_t m_isStart;
        void stop();

    protected:
        virtual void startLoop()=0;
        virtual void stopLoop()=0;

        void run();
        vector<YangAudioPlayBuffer*> *m_in_audioBuffer;
        YangAudioInfo *m_ini;
        int32_t isInit;
        YangAecBase *m_ace;
        YangAudioMix mix;
        int	m_frames;
        int	m_channel;
        int	m_sample;

       void aecHandle(uint8_t *ppcm,int32_t plen);
    private:



    };

#endif // YANGAUDIOCAPTURE_H
