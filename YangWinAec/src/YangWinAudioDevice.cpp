#include "yangwinaudiodevice.h"
//#include <QDebug>
#include <assert.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>
#include <dmo.h>

#include <audiopolicy.h>
#include <wmcodecdsp.h>
#include <windows.h>
#include <uuids.h>
#include <iostream>
#include <Functiondiscoverykeys_devpkey.h>
#include <propvarutil.h>
#include <functiondiscoverykeys.h>
#define SAFE_RELEASE(x) if(x){x->Release();x=NULL;}
#define CONTINUE_ON_ERROR(x) if(x!=S_OK){continue;}
#define Yang_Release(x) if(x){x->Release();x=NULL;}
#define EXIT_ON_ERROR(hres) \
    do {                      \
    if (FAILED(hres))       \
    goto Exit;            \
    } while (0)

YangWinAudioDevice::YangWinAudioDevice()
{
    m_enum=NULL;
    CoInitialize(NULL); //初始化COM库
    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
                     __uuidof(IMMDeviceEnumerator),
                     reinterpret_cast<void**>(&m_enum));

}
YangWinAudioDevice::~YangWinAudioDevice()
{
     Yang_Release(m_enum);

}

HRESULT YangWinAudioDevice::enumEndpointDevicesAll(IMMDeviceEnumerator* penum,EDataFlow dataFlow)  {
    HRESULT hr = S_OK;
    IMMDeviceCollection* pCollection = NULL;
    IMMDevice* pEndpoint = NULL;
    IPropertyStore* pProps = NULL;
    IAudioEndpointVolume* pEndpointVolume = NULL;
    LPWSTR pwszID = NULL;
    hr = penum->EnumAudioEndpoints(
                dataFlow,  // data-flow direction (input parameter)
                DEVICE_STATE_ACTIVE | DEVICE_STATE_DISABLED | DEVICE_STATE_UNPLUGGED,&pCollection);  // release interface when done

    EXIT_ON_ERROR(hr);
    UINT count = 0;
    hr = pCollection->GetCount(&count);
    EXIT_ON_ERROR(hr);

    if (count == 0) {
        return 0;
    }
    for (ULONG i = 0; i < count; i++) {
         hr = pCollection->Item(i, &pEndpoint);
        hr = pEndpoint->GetId(&pwszID);
        CONTINUE_ON_ERROR(hr);

        hr = pEndpoint->OpenPropertyStore(STGM_READ, &pProps);
        PROPVARIANT varName;
        PropVariantInit(&varName);
        hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
        CONTINUE_ON_ERROR(hr);
        DWORD dwState;
        hr = pEndpoint->GetState(&dwState);
        CONTINUE_ON_ERROR(hr);
        DWORD dwHwSupportMask = 0;
        hr = pEndpoint->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL,
                                 (void**)&pEndpointVolume);
        CONTINUE_ON_ERROR(hr);
        hr = pEndpointVolume->QueryHardwareSupport(&dwHwSupportMask);
        CONTINUE_ON_ERROR(hr);

  //  if (dwHwSupportMask & ENDPOINT_HARDWARE_SUPPORT_MUTE)
    //if (dwHwSupportMask & ENDPOINT_HARDWARE_SUPPORT_METER)
        UINT nChannelCount(0);
        hr = pEndpointVolume->GetChannelCount(&nChannelCount);
        CONTINUE_ON_ERROR(hr);
          if (dwHwSupportMask & ENDPOINT_HARDWARE_SUPPORT_VOLUME) {
            float fLevelMinDB(0.0);
            float fLevelMaxDB(0.0);
            float fVolumeIncrementDB(0.0);
            hr = pEndpointVolume->GetVolumeRange(&fLevelMinDB, &fLevelMaxDB,&fVolumeIncrementDB);
            CONTINUE_ON_ERROR(hr);
            int n = (int)((fLevelMaxDB - fLevelMinDB) / fVolumeIncrementDB);
            UINT nStep(0);
            UINT nStepCount(0);
            hr = pEndpointVolume->GetVolumeStepInfo(&nStep, &nStepCount);
            CONTINUE_ON_ERROR(hr);
        }
Next:
        if (FAILED(hr)) {
              }
        CoTaskMemFree(pwszID);
        pwszID = NULL;
        PropVariantClear(&varName);
        SAFE_RELEASE(pProps);
        SAFE_RELEASE(pEndpoint);
        SAFE_RELEASE(pEndpointVolume);
    }
    SAFE_RELEASE(pCollection);
    return 0;

Exit:
    //_TraceCOMError(hr);
    CoTaskMemFree(pwszID);
    pwszID = NULL;
    SAFE_RELEASE(pCollection);
    SAFE_RELEASE(pEndpoint);
    SAFE_RELEASE(pEndpointVolume);
    SAFE_RELEASE(pProps);
    return 1;
}

