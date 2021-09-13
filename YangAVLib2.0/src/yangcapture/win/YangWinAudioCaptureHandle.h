#ifndef ___YangWinAudioCaptureHandlee__
#define ___YangWinAudioCaptureHandlee__
#ifdef _WIN32
#include "qedit.h"
//#include <dshow.h>
//#include "yanavgutil/audio/YangAecBase.h"
#include "yangavutil/audio/YangMakeWave.h"
#include "yangutil/buffer/YangAudioBuffer.h"
#include "../YangAudioCaptureHandle.h"
#include "YangAudioPlaySdl.h"
//#include "yangutil/sys/YangIniImpl.h"
//#define SIZE_AUDIO_FRAME 1024
class YangWinAudioCaptureHandle: public ISampleGrabberCB,public YangAudioCaptureHandle
{
public:
    YangWinAudioCaptureHandle(YangAudioInfo *pini);
	virtual ~YangWinAudioCaptureHandle(void);
	int32_t   readStart;
	YangAudioPlaySdl *m_player;
	STDMETHODIMP_(ULONG) AddRef();
	    STDMETHODIMP_(ULONG) Release();
	    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
		STDMETHODIMP SampleCB(double Time, IMediaSample *pSample);
	    STDMETHODIMP BufferCB(double Time, BYTE *pBuffer, long BufferLen);
private:
	    uint8_t *m_buf;
	    int32_t m_len;

};
#endif
#endif
