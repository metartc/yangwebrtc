#ifndef YangWinAudioApiRender_H
#define YangWinAudioApiRender_H
#ifdef _WIN32
#include <audioclient.h>
#include <yangutil/yangavinfotype.h>
#include "YangWinAudioApi.h"
#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <yangavutil/audio/YangResample.h>
#include <yangavutil/audio/YangAudioMix.h>
#include <yangplayer/YangAudioPlay.h>
#include <vector>
class YangWinAudioApiRender:public YangWinAudioApi,public YangThread
{
public:
    YangWinAudioApiRender(YangAudioInfo *pini);
virtual ~YangWinAudioApiRender();
public:
    int initRender();
    int playThread();
    int startRender();
    int stopRender();
    int render_16k(uint8_t *pdata,int nb);
    int render_48k(uint8_t *pdata,int nb);
    int render_44k(uint8_t *pdata,int nb);

    int m_init;
    int m_isStart;
    int32_t hasData();
    int32_t getData(uint8_t *pdata);

    void setAudioList(YangAudioPlayBuffer *pal);
    int init();

    int32_t m_loops;
    void stop();
private:
    YangAudioPlayBuffer* m_in_audioBuffer;
    int32_t m_audioPlayCacheNum;
    int32_t m_size;
    YangResample m_res;
    YangAudioMix m_mix;
    YangFrame m_audioFrame;
    uint8_t* m_resBuf;
    uint8_t* m_resTmp;

    uint8_t* m_buffer;
    uint8_t* m_44ktmp;
    int m_bufLen;
    int m_is16k;
protected:
    void startLoop();
    void stopLoop();
    void run();
private:
    HANDLE _hRenderSamplesReadyEvent;
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
