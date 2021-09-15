#include "yangwinaec.h"

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

#pragma comment (lib,"wmcodecdspuuid.lib")
#pragma comment (lib,"Msdmo.lib")
const float MAX_MICROPHONE_VOLUME = 255.0f;
const float MIN_MICROPHONE_VOLUME = 0.0f;

enum { kAecCaptureStreamIndex = 0, kAecRenderStreamIndex = 1 };



class MediaBufferImpl final : public IMediaBuffer {
public:
    explicit MediaBufferImpl(BYTE* p,DWORD maxLength)
        : _data(p),
          _length(0),
          _maxLength(maxLength),
          _refCount(0) {
       // printf("bufersize============================"<<maxLength;

    }
    void init(BYTE* p){
        _data=p;
        _length=0;
    }
    // IMediaBuffer methods.
    STDMETHOD(GetBufferAndLength(BYTE** ppBuffer, DWORD* pcbLength)) {
      //  if (!ppBuffer || !pcbLength) {
           // return E_POINTER;
       // }

       if(ppBuffer) *ppBuffer = _data;
       if(pcbLength) *pcbLength = _length;

        return S_OK;
    }

    STDMETHOD(GetMaxLength(DWORD* pcbMaxLength)) {
        if (!pcbMaxLength) {
            return E_POINTER;
        }

        *pcbMaxLength = _maxLength;
        return S_OK;
    }

    STDMETHOD(SetLength(DWORD cbLength)) {
        if (cbLength > _maxLength) {
            return E_INVALIDARG;
        }

        _length = cbLength;
        return S_OK;
    }

    // IUnknown methods.
    STDMETHOD_(ULONG, AddRef()) { return InterlockedIncrement(&_refCount); }

    STDMETHOD(QueryInterface(REFIID riid, void** ppv)) {
        if (!ppv) {
            return E_POINTER;
        } else if (riid != IID_IMediaBuffer && riid != IID_IUnknown) {
            return E_NOINTERFACE;
        }

        *ppv = static_cast<IMediaBuffer*>(this);
        AddRef();
        return S_OK;
    }

    STDMETHOD_(ULONG, Release()) {
        LONG refCount = InterlockedDecrement(&_refCount);
        if (refCount == 0) {
            delete this;
        }

        return refCount;
    }

private:
    ~MediaBufferImpl() {_data=NULL; }

    BYTE* _data;
    DWORD _length;
    const DWORD _maxLength;
    LONG _refCount;
};



extern "C"  void yang_get_winaec(YangAecType* context){
    context->context=(void*)(new YangWinAec() );
}
extern "C"   int yang_init_aec(YangAecType* context){
    return ((YangWinAec*)context->context)->initAec();
}
extern "C"   int yang_get_aec_buffer(YangAecType* context){
    return ((YangWinAec*)context->context)->getBuffer(context->data,&context->bufLen);
}
extern "C"  void  yang_delete_winaec(YangAecType* context){
    if(!context->context) return;
    delete ((YangWinAec*)context->context);
    context->context=NULL;
}
YangWinAec::YangWinAec()
{
    m_dmo=NULL;
    m_mediaBuffer=NULL;
    m_micIndex=0;
    m_ps=NULL;
    m_dataBuf=new BYTE[640];
    CoCreateInstance(CLSID_CWMAudioAEC, NULL, CLSCTX_INPROC_SERVER,
                                  IID_IMediaObject, reinterpret_cast<void**>(&m_dmo));
   if(m_dmo) m_dmo->QueryInterface(IID_IPropertyStore, reinterpret_cast<void**>(&m_ps));
   m_dwStatus=0;
}
YangWinAec::~YangWinAec()
{
    SAFE_RELEASE(m_ps);
    Yang_Release(m_mediaBuffer);
    Yang_Release(m_dmo);
    delete[] m_dataBuf;
}



