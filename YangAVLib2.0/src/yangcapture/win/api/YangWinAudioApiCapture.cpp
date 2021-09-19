#include "YangWinAudioApiCapture.h"
#ifdef _WIN32
#include <yangutil/sys/YangLog.h>
#include <assert.h>
#include <ksmedia.h>

#define SAFE_RELEASE(x) if(x){x->Release();x=NULL;}
#define Yang_Release(x) if(x){x->Release();x=NULL;}


const float MAX_MICROPHONE_VOLUME = 255.0f;
const float MIN_MICROPHONE_VOLUME = 0.0f;
YangWinAudioApiCapture::YangWinAudioApiCapture()
{
    m_inputDeviceIndex=0;
    m_captureCollection=NULL;
    m_deviceIn=NULL;
     m_clientIn=NULL;
     m_captureClient=NULL;
     m_captureVolume=NULL;
}
YangWinAudioApiCapture::~YangWinAudioApiCapture()
{

    Yang_Release(m_captureCollection);
    Yang_Release(m_deviceIn);
    Yang_Release(m_clientIn);
    Yang_Release(m_captureClient);
    Yang_Release(m_captureVolume);
}




int YangWinAudioApiCapture::setMicrophoneVolume(int volume)
{


    if (volume < static_cast<int>(MIN_MICROPHONE_VOLUME) ||
            volume > static_cast<int>(MAX_MICROPHONE_VOLUME)) {
        return 1;
    }

    HRESULT hr = S_OK;
    // scale input volume to valid range (0.0 to 1.0)
    const float fLevel = static_cast<float>(volume) / MAX_MICROPHONE_VOLUME;

 //   m_lock.lock();
    m_captureVolume->SetMasterVolumeLevelScalar(fLevel, NULL);
//    m_lock.unlock();
   if(FAILED(hr)) return 1;

    return 0;


}
int YangWinAudioApiCapture::getMicrophoneVolume(int& volume)  {

    HRESULT hr = S_OK;
    float fLevel(0.0f);
    volume = 0;
  //  m_lock.lock();
    hr = m_captureVolume->GetMasterVolumeLevelScalar(&fLevel);
  //  m_lock.unlock();
    if(FAILED(hr)) return 1;

    // scale input volume range [0.0,1.0] to valid output range
    volume = static_cast<int>(fLevel * MAX_MICROPHONE_VOLUME);

    return 0;


}

int YangWinAudioApiCapture::setMicrophoneMute(bool enable) {

    if (m_deviceIn == NULL) {
        return 1;
    }

    HRESULT hr = S_OK;
    IAudioEndpointVolume* pVolume = NULL;

    // Set the microphone system mute state.
    hr = m_deviceIn->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL,
                              reinterpret_cast<void**>(&pVolume));
 if(FAILED(hr)) return 1;

    const BOOL mute(enable);
    hr = pVolume->SetMute(mute, NULL);
   if(FAILED(hr)) return 1;

    SAFE_RELEASE(pVolume);
    return 0;

}


