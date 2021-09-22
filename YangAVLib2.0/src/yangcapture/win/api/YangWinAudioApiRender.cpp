#include "YangWinAudioApiRender.h"
#ifdef _WIN32
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>
#include <endpointvolume.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <uuids.h>
#include <unistd.h>
#include <Errors.h>
#include <yangavutil/audio/YangAudioUtil.h>
#include <yangutil/sys/YangLog.h>
#include <memory.h>
#define Yang_Release(x) if(x){x->Release();x=NULL;}
#define ROUND(x) ((x) >= 0 ? (int)((x) + 0.5) : (int)((x)-0.5))


#define CONTINUE_ON_ERROR(x) if(x!=S_OK){continue;}

const float MAX_SPEAKER_VOLUME = 255.0f;
const float MIN_SPEAKER_VOLUME = 0.0f;

YangWinAudioApiRender::YangWinAudioApiRender(YangAudioInfo *pini) {
    m_outputDeviceIndex = 0;
    m_renderCollection = NULL;
    m_deviceOut = NULL;
    m_clientOut = NULL;
    m_renderClient = NULL;
    m_renderSimpleVolume = NULL;
    m_dataBufP = NULL;
    m_init = 0;
   // m_in_audioBuffer = NULL;
    m_resTmp=NULL;
    m_resBuf=NULL;
    m_aec=NULL;
   // m_buffer=NULL;
   // m_44ktmp=NULL;
    m_bufferLength = 0;
    keepPlaying = false;
    flags = 0;
    padding = 0;
    framesAvailable = 0;
    m_audioPlayCacheNum = pini->audioPlayCacheNum;
    m_audioData.initPlay(pini->sample, pini->channel);
    m_sample=pini->sample;
   // m_is16k=pini->sample==16000?1:0;
    m_size = (pini->sample / 50) * pini->channel * 2;


    m_loops=0;
    m_isStart=0;
    m_samplesReadyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

   /**
    *    if(m_is16k){
        m_res.init(1, 16000, 48000);
        m_resBuf = new uint8_t[8192];
        m_resTmp = new uint8_t[8192];
    }
    *  if(pini->sample==44100){
        m_res.init(2, 44100, 48000);
        m_buffer=new uint8_t[8192*4];
        m_44ktmp=new uint8_t[8192*4];

    }**/
    m_bufLen=0;
   m_hasRenderEcho=0;
   m_blockSize=4;
}
YangWinAudioApiRender::~YangWinAudioApiRender() {
    if (NULL != m_samplesReadyEvent) {
        CloseHandle(m_samplesReadyEvent);
        m_samplesReadyEvent = NULL;
    }
    Yang_Release(m_renderCollection);
    Yang_Release(m_deviceOut);
    Yang_Release(m_clientOut);
    Yang_Release(m_renderClient);
    Yang_Release(m_renderSimpleVolume);
    yang_deleteA(m_resTmp);
    yang_deleteA(m_resBuf);
   // yang_deleteA(m_buffer);
   // yang_deleteA(m_44ktmp);
   // m_in_audioBuffer = NULL;
    m_aec=NULL;
}

void YangWinAudioApiRender::setAudioList(YangAudioPlayBuffer *pal) {
	m_audioData.setAudioList(pal) ;
}

