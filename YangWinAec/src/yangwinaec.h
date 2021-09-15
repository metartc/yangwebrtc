#ifndef YANGWINAEC_H
#define YANGWINAEC_H
#include "yangwinaudiodevice.h"
#include <mmdeviceapi.h>
#include <dmo.h>
#include "YangWinAecI.h"
class YangWinAec:public YangWinAudioDevice
{
public:
    YangWinAec();
    virtual ~YangWinAec();

public:

    int captureAecThread();
    int startBuiltInAec();
    int initCapture();
    int initMicrophone();
    int initRecordingDMO();
    int setDMOProperties();
    int getRenderIndex();
    int initAec();
    int getBuffer(unsigned char* p,unsigned long* pbufLen);
private:
    IMediaObject* m_dmo;
    IMediaBuffer* m_mediaBuffer;
    DMO_OUTPUT_DATA_BUFFER OutputBufferStruct;
    DWORD m_dwStatus;
  //  IMMDevice* m_deviceIn;
   // IAudioClient* m_clientIn;
    //IAudioEndpointVolume* m_captureVolume;
   // IMMDeviceCollection* m_captureCollection;
    int m_micIndex;
    BYTE* m_dataBuf;
   IPropertyStore* m_ps;

   // int _outputDeviceIndex;


    YangRecData m_recData;
};

#endif // YANGWINAEC_H
