#ifndef YangWinAudioApiRender_H
#define YangWinAudioApiRender_H
#ifdef _WIN32
#include <audioclient.h>
#include <yangutil/yangavinfotype.h>
#include "YangWinAudioApi.h"
#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <yangavutil/audio/YangResample.h>
#include <yangavutil/audio/YangAudioMix.h>
#include <vector>
class YangWinAudioApiRender:public YangWinAudioApi
{
public:
    YangWinAudioApiRender(YangAudioInfo *pini);
virtual ~YangWinAudioApiRender();
public:
    int initRender();
    int playThread();
    int startRender();
    int stopRender();
    int render(uint8_t *pdata);
    int m_init;

    int32_t hasData();
    int32_t getData(uint8_t *pdata);
    void setInAudioBuffer(std::vector<YangAudioPlayBuffer*> *pal);
private:
    std::vector<YangAudioPlayBuffer*> *m_in_audioBuffer;
    int32_t m_audioPlayCacheNum;
    int32_t m_size;
    YangResample m_res;
    YangAudioMix m_mix;
    YangFrame m_audioFrame;
    uint8_t* m_resBuf;
    uint8_t* m_resTmp;
private:
    IMMDeviceCollection* m_renderCollection;
    IMMDevice* m_deviceOut;
    IAudioClient* m_clientOut;
    IAudioRenderClient* m_renderClient;
    ISimpleAudioVolume* m_renderSimpleVolume;

    //IAudioClock* m_clock;
    UINT32 m_bufferLength;

    int getSpeakerVolume(int& volume);
    int setSpeakerVolume(int volume);
    int getSpeakerMute(bool& enabled);
    int setSpeakerMute(bool enable);

   //  int playoutDeviceCount();


    int initSpeaker(int pind);
    int initPlay(int pind);


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