void YangWinAudioApiRender::setAec(YangAecBase *paec){

    m_aec=paec;
}
int YangWinAudioApiRender::setSpeakerVolume(int volume) {
    if (m_deviceOut == NULL) {
        return 1;
    }
    if (volume < (int) MIN_SPEAKER_VOLUME
            || volume > (int) MAX_SPEAKER_VOLUME) {
        return 1;
    }

    HRESULT hr = S_OK;

    // scale input volume to valid range (0.0 to 1.0)
    const float fLevel = (float) volume / MAX_SPEAKER_VOLUME;
    //  m_lock.lock();
    hr = m_renderSimpleVolume->SetMasterVolume(fLevel, NULL);
    // m_lock.unlock();
    if (FAILED(hr))
        return 1;
    return 0;

}
int YangWinAudioApiRender::getSpeakerVolume(int &volume) {
    if (m_deviceOut == NULL) {
        return 1;
    }

    HRESULT hr = S_OK;
    float fLevel(0.0f);

    //m_lock.lock();
    hr = m_renderSimpleVolume->GetMasterVolume(&fLevel);
    // m_lock.unlock();
    if (FAILED(hr))
        return 1;

    // scale input volume range [0.0,1.0] to valid output range
    volume = static_cast<uint32_t>(fLevel * MAX_SPEAKER_VOLUME);

    return 0;

}
int YangWinAudioApiRender::getSpeakerMute(bool &enabled) {

    if (m_deviceOut == NULL) {
        return 1;
    }

    HRESULT hr = S_OK;
    IAudioEndpointVolume *pVolume = NULL;

    // Query the speaker system mute state.
    hr = m_deviceOut->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL,
                               reinterpret_cast<void**>(&pVolume));
    if (FAILED(hr))
        return 1;

    BOOL mute;
    hr = pVolume->GetMute(&mute);
    if (FAILED(hr))
        return 1;

    enabled = (mute == TRUE) ? true : false;

    Yang_Release(pVolume);

    return 0;

}
int YangWinAudioApiRender::setSpeakerMute(bool enable) {
    if (m_deviceOut == NULL) {
        return 1;
    }

    HRESULT hr = S_OK;
    IAudioEndpointVolume *pVolume = NULL;

    // Set the speaker system mute state.
    hr = m_deviceOut->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL,
                               reinterpret_cast<void**>(&pVolume));
    if (FAILED(hr))
        return 1;

    const BOOL mute(enable);
    hr = pVolume->SetMute(mute, NULL);
    if (FAILED(hr))
        return 1;

    Yang_Release(pVolume);

    return 0;

}
int YangWinAudioApiRender::initSpeaker(int pind) {
    // int  nDevices = playoutDeviceCount();
    // if(pind>=nDevices) return 1;
    // getListDevice(m_enum, eRender, pind, &m_deviceOut);
    IAudioSessionManager *pManager = NULL;
    int ret = m_deviceOut->Activate(__uuidof(IAudioSessionManager), CLSCTX_ALL,
                                    NULL, (void**) &pManager);
    if (ret != 0 || pManager == NULL) {
        Yang_Release(pManager);
        return 1;
    }
    Yang_Release(m_renderSimpleVolume);
    ret = pManager->GetSimpleAudioVolume(NULL, FALSE, &m_renderSimpleVolume);
    if (ret != 0 || m_renderSimpleVolume == NULL) {
        Yang_Release(pManager);
        Yang_Release(m_renderSimpleVolume);
        return 1;
    }
    Yang_Release(pManager);
    return 0;
}

