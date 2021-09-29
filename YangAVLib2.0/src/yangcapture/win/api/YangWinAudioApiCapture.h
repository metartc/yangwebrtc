#ifndef YangWinAudioApiCapture_H1
#define YangWinAudioApiCapture_H1
#ifdef _WIN32
#include <audioclient.h>
#include <endpointvolume.h>

#include <yangutil/yangavtype.h>
#include <yangcapture/YangCaptureAudioData.h>
#include "YangAudioApiCapture.h"
#include "YangWinAudioApi.h"

class YangWinAudioApiCapture:public YangAudioApiCapture,public YangWinAudioApi
{
public:
    YangWinAudioApiCapture(YangAudioInfo* pini);
    virtual ~YangWinAudioApiCapture();
	void setCaptureCallback(YangCaptureCallback *cb);
	void captureThread();
    int initCapture();
    int startCpature();
    int stopCapture();
    int getAudioOutLength();
    int getMicrophoneVolume(int& volume);
    int setMicrophoneVolume(int volume);
    int setMicrophoneMute(bool enable);



    IMMDeviceCollection* m_captureCollection;
    IMMDevice* m_deviceIn;
    IAudioClient* m_clientIn;

    IAudioCaptureClient* m_captureClient;
    IAudioEndpointVolume* m_captureVolume;

    int m_inputDeviceIndex;
    int captureFrame(YangFrame* paudioFrame);

    void stop();
protected:
    virtual void startLoop();
      void stopLoop();
      void run();
      int m_loops;
private:
    int initMicrophone();
     UINT32 m_bufferLength;
     int m_blockSize;
     int m_frameSize;

     uint16_t m_channlList[3];
    YangCaptureAudioData m_audioData;
    HANDLE m_samplesReadyEvent;
    uint8_t* m_blank;


};
#endif
#endif // YangWinAudioApiCapture_H
