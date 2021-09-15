#include "YangWinAudioApiRender.h"
#ifdef _WIN32
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>
#include <endpointvolume.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <uuids.h>

#include <Errors.h>
#include <yangavutil/audio/YangAudioUtil.h>

#define Yang_Release(x) if(x){x->Release();x=NULL;}
#define SAFE_RELEASE(x) if(x){x->Release();x=NULL;}
#define ROUND(x) ((x) >= 0 ? (int)((x) + 0.5) : (int)((x)-0.5))
//#pragma comment (lib,"Quarz.lib")

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
	m_in_audioBuffer = NULL;

	m_bufferLength = 0;
	keepPlaying = false;
	flags = 0;
	padding = 0;
	framesAvailable = 0;
	m_audioPlayCacheNum = pini->audioPlayCacheNum;
	m_size = (pini->sample / 50) * pini->channel * 2;
	m_res.init(2, 16000, 48000);
	m_resBuf = new uint8_t[4096];
	m_resTmp = new uint8_t[m_size * 2];

	//  m_clock=NULL;

}
YangWinAudioApiRender::~YangWinAudioApiRender() {
	// Yang_Release(m_clock);
	Yang_Release(m_renderCollection);
	Yang_Release(m_deviceOut);
	Yang_Release(m_clientOut);
	Yang_Release(m_renderClient);
	Yang_Release(m_renderSimpleVolume);
	yang_deleteA(m_resTmp);
	yang_deleteA(m_resBuf);
	m_in_audioBuffer = NULL;
}
void YangWinAudioApiRender::setInAudioBuffer(
		std::vector<YangAudioPlayBuffer*> *pal) {
	m_in_audioBuffer = pal;
}
int32_t YangWinAudioApiRender::hasData() {
	if (!m_in_audioBuffer)
		return 0;
	for (int32_t i = 0; i < (int) m_in_audioBuffer->size(); i++) {
		//al=audioList->at(i);
		//if(audioList->at(i)==NULL) printf("\n%d__%d*****************has null value\n",audioList->size(),i);
		if (m_in_audioBuffer->at(i)->m_size > 0)
			return 1;
	}
	//al=NULL;
	return 0;
}
int32_t YangWinAudioApiRender::getData(uint8_t *pcm_write) {
	//YangFrame* frame=NULL;
	uint8_t *tmp = NULL;
	for (size_t i = 0; i < m_in_audioBuffer->size(); i++) {
		if (m_in_audioBuffer->at(i) && m_in_audioBuffer->at(i)->m_size > 0) {
			//YangFrame* frame=m_in_audioBuffer->at(i)->getAudios();

			tmp = m_in_audioBuffer->at(i)->getAudios(&m_audioFrame);
			if (tmp) {
				//if (m_preProcess)	m_preProcess->preprocess_run((short*) tmp);
				if (i == 0) {
					memcpy(pcm_write, tmp, m_size);
				} else {
					m_mix.yangMix1(pcm_write, tmp, m_size, 128);
				}
			}
			tmp = NULL;

			if (m_in_audioBuffer->at(i)->m_size > m_audioPlayCacheNum)
				m_in_audioBuffer->at(i)->resetIndex();
		}
	}
	return Yang_Ok;
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

	SAFE_RELEASE(pVolume);

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

	SAFE_RELEASE(pVolume);

	return 0;

}
int YangWinAudioApiRender::initSpeaker(int pind) {
	// int  nDevices = playoutDeviceCount();
	// if(pind>=nDevices) return 1;
	getListDevice(m_enum, eRender, pind, &m_deviceOut);
	IAudioSessionManager *pManager = NULL;
	int ret = m_deviceOut->Activate(__uuidof(IAudioSessionManager), CLSCTX_ALL,
	NULL, (void**) &pManager);
	if (ret != 0 || pManager == NULL) {
		SAFE_RELEASE(pManager);
		return 1;
	}
	SAFE_RELEASE(m_renderSimpleVolume);
	ret = pManager->GetSimpleAudioVolume(NULL, FALSE, &m_renderSimpleVolume);
	if (ret != 0 || m_renderSimpleVolume == NULL) {
		SAFE_RELEASE(pManager);
		SAFE_RELEASE(m_renderSimpleVolume);
		return 1;
	}
	SAFE_RELEASE(pManager);
	return 0;
}