int YangWinAudioApiRender::initPlay(int pind) {
    if (m_deviceOut == NULL) {
        yang_error("1_Init play failed device is null");
        return 1;
    }

    // Initialize the speaker (devices might have been added or removed)
    if (initSpeaker(pind)) {
        yang_error("InitSpeaker() failed");
    }

    // Ensure that the updated rendering endpoint device is valid
    if (m_deviceOut == NULL) {
        yang_error("2_Init play failed device is null");
        return 1;
    }

    HRESULT hr = S_OK;
    WAVEFORMATEX *pWfxOut = NULL;
    WAVEFORMATEX Wfx = WAVEFORMATEX();
    WAVEFORMATEX *pWfxClosestMatch = NULL;

    // Create COM object with IAudioClient interface.
    Yang_Release(m_clientOut);
    hr = m_deviceOut->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL,
                               (void**) &m_clientOut);
    if (FAILED(hr)) return yang_error_wrap(1,"create IAudioClient fail ...");


    hr = m_clientOut->GetMixFormat(&pWfxOut);
    // Set wave format
    Wfx.wFormatTag = WAVE_FORMAT_PCM;
    Wfx.wBitsPerSample = 16;
    Wfx.cbSize = 0;
    const int freqs[] = { 48000, 44100, 16000, 96000, 32000, 8000 };
    hr = S_FALSE;
    int _playChannelsPrioList[2];
    _playChannelsPrioList[0] = 2;  // stereo is prio 1
    _playChannelsPrioList[1] = 1;  // mono is prio 2
    // Iterate over frequencies and channels, in order of priority
    for (unsigned int freq = 0; freq < sizeof(freqs) / sizeof(freqs[0]);
         freq++) {
        for (unsigned int chan = 0;
             chan
             < sizeof(_playChannelsPrioList)
             / sizeof(_playChannelsPrioList[0]); chan++) {
            Wfx.nChannels = _playChannelsPrioList[chan];
            Wfx.nSamplesPerSec = freqs[freq];
            Wfx.nBlockAlign = Wfx.nChannels * Wfx.wBitsPerSample / 8;
            Wfx.nAvgBytesPerSec = Wfx.nSamplesPerSec * Wfx.nBlockAlign;
            // If the method succeeds and the audio endpoint device supports the
            // specified stream format, it returns S_OK. If the method succeeds and
            // provides a closest match to the specified format, it returns S_FALSE.
            hr = m_clientOut->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &Wfx,
                                                &pWfxClosestMatch);
            if (hr == S_OK) {
                break;
            } else {
                if (pWfxClosestMatch) {

                    CoTaskMemFree(pWfxClosestMatch);
                    pWfxClosestMatch = NULL;
                }
            }
        }
        if (hr == S_OK)
            break;
    }

    // TODO(andrew): what happens in the event of failure in the above loop?
    //   Is _ptrClientOut->Initialize expected to fail?
    //   Same in InitRecording().
    if (hr == S_OK) {
        m_playData.framesize = Wfx.nBlockAlign;
        // Block size is the number of samples each channel in 10ms.

        m_playData.sample = Wfx.nSamplesPerSec;
        // The device itself continues to run at 44.1 kHz.
        m_playData.blockSize = Wfx.nSamplesPerSec / 100;
        m_blockSize=Wfx.nSamplesPerSec / 100;
        m_playData.channel = Wfx.nChannels;

    }
    yang_trace("\nsample==%d,channle==%d,nBlockAlign==%d,blockSize==%d",Wfx.nSamplesPerSec,Wfx.nChannels,Wfx.nBlockAlign,m_blockSize);
    m_audioData.initRender(Wfx.nSamplesPerSec, Wfx.nChannels);
    REFERENCE_TIME hnsBufferDuration = 20 * 10000; // ask for minimum buffer size (default)
    if (m_playData.sample == 44100) {
        hnsBufferDuration = 30 * 10000;
    }
    hr = m_clientOut->Initialize(AUDCLNT_SHAREMODE_SHARED, // share Audio Engine with other applications
                                 AUDCLNT_STREAMFLAGS_EVENTCALLBACK, // processing of the audio buffer by
                                 // the client will be event driven
                                 hnsBufferDuration,  // requested buffer capacity as a time value (in
                                 // 100-nanosecond units)
                                 0,// periodicity
                                 &Wfx,               // selected wave format
                                 NULL);              // session GUID

    if (FAILED(hr))
        return yang_error_wrap(1,"AudioClient Initialize param fail ...");

    // Get the actual size of the shared (endpoint buffer).
    // Typical value is 960 audio frames <=> 20ms @ 48kHz sample rate.

    hr = m_clientOut->GetBufferSize(&m_bufferLength);
    yang_trace("\nbufersize==================%d",m_bufferLength);
    if (SUCCEEDED(hr)) {
        //  RTC_LOG(LS_VERBOSE) << "IAudioClient::GetBufferSize() => "
        //                       << bufferFrameCount << " (<=> "
        //                       << bufferFrameCount * _playAudioFrameSize << " bytes)";
    }

    // Set the event handle that the system signals when an audio buffer is ready
    // to be processed by the client.
    hr = m_clientOut->SetEventHandle(m_samplesReadyEvent);
    if (FAILED(hr))
        return yang_error_wrap(1,"SetEventHandle fail ...");
    //  EXIT_ON_ERROR(hr);

    // Get an IAudioRenderClient interface.
    Yang_Release(m_renderClient);
    hr = m_clientOut->GetService(__uuidof(IAudioRenderClient),
                                 (void**) &m_renderClient);
    if (FAILED(hr))
        return yang_error_wrap(1,"create AudioRenderClient  fail ...");

    // Mark playout side as initialized
    // _playIsInitialized = true;

    CoTaskMemFree(pWfxOut);
    CoTaskMemFree(pWfxClosestMatch);

    //RTC_LOG(LS_VERBOSE) << "render side is now initialized";
    return 0;

}

