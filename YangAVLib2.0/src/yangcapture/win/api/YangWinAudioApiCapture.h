#ifndef YangWinAudioApiCapture_H1
#define YangWinAudioApiCapture_H1
#ifdef _WIN32
#include <audioclient.h>
#include <endpointvolume.h>

#include <yangutil/yangavtype.h>
#include "YangCaptureAudioData.h"
#include "YangAudioApiCapture.h"
#include "YangWinAudioApi.h"

class YangWinAudioApiCapture:public YangAudioApiCapture,public YangWinAudioApi
{
public:
    YangWinAudioApiCapture(YangAudioInfo* pini);
    virtual ~YangWinAudioApiCapture();
	void setCaptureCallback(YangCaptureCallback *cb);
    int initCapture();
    int startCpature();
    int stopCapture();
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
    int captureFrame(YangFrame* paudioFrame);
    int m_isStart;
    void stop();
protected:
    virtual void startLoop();
      void stopLoop();
      void run();
      int m_loops;
private:
    int initMicrophone();
    //YangFrame m_audioFrame;
     UINT32 m_bufferLength;
     int m_blockSize;


    YangCaptureAudioData m_audioData;
    HANDLE m_samplesReadyEvent;


};
#endif
#endif // YangWinAudioApiCapture_H
