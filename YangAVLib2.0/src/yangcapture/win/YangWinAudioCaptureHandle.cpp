#include "unistd.h"
#ifdef _WIN32
#include <strmif.h>
#include "YangWinAudioCaptureHandle.h"

YangWinAudioCaptureHandle::YangWinAudioCaptureHandle(YangAudioInfo *pini):YangAudioCaptureHandle(pini)
{
	readStart=0;
	m_player=new YangAudioPlaySdl(pini);
	if(m_player) m_player->init();
	if (pini->usingMono) {
		m_len=320*2;
	} else {
		m_len = 1024*4;
	}
	m_buf=new uint8_t[m_len];
	//m_preProcess=NULL;
	m_aec=NULL;

}
YangWinAudioCaptureHandle::~YangWinAudioCaptureHandle(void)
{
	if(m_player) delete m_player;
	m_player=NULL;
	if(m_buf) delete[] m_buf;
	m_buf=NULL;
}


   STDMETHODIMP_(ULONG) YangWinAudioCaptureHandle::AddRef() { return 1; }
   STDMETHODIMP_(ULONG) YangWinAudioCaptureHandle::Release() { return 2; }

    STDMETHODIMP YangWinAudioCaptureHandle::QueryInterface(REFIID riid, void **ppvObject)
    {
       // printf("*********************************\n");
		if (NULL == ppvObject) return E_POINTER;
        if (riid == __uuidof(IUnknown))
        {
            *ppvObject = static_cast<IUnknown*>(this);
             return S_OK;
        }
        if (riid == IID_ISampleGrabber)
        {
            *ppvObject = static_cast<ISampleGrabberCB*>(this);
             return S_OK;
        }
        return E_NOTIMPL;
    }

    STDMETHODIMP YangWinAudioCaptureHandle::SampleCB(double Time, IMediaSample *pSample)
    {
		//printf(".len=%d..\n",pSample->GetSize());
		//list->putAudio(
    	//printf("")
        return E_NOTIMPL;
    }

    STDMETHODIMP YangWinAudioCaptureHandle::BufferCB(double Time, BYTE *pBuffer, long BufferLen)
    {
    	//printf()
    	//printf("a%d,",BufferLen);
    	memset(m_buf,0,m_len);
        if(m_player->hasData())    		m_player->getData(m_buf);

    	m_player->playSDL(m_buf);
        if (readStart)	putEchoPlay((short*) m_buf,BufferLen);

        if (!readStart&&m_player->hasData())		readStart = 1;
    	if (readStart)
                    putEchoBuffer(pBuffer,BufferLen);
    	else
                    putBuffer1(pBuffer,BufferLen);


	return E_NOTIMPL;
    }
//int32_t tcou=0;

#endif