int YangWinAudioApiRender::initPlay(int pind) {
	if (m_deviceOut == NULL) {
		return 1;
	}

	// Initialize the speaker (devices might have been added or removed)
	if (initSpeaker(pind)) {
		// RTC_LOG(LS_WARNING) << "InitSpeaker() failed";
	}

	// Ensure that the updated rendering endpoint device is valid
	if (m_deviceOut == NULL) {
		return 1;
	}

	HRESULT hr = S_OK;
	WAVEFORMATEX *pWfxOut = NULL;
	WAVEFORMATEX Wfx = WAVEFORMATEX();
	WAVEFORMATEX *pWfxClosestMatch = NULL;

	// Create COM object with IAudioClient interface.
	SAFE_RELEASE(m_clientOut);
	hr = m_deviceOut->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL,
			(void**) &m_clientOut);
	if (FAILED(hr))
		return 1;

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
		m_playData.channel = Wfx.nChannels;

	}

	// Create a rendering stream.
	//
	// ****************************************************************************
	// For a shared-mode stream that uses event-driven buffering, the caller must
	// set both hnsPeriodicity and hnsBufferDuration to 0. The Initialize method
	// determines how large a buffer to allocate based on the scheduling period
	// of the audio engine. Although the client's buffer processing thread is
	// event driven, the basic buffer management process, as described previously,
	// is unaltered.
	// Each time the thread awakens, it should call
	// IAudioClient::GetCurrentPadding to determine how much data to write to a
	// rendering buffer or read from a capture buffer. In contrast to the two
	// buffers that the Initialize method allocates for an exclusive-mode stream
	// that uses event-driven buffering, a shared-mode stream requires a single
	// buffer.
	// ****************************************************************************
	//
	REFERENCE_TIME hnsBufferDuration = 20 * 10000; // ask for minimum buffer size (default)
	if (m_playData.sample == 44100) {
		// Ask for a larger buffer size (30ms) when using 44.1kHz as render rate.
		// There seems to be a larger risk of underruns for 44.1 compared
		// with the default rate (48kHz). When using default, we set the requested
		// buffer duration to 0, which sets the buffer to the minimum size
		// required by the engine thread. The actual buffer size can then be
		// read by GetBufferSize() and it is 20ms on most machines.
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
		return 1;

	// Get the actual size of the shared (endpoint buffer).
	// Typical value is 960 audio frames <=> 20ms @ 48kHz sample rate.

	hr = m_clientOut->GetBufferSize(&m_bufferLength);
	if (SUCCEEDED(hr)) {
		//  RTC_LOG(LS_VERBOSE) << "IAudioClient::GetBufferSize() => "
		//                       << bufferFrameCount << " (<=> "
		//                       << bufferFrameCount * _playAudioFrameSize << " bytes)";
	}

	// Set the event handle that the system signals when an audio buffer is ready
	// to be processed by the client.
	//  hr = m_clientOut->SetEventHandle(_hRenderSamplesReadyEvent);
	//  EXIT_ON_ERROR(hr);

	// Get an IAudioRenderClient interface.
	SAFE_RELEASE(m_renderClient);
	hr = m_clientOut->GetService(__uuidof(IAudioRenderClient),
			(void**) &m_renderClient);
	if (FAILED(hr))
		return 1;

	// Mark playout side as initialized
	// _playIsInitialized = true;

	CoTaskMemFree(pWfxOut);
	CoTaskMemFree(pWfxClosestMatch);

	//RTC_LOG(LS_VERBOSE) << "render side is now initialized";
	return 0;

}

int YangWinAudioApiRender::startRender() {
	if (m_clientOut) {
		return m_clientOut->Start() == S_OK ? 0 : 1;

	}

	return 1;
}
int YangWinAudioApiRender::stopRender() {
	if (m_clientOut) {
		m_clientOut->Stop();
		return 0;
	}

	return 1;
}
int YangWinAudioApiRender::initRender() {

	if (m_init)
		return 0;
	getDefaultDeviceIndex(m_enum, eRender, eConsole, &m_outputDeviceIndex);
	initPlay(m_outputDeviceIndex);
	// BYTE* pData = NULL;
	HRESULT hr = m_renderClient->GetBuffer(m_bufferLength, &m_dataBufP);
	if (FAILED(hr))
		return 1;
	hr = m_renderClient->ReleaseBuffer(m_bufferLength,
			AUDCLNT_BUFFERFLAGS_SILENT);
	if (FAILED(hr))
		return 1;
	m_init = 1;
	return 0;
}

int YangWinAudioApiRender::render(uint8_t *pdata) {
	uint32_t inlen = 640;
	uint32_t outlen = 960 * 4;

	single2Double((short*) pdata, (short*) m_resTmp, m_size / 2);
	m_res.resample((const short*) m_resTmp, &inlen, (short*) m_resBuf, &outlen);
	HRESULT hr = m_clientOut->GetCurrentPadding(&padding);
	if (FAILED(hr))
		return 1;

	framesAvailable = m_bufferLength - padding;
	hr = m_renderClient->GetBuffer(framesAvailable, &m_dataBufP);
	//copy data to pdata
	memcpy(m_dataBufP, pdata, framesAvailable);
	m_renderClient->ReleaseBuffer(framesAvailable, flags);
	return 0;
}

int YangWinAudioApiRender::playThread() {
	keepPlaying = true;

	HRESULT hr = S_OK;
	startRender();
	while (keepPlaying) {
		Sleep(5);
		render(NULL);

	}

	Sleep(10);
	stopRender();
	return (DWORD) hr;
}
#endif