int YangWinAudioApiRender::startRender() {
    uint32_t err=Yang_Ok;
    if (m_clientOut) {
        err=m_clientOut->Start();
        if(err != S_OK ) {
            return yang_error_wrap(err,"client start fail..");
        }
        return Yang_Ok;
    }
    yang_error("client is null");
    return 1;
}
int YangWinAudioApiRender::stopRender() {
    if (m_clientOut) {
        m_clientOut->Stop();
        return 0;
    }

    return 1;
}
int YangWinAudioApiRender::init() {

    if (m_init)
        return 0;
    //getDefaultDeviceIndex(m_enum, eRender, eConsole, &m_outputDeviceIndex);
    m_enum->GetDefaultAudioEndpoint(eRender, eConsole, &m_deviceOut);
    // getListDevice(m_enum,eRender, m_outputDeviceIndex,&m_deviceOut);

    int err=Yang_Ok;
    err=initPlay(m_outputDeviceIndex);
    if(err) {
        return yang_error_wrap(err,"init render fail.......");
    }
    // BYTE* pData = NULL;
    HRESULT hr = m_renderClient->GetBuffer(m_bufferLength, &m_dataBufP);
    if (FAILED(hr))
        return yang_error_wrap(1,"renderClient getBuffer fail.......");
    hr = m_renderClient->ReleaseBuffer(m_bufferLength,AUDCLNT_BUFFERFLAGS_SILENT);
    if (FAILED(hr)) return yang_error_wrap(1,"renderClient ReleaseBuffer fail.......");

    m_init = 1;
    return 0;
}
/**
int YangWinAudioApiRender::render_16k(uint8_t *pdata,int nb) {
    uint32_t inlen = 320;
    uint32_t outlen = 960 ;
    uint32_t outbuflen=6400;
    m_res.resample((const short*)pdata , inlen, (short*) m_resTmp, &outbuflen);

    MonoToStereo((short*) m_resTmp, (short*) m_resBuf, outlen );

    render_48k(m_resBuf,outlen*4);
    return 0;
}

int YangWinAudioApiRender::render_44k(uint8_t *pdata,int nb) {
    uint32_t inFrame = 882;
    uint32_t inlen=inFrame*4;
    uint32_t outlen = 960 * 4;
    uint32_t outbuflen=6400;
    memcpy(m_buffer+m_bufLen,pdata,nb);
    m_bufLen+=nb;

    m_res.resample((const short*) m_buffer, inFrame, (short*) m_resBuf, &outbuflen);
    render_48k(m_resBuf,outlen);
    m_bufLen-=inlen;
    if(m_bufLen>=inlen){
        m_res.resample((const short*) (m_buffer+inlen), inFrame, (short*) m_resBuf, &outbuflen);
        render_48k(m_resBuf,outlen);
        m_bufLen-=inlen;
    }
    if(m_bufLen>0){
        memcpy(m_44ktmp,m_buffer+inlen,m_bufLen);
        memcpy(m_buffer,m_44ktmp,m_bufLen);
    }
    return 0;
}
int YangWinAudioApiRender::render_48k(uint8_t *pdata,int nb) {
    HRESULT hr = m_clientOut->GetCurrentPadding(&padding);
    if (FAILED(hr))       return -1;
    //*nb=0;
    m_dataBufP=NULL;
    framesAvailable = m_bufferLength - padding;
    hr = m_renderClient->GetBuffer(framesAvailable, &m_dataBufP);
    yang_trace("%d,",framesAvailable);

    if(m_dataBufP&&framesAvailable>0&&framesAvailable<=960) {

        memcpy(m_dataBufP,pdata,framesAvailable<<2);
       // *nb=framesAvailable<<2;



    }
    m_renderClient->ReleaseBuffer(framesAvailable, flags);
    return Yang_Ok;
}

**/
int YangWinAudioApiRender::render_10ms(){
	   HRESULT hr = m_clientOut->GetCurrentPadding(&padding);
	    if (FAILED(hr))        return yang_error_wrap(1,"outClient GetCurrentPadding fail.......");
	    m_dataBufP=NULL;
	    framesAvailable = m_bufferLength - padding;
	   // yang_trace("%d,",framesAvailable);
	    if(framesAvailable<m_blockSize) return Yang_Ok;
	    if(framesAvailable>0) {
	    	const uint32_t n10ms = (framesAvailable /m_blockSize);
	    	//yang_trace("n10m_%d,",n10ms);
	    	for (uint32_t n = 0; n < n10ms; n++) {
	    	        // Get pointer (i.e., grab the buffer) to next space in the shared
	    	        // render buffer.
	    	        hr = m_renderClient->GetBuffer(m_blockSize, &m_dataBufP);
	    	        if (FAILED(hr))
	    	              return yang_error_wrap(1,"renderClient getBuffer fail.......");

	    	        if(m_dataBufP) {
	    	        	uint8_t* tmp=m_audioData.getRenderAudioData(m_blockSize<<2);
	    	        	if(tmp)
	    	        		memcpy(m_dataBufP,tmp,m_blockSize<<2);
	    	        	else
	    	        		memset(m_dataBufP,0,m_blockSize<<2);
	    	        }

	    	        DWORD dwFlags(0);
	    	        hr = m_renderClient->ReleaseBuffer(m_blockSize, dwFlags);
	    	         if (FAILED(hr))          return yang_error_wrap(1,"renderClient getBuffer fail.......");


	    	      }



	    }
	    return Yang_Ok;
}


