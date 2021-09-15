#ifndef YANGWINAUDIODEVICE_H
#define YANGWINAUDIODEVICE_H
#include "windows.h"
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>
#include <stdint.h>
struct YangPlayData{
    int framesize;
    int frames;
    int channel;
    int blockSize;
    unsigned int sample;
};

struct YangRecData{
    int framesize;
    int frames;
    int channel;
    int blockSize;
    unsigned int sample;
};
class YangWinAudioDevice
{
public:
    YangWinAudioDevice();
virtual ~YangWinAudioDevice();
protected:
    HRESULT enumEndpointDevicesAll(IMMDeviceEnumerator* penum,EDataFlow dataFlow);
    int getDefaultDeviceID( IMMDeviceEnumerator* penum,EDataFlow dir,ERole role,LPWSTR szBuffer,int bufferLen);
    int getDeviceID(IMMDevice* pDevice, LPWSTR pszBuffer,int bufferLen);
    int getDefaultDeviceIndex(IMMDeviceEnumerator* penum,EDataFlow dir,  ERole role, int* index);
    int setBoolProperty(IPropertyStore* ptrPS, REFPROPERTYKEY key,VARIANT_BOOL value);
    int setVtI4Property(IPropertyStore* ptrPS,REFPROPERTYKEY key,LONG value);
    int getDeviceListCount(IMMDeviceCollection* pcollection,EDataFlow dir);
    int getListDevice(IMMDeviceEnumerator* penum,EDataFlow dir,int index,IMMDevice** ppDevice);

protected:
      IMMDeviceEnumerator* m_enum;
};

#endif // YANGWINAUDIODEVICE_H
