#ifndef YangWinAudioApiRender_H
#define YangWinAudioApiRender_H
#ifdef _WIN32
#include <audioclient.h>
#include <yangutil/yangavinfotype.h>

#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <yangavutil/audio/YangResample.h>
#include <yangavutil/audio/YangAudioMix.h>
#include <yangplayer/YangAudioPlay.h>
#include <vector>
#include "YangWinAudioApi.h"
#include "YangRenderAudioData.h"
class YangWinAudioApiRender:public YangWinAudioApi,public YangThread
{
public:
    YangWinAudioApiRender(YangAudioInfo *pini);
virtual ~YangWinAudioApiRender();
public:
    int initRender();
    void setAec(YangAecBase *paec);
    void setAudioList(YangAudioPlayBuffer *pal);
    int playThread();
    int startRender();
    int stopRender();
    //int render_16k(uint8_t *pdata,int nb);
    int render_10ms();
    int render_aec_10ms();
   // int render_44k(uint8_t *pdata,int nb);

    int m_init;
    int m_isStart;
    YangRenderAudioData m_audioData;

    int init();
    int m_hasRenderEcho;
    int32_t m_loops;
    void stop();
protected:

    YangFrame m_audioFrame;
private:
   // int render_48k(uint8_t *pdata,int nb);
    int32_t m_audioPlayCacheNum;
    int32_t m_sample;
    int32_t m_size;
    YangResample m_res;

    uint8_t* m_resBuf;
    uint8_t* m_resTmp;

   // uint8_t* m_buffer;
  //  uint8_t* m_44ktmp;
    int m_bufLen;
   // int m_is16k;
    uint32_t m_blockSize;


protected:
    virtual void startLoop();
    void stopLoop();
    void run();

    YangAecBase *m_aec;
    HANDLE m_samplesReadyEvent;
private:

    IMMDeviceCollection* m_renderCollection;
    IMMDevice* m_deviceOut;
    IAudioClient* m_clientOut;
    IAudioRenderClient* m_renderClient;
    ISimpleAudioVolume* m_renderSimpleVolume;


    UINT32 m_bufferLength;

    int getSpeakerVolume(int& volume);
    int setSpeakerVolume(int volume);
    int getSpeakerMute(bool& enabled);
    int setSpeakerMute(bool enable);

    int initSpeaker(int pind);
    int initPlay(int pind);
    int setFormat(WAVEFORMATEX *pwfx);


   int  m_outputDeviceIndex;

   YangPlayData m_playData;

   BYTE* m_dataBufP ;
   DWORD flags ;
   UINT32 padding;
   uint32_t framesAvailable;
   bool keepPlaying;

};
#endif
#endif // YangWinAudioApiRender_H