int YangWinAudioDevice::getListDevice(IMMDeviceEnumerator* penum,EDataFlow dir,int index,IMMDevice** ppDevice) {
    HRESULT hr(S_OK);
    IMMDeviceCollection* pCollection = NULL;

    hr = penum->EnumAudioEndpoints(
                dir,
                DEVICE_STATE_ACTIVE,  // only active endpoints are OK
                &pCollection);
    if (FAILED(hr)) {

        SAFE_RELEASE(pCollection);
        return 1;
    }

    hr = pCollection->Item(index, ppDevice);
    if (FAILED(hr)) {

        SAFE_RELEASE(pCollection);
        return 1;
    }

    return 0;
}
int YangWinAudioDevice::getDeviceListCount(IMMDeviceCollection* pcollection,EDataFlow dir){
    HRESULT hr = S_OK;
    UINT count = 0;

        hr = pcollection->GetCount(&count);


    return static_cast<int>(count);
}
int YangWinAudioDevice::getDeviceID(IMMDevice* pDevice, LPWSTR pszBuffer,int bufferLen) {


  static const WCHAR szDefault[] = L"<Device not available>";

  HRESULT hr = E_FAIL;
  LPWSTR pwszID = NULL;

  assert(pszBuffer != NULL);
  assert(bufferLen > 0);

  if (pDevice != NULL) {
    hr = pDevice->GetId(&pwszID);
  }

  if (hr == S_OK) {
    // Found the device ID.
    wcsncpy_s(pszBuffer, bufferLen, pwszID, _TRUNCATE);
  } else {
    // Failed to find the device ID.
    wcsncpy_s(pszBuffer, bufferLen, szDefault, _TRUNCATE);
  }

  CoTaskMemFree(pwszID);
  return 0;
}
int YangWinAudioDevice::getDefaultDeviceID( IMMDeviceEnumerator* _ptrEnumerator,EDataFlow dir,ERole role,LPWSTR szBuffer,int bufferLen) {


  HRESULT hr = S_OK;
  IMMDevice* pDevice = NULL;

  hr = _ptrEnumerator->GetDefaultAudioEndpoint(dir, role, &pDevice);

  if (FAILED(hr)) {

    SAFE_RELEASE(pDevice);
    return 1;
  }

  int32_t res = getDeviceID(pDevice, szBuffer, bufferLen);
  SAFE_RELEASE(pDevice);
  return res;
}

int YangWinAudioDevice::getDefaultDeviceIndex(IMMDeviceEnumerator* penum,EDataFlow dir, ERole role,int* index) {
     HRESULT hr = S_OK;


  WCHAR szDefaultDeviceID[MAX_PATH] = {0};
  WCHAR szDeviceID[MAX_PATH] = {0};

  const size_t kDeviceIDLength = sizeof(szDeviceID) / sizeof(szDeviceID[0]);
  assert(kDeviceIDLength ==
         sizeof(szDefaultDeviceID) / sizeof(szDefaultDeviceID[0]));

  if (getDefaultDeviceID(penum,dir, role, szDefaultDeviceID, kDeviceIDLength)) {
       printf( "getDefaultDeviceI r failed");
    return 1;
  }

  IMMDeviceCollection* collection;
  hr = penum->EnumAudioEndpoints(dir, DEVICE_STATE_ACTIVE,
                                            &collection);

  if (!collection) {
    printf( "Device collection not valid");
    return 1;
  }

  UINT count = 0;
  hr = collection->GetCount(&count);
  if (FAILED(hr)) {
     printf("..........collection GetCount failed====================");
    return 1;
  }
  printf("..........collection GetCount====================");
  *index = -1;
  for (UINT i = 0; i < count; i++) {
    memset(szDeviceID, 0, sizeof(szDeviceID));
    IMMDevice* device;
    {
      IMMDevice* ptrDevice = NULL;
      hr = collection->Item(i, &ptrDevice);
      if (FAILED(hr) || ptrDevice == NULL) {
         printf("..........collection Item failed====================");
        return 1;
      }
      device = ptrDevice;
      SAFE_RELEASE(ptrDevice);
    }

    if (getDeviceID(device, szDeviceID, kDeviceIDLength)) {
        printf("..........getDeviceID failed====================");
      return 1;
    }

    if (wcsncmp(szDefaultDeviceID, szDeviceID, kDeviceIDLength) == 0) {
      // Found a match.
      *index = i;
      break;
    }
  }

  if (*index == -1) {
   printf( "Unable to find collection index for default device");
    return 1;
  }
   SAFE_RELEASE(collection);


  return 0;
}

int YangWinAudioDevice::setVtI4Property(IPropertyStore* ptrPS,REFPROPERTYKEY key,LONG value) {
    PROPVARIANT pv;
    PropVariantInit(&pv);
    pv.vt = VT_I4;
    pv.lVal = value;
    HRESULT hr = ptrPS->SetValue(key, pv);
    PropVariantClear(&pv);
    if (FAILED(hr)) {
        // _TraceCOMError(hr);
        printf("setVtI4Property error.....key=");
        return 1;
    }
    return 0;
}
int YangWinAudioDevice::setBoolProperty(IPropertyStore* ptrPS, REFPROPERTYKEY key,VARIANT_BOOL value) {
    PROPVARIANT pv;
    PropVariantInit(&pv);
    pv.vt = VT_BOOL;
    pv.boolVal = value;
    HRESULT hr = ptrPS->SetValue(key, pv);
    PropVariantClear(&pv);
    if (FAILED(hr)) {
        //_TraceCOMError(hr);
        printf("setVtI4Property error.....key=");
        return 1;
    }
    return 0;
}
