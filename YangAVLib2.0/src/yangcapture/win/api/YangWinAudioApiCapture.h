#ifndef YangWinAudioApiCapture_H
#define YangWinAudioApiCapture_H
#ifdef _WIN32
#include <audioclient.h>
#include <endpointvolume.h>
#include "YangWinAudioApi.h"

class YangWinAudioApiCapture:public YangWinAudioApi
{
public:
    YangWinAudioApiCapture();
virtual ~YangWinAudioApiCapture();
    int initCapture();
    int initMicrophone();
    int getMicrophoneVolume(int& volume);
    int setMicrophoneVolume(int volume);
    int setMicrophoneMute(bool enable);

    void captureThread();

    IMMDeviceCollection* m_captureCollection;
    IMMDevice* m_deviceIn;
    IAudioClient* m_clientIn;

    IAudioCaptureClient* m_captureClient;
    IAudioEndpointVolume* m_captureVolume;

    int m_inputDeviceIndex;
    YangRecData m_recData;

    int captureFrame(uint8_t* p);
  //  double _perfCounterFactor;
   // LARGE_INTEGER _perfCounterFreq;

private:
     UINT32 m_bufferLength;

     BYTE* pData = 0;
     UINT32 framesAvailable = 0;
     DWORD flags = 0;
     UINT64 recTime = 0;
     UINT64 recPos = 0;
     UINT32 packetLength=0;
};
#endif
#endif // YangWinAudioApiCapture_H