int YangWinAudioApiCapture::initMicrophone(){
    //eCommunications eConsole
    int ret=getListDevice(m_enum,eCapture, m_inputDeviceIndex, &m_deviceIn);
    if (ret != 0 || (m_deviceIn == NULL)) {
        SAFE_RELEASE(m_deviceIn);
        return yang_error_wrap(1,"get capture devicein fail...");
    }

    SAFE_RELEASE(m_captureVolume);
    ret = m_deviceIn->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL,
                               reinterpret_cast<void**>(&m_captureVolume));
    if (ret != 0 || m_captureVolume == NULL) {
        SAFE_RELEASE(m_captureVolume);
        return 1;
    }
   // qDebug()<<"m_captureVolume....."<<m_captureVolume;
    return 0;
    // if(m_deviceIn==NULL)    getDefaultDevice(eCapture,eCommunications,&m_deviceIn);
}
int YangWinAudioApiCapture::initCapture() {
    getDefaultDeviceIndex(m_enum,eRender,eConsole,&m_inputDeviceIndex);
      if (initMicrophone()) {
        return yang_error_wrap(1,"init microphone fail...");
    }
    if (m_deviceIn == NULL) {
        yang_error("..............initCapture failed");
        return 1;
    }

    HRESULT hr = S_OK;
    WAVEFORMATEX* pWfxIn = NULL;
   // WAVEFORMATEXTENSIBLE Wfx = WAVEFORMATEXTENSIBLE();
   // WAVEFORMATEX Wfx = WAVEFORMATEX();
    WAVEFORMATEX* pWfxClosestMatch = NULL;

    // Create COM object with IAudioClient interface.
    SAFE_RELEASE(m_clientIn);
    hr = m_deviceIn->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL,
                              (void**)&m_clientIn);
   // qDebug()<<"m_clientIn....."<<m_clientIn;
    if(FAILED(hr)) return yang_error_wrap(1,"create capture audioclient fail...");

    // Retrieve the stream format that the audio engine uses for its internal
    // processing (mixing) of shared-mode streams.
    hr = m_clientIn->GetMixFormat(&pWfxIn);
    if (SUCCEEDED(hr)) {

    }


    // Create a capturing stream.
    hr = m_clientIn->Initialize(
                AUDCLNT_SHAREMODE_SHARED,  // share Audio Engine with other applications
                AUDCLNT_STREAMFLAGS_EVENTCALLBACK |  // processing of the audio buffer by
                // the client will be event driven
                AUDCLNT_STREAMFLAGS_NOPERSIST,   // volume and mute settings for an
                // audio session will not persist
                // across system restarts
                0,                    // required for event-driven shared mode
                0,                    // periodicity
                (WAVEFORMATEX*)&pWfxIn,  // selected wave format
                NULL);

   if(FAILED(hr))  return yang_error_wrap(1," capture audioclient Initialize fail...");


    hr = m_clientIn->GetBufferSize(&m_bufferLength);

     // Get an IAudioCaptureClient interface.
    SAFE_RELEASE(m_captureClient);
    hr = m_clientIn->GetService(__uuidof(IAudioCaptureClient),
                                (void**)&m_captureClient);
    if(FAILED(hr)) return yang_error_wrap(1," captureclient Initialize fail...");

    CoTaskMemFree(pWfxIn);
    CoTaskMemFree(pWfxClosestMatch);
    return 0;

}
int YangWinAudioApiCapture::startCpature(){
    if(m_clientIn){
         HRESULT hr=m_clientIn->Start();
         if(FAILED(hr)) return yang_error_wrap(1,"capure clientin start fail...");
    }
    return yang_error_wrap(1,"capure clientin is null");

}
int YangWinAudioApiCapture::stopCapture(){
    if(m_clientIn) {
        m_clientIn->Stop();
    }
}
int YangWinAudioApiCapture::captureFrame(YangFrame* audioFrame){
    audioFrame->nb=0;
    while (packetLength) {

       HRESULT hr = m_captureClient->GetBuffer(
                    &pData,            // packet which is ready to be read by used
                    &framesAvailable,  // #frames in the captured packet (can be zero)
                    &flags,            // support flags (check)
                    &recPos,    // device position of first audio frame in data packet
                    &recTime);  // value of performance counter at the time of recording
        if (SUCCEEDED(hr)) {
            if (AUDCLNT_S_BUFFER_EMPTY == hr) {
                // Buffer was empty => start waiting for a new capture notification
                // event
                // _UnLock();
                break;
            }

            if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
                pData = NULL;
            }

            //  assert(framesAvailable != 0);

            if (pData&&audioFrame->payload) {
                memcpy(audioFrame->payload+audioFrame->nb,pData,framesAvailable<<2);
                audioFrame->nb+=framesAvailable<<2;
                yang_trace("%d,",audioFrame->nb);
            }

            hr = m_captureClient->ReleaseBuffer(framesAvailable);

           hr = m_captureClient->GetNextPacketSize(&packetLength);

        }
}
    return 0;
}
void YangWinAudioApiCapture::captureThread() {
    bool keepRecording = true;
    HRESULT hr = S_OK;
    BYTE* syncBuffer = NULL;
    UINT32 syncBufIndex = 0;
    const UINT32 syncBufferSize = 2 * (m_bufferLength * m_recData.framesize);
    syncBuffer = new BYTE[syncBufferSize];
    hr = m_clientIn->Start();
    if(FAILED(hr)) return;

     if (m_captureClient == NULL || m_clientIn == NULL) return;
    while (keepRecording) {
        Sleep(5);
         hr = m_captureClient->GetNextPacketSize(&packetLength);
        captureFrame(NULL);
            //_UnLock();

    }



    if (m_clientIn) {
        hr = m_clientIn->Stop();
    }


    return ;
}
#endif