int YangWinAec::setDMOProperties() {
    HRESULT hr = S_OK;
    if(m_dmo == NULL) return 1;




    if (FAILED(hr) || m_ps == NULL) {
        //_TraceCOMError(hr);
        printf("get IID_IPropertyStore COM failed!.......");
        return 1;
    }
    // Set the AEC system mode.
    // SINGLE_CHANNEL_AEC - AEC processing only.
    if (setVtI4Property(m_ps, MFPKEY_WMAAECMA_SYSTEM_MODE, SINGLE_CHANNEL_AEC)) {
         printf("set SINGLE_CHANNEL_AEC COM failed!.......");
        return 1;
    }

    // Set the AEC source mode.
    // VARIANT_TRUE - Source mode (we poll the AEC for captured data).
    if (setBoolProperty(m_ps, MFPKEY_WMAAECMA_DMO_SOURCE_MODE, VARIANT_TRUE)) {
         printf("set MFPKEY_WMAAECMA_DMO_SOURCE_MODE COM failed!.......");
        return 1;
    }

    // Enable the feature mode.
    // This lets us override all the default processing settings below.
    if (setBoolProperty(m_ps, MFPKEY_WMAAECMA_FEATURE_MODE, VARIANT_TRUE) ) {
        printf("set MFPKEY_WMAAECMA_FEATURE_MODE COM failed!.......");
        return 1;
    }

    // Disable analog AGC (default enabled).
    if (setBoolProperty(m_ps, MFPKEY_WMAAECMA_MIC_GAIN_BOUNDER, VARIANT_FALSE) ) {
        printf("set MFPKEY_WMAAECMA_MIC_GAIN_BOUNDER COM failed!.......");
        return 1;
    }

    // Disable noise suppression (default enabled).
    // 0 - Disabled, 1 - Enabled
    if (setVtI4Property(m_ps, MFPKEY_WMAAECMA_FEATR_NS, 0)) {
          printf("set MFPKEY_WMAAECMA_FEATR_NS COM failed!.......");
        return 1;
    }

    // Relevant parameters to leave at default settings:
    // MFPKEY_WMAAECMA_FEATR_AGC - Digital AGC (disabled).
    // MFPKEY_WMAAECMA_FEATR_CENTER_CLIP - AEC center clipping (enabled).
    // MFPKEY_WMAAECMA_FEATR_ECHO_LENGTH - Filter length (256 ms).
    //   TODO(andrew): investigate decresing the length to 128 ms.
    // MFPKEY_WMAAECMA_FEATR_FRAME_SIZE - Frame size (0).
    //   0 is automatic; defaults to 160 samples (or 10 ms frames at the
    //   selected 16 kHz) as long as mic array processing is disabled.
    // MFPKEY_WMAAECMA_FEATR_NOISE_FILL - Comfort noise (enabled).
    // MFPKEY_WMAAECMA_FEATR_VAD - VAD (disabled).

    // Set the devices selected by VoE. If using a default device, we need to
    // search for the device index.
    int inDevIndex = m_micIndex;

    int outDevIndex =0;
    this->getDefaultDeviceIndex(m_enum,eRender,eConsole,&outDevIndex);//0;
   // printf("inputIndex=="<<m_micIndex<<"...outputIndex===="<<outDevIndex;

    DWORD devIndex = static_cast<uint32_t>(outDevIndex << 16) +
            static_cast<uint32_t>(0x0000ffff & inDevIndex);
    // printf( "Capture device index: " << inDevIndex   << ", render device index: " << outDevIndex;
    if (setVtI4Property(m_ps, MFPKEY_WMAAECMA_DEVICE_INDEXES, devIndex)) {

                printf("set MFPKEY_WMAAECMA_FEATR_NS COM failed!.......");
        return 1;
    }

    return 0;
}