int YangWinAudioApiRender::render_aec_10ms(){
	   HRESULT hr = m_clientOut->GetCurrentPadding(&padding);
	    if (FAILED(hr))        return yang_error_wrap(1,"outClient GetCurrentPadding fail.......");
	    m_dataBufP=NULL;
	    framesAvailable = m_bufferLength - padding;
	   // yang_trace("%d,",framesAvailable);
	    if(framesAvailable<m_blockSize) return Yang_Ok;
	    if(framesAvailable>0) {
	    	const uint32_t n10ms = (framesAvailable /m_blockSize);
	    	//yang_trace("n10m_%d,",n10ms);
	    	for (uint32_t n = 0; n < n10ms; n++) {
	    	        // Get pointer (i.e., grab the buffer) to next space in the shared
	    	        // render buffer.
	    	        hr = m_renderClient->GetBuffer(m_blockSize, &m_dataBufP);
	    	        if (FAILED(hr))
	    	              return yang_error_wrap(1,"renderClient getBuffer fail.......");

	    	        if(m_dataBufP) {
	    	        	uint8_t* tmp=m_audioData.getRenderAudioData(m_blockSize<<2);
	    	        	if(tmp)
	    	        		memcpy(m_dataBufP,tmp,m_blockSize<<2);
	    	        	else
	    	        		memset(m_dataBufP,0,m_blockSize<<2);
	    	        }

	    	        DWORD dwFlags(0);
	    	        hr = m_renderClient->ReleaseBuffer(m_blockSize, dwFlags);
	    	        if(m_audioData.n20ms){
	    	        	if(m_aec) m_aec->echo_playback((short*)m_audioData.getAecAudioData());
	    	        	if(!m_hasRenderEcho) m_hasRenderEcho=1;
	    	        	m_audioData.n20ms=false;
	    	        }
	    	         if (FAILED(hr))          return yang_error_wrap(1,"renderClient getBuffer fail.......");


	    	      }



	    }
	    return Yang_Ok;

}


void YangWinAudioApiRender::run() {
    m_isStart=1;
    startLoop();
    m_isStart=0;
}
void YangWinAudioApiRender::stop() {
    stopLoop();
}
void YangWinAudioApiRender::stopLoop() {

    m_loops = 0;
}

 void YangWinAudioApiRender::startLoop() {

    m_loops = 1;

    if(startRender()){
        yang_error("start render fail");
        return;
    }


    HANDLE waitArray[1] = {m_samplesReadyEvent};
    while (m_loops == 1) {
    	DWORD waitResult = WaitForMultipleObjects(1, waitArray, FALSE, 500);
    	if(waitResult==(WAIT_OBJECT_0 + 0)){
    		render_10ms();
    	}


    }
    stopRender();

}

#endif