int YangWinAec::initRecordingDMO() {


    if(m_dmo == NULL) return 1;

    if (setDMOProperties()) {
        printf("..........setDMOProperties failed====================");
        return 1;
    }

    DMO_MEDIA_TYPE mt = {};
    HRESULT hr = MoInitMediaType(&mt, sizeof(WAVEFORMATEX));
    if (FAILED(hr)) {
        MoFreeMediaType(&mt);
        printf("..........MoInitMediaType failed====================");
        return 1;
    }

/**
    int iFrameSize;
    PROPVARIANT pvFrameSize;
    PropVariantInit(&pvFrameSize);
    m_ps->GetValue(MFPKEY_WMAAECMA_FEATR_FRAME_SIZE, &pvFrameSize);
    iFrameSize = pvFrameSize.lVal;
    PropVariantClear(&pvFrameSize);
    printf("..........set framesize==="<<iFrameSize;
**/

    mt.majortype = MEDIATYPE_Audio;
    mt.subtype = MEDIASUBTYPE_PCM;
    mt.formattype = FORMAT_WaveFormatEx;

    WAVEFORMATEX* ptrWav = reinterpret_cast<WAVEFORMATEX*>(mt.pbFormat);
    ptrWav->wFormatTag = WAVE_FORMAT_PCM;
    ptrWav->nChannels = 1;
    ptrWav->nSamplesPerSec = 16000;
    ptrWav->nAvgBytesPerSec = 32000;
    ptrWav->nBlockAlign = 2;
    ptrWav->wBitsPerSample = 16;
    ptrWav->cbSize = 0;

    // Set the VoE format equal to the AEC output format.
    m_recData.framesize = ptrWav->nBlockAlign;
    m_recData.sample = ptrWav->nSamplesPerSec;
    m_recData.blockSize = ptrWav->nSamplesPerSec / 100;
    m_recData.channel = ptrWav->nChannels;

    // Set the DMO output format parameters.
    hr = m_dmo->SetOutputType(kAecCaptureStreamIndex, &mt, 0);
    MoFreeMediaType(&mt);
    if (FAILED(hr)) {
        printf("..........SetOutputType failed====================");
        return 1;
    }
    if(m_mediaBuffer==NULL) m_mediaBuffer = new MediaBufferImpl(m_dataBuf,640);
    hr = m_dmo->AllocateStreamingResources();
    if (FAILED(hr)) {
        return 1;
    }
    return 0;
}
int YangWinAec::initMicrophone(){
    getDefaultDeviceIndex(m_enum,eCapture,eConsole,&m_micIndex);
 //   int ret=getListDevice(m_enum,eCapture, m_micIndex, &m_deviceIn);
 //   if (ret != 0 || (m_deviceIn == NULL)) {
     //   SAFE_RELEASE(m_deviceIn);
       // return 1;
  //  }
   // printf("deviceIn....."<<m_deviceIn;
   /** SAFE_RELEASE(m_captureVolume);
    ret = m_deviceIn->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL,
                               reinterpret_cast<void**>(&m_captureVolume));
    if (ret != 0 || m_captureVolume == NULL) {
        SAFE_RELEASE(m_captureVolume);
        return 1;
    }**/

    return 0;
    // if(m_deviceIn==NULL)    getDefaultDevice(eCapture,eCommunications,&m_deviceIn);
}
int YangWinAec::initCapture() {
    if (initMicrophone()) {
        return 1;
    }

   // if (m_deviceIn == NULL) {
    //    printf("..............initCapture failed,m_deviceIn=="<<m_deviceIn;
     //   return 1;
  //  }

    return initRecordingDMO();

}

int YangWinAec::getBuffer(unsigned char* p,unsigned long* pbufLen){
    m_mediaBuffer->SetLength(0);
    do{

             OutputBufferStruct.dwStatus = 0;
             HRESULT   hr = m_dmo->ProcessOutput(0, 1, &OutputBufferStruct, &m_dwStatus);

                if (hr == S_FALSE) {
                  //  printf("ProcessOutput failed..");
                    *pbufLen = 0;
                    return 1;
                } else {
                    hr = m_mediaBuffer->GetBufferAndLength(NULL,  pbufLen);
                }

            } while (OutputBufferStruct.dwStatus & DMO_OUTPUT_DATA_BUFFERF_INCOMPLETE);

   // if ( *pbufLen) {
      //  printf("size=="<<  *pbufLen;
   // }
    if(*pbufLen) memcpy(p,m_dataBuf,*pbufLen);
    return 0;
}
int YangWinAec::initAec(){
    int ret=this->initCapture();
    if(ret) return ret;
    OutputBufferStruct.pBuffer = m_mediaBuffer;
    OutputBufferStruct.pBuffer->AddRef();
    return 0;
    //MediaBufferImpl* buf=(MediaBufferImpl*)m_mediaBuffer;
}
int YangWinAec::captureAecThread() {
    bool keepRecording = true;

    // Set event which will ensure that the calling thread modifies the
    // recording state to true.

    HRESULT hr=0;


    ULONG cbProduced=0;
    while (keepRecording) {
        //getBuffer(&cbProduced);
        Sleep(10);
    }

    return hr;
}
