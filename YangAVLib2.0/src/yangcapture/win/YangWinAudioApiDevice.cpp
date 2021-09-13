
#include "YangWinAudioApiDevice.h"
#ifdef _WIN32
#include <unistd.h>
#include <yangutil/yangtype.h>
YangWinAudioApiDevice::YangWinAudioApiDevice(YangAudioInfo *pini) {
	m_aec = NULL;
	m_ahandle = new YangAudioCaptureHandle(pini);
	m_isStart=0;
	m_isInit=0;
	m_ret=0;
	m_size=640;
	m_loops=0;
#if UsingApi
	m_player = new YangWinAudioApiRender(pini);
	if (m_player)
		m_player->initRender();
#else
    m_player=new YangAudioPlaySdl(pini);
    if(m_player) m_player->init();
#endif
    m_lib.loadObject("YangWinAec");
	yang_get_winaec =(void (*)(YangAecType* context)) m_lib.loadFunction("yang_get_winaec");
	yang_init_aec =(int (*)(YangAecType* context)) m_lib.loadFunction("yang_init_aec");
	yang_get_aec_buffer  = (int (*)(YangAecType* context)) m_lib.loadFunction("yang_get_aec_buffer");
	yang_delete_winaec  = (void (*)(YangAecType* context)) m_lib.loadFunction("yang_delete_winaec");
	m_aec=new YangAecType();
	m_aec->audioIndex=pini->aIndex;
	m_aec->bufLen=0;
	m_aec->context=NULL;
	m_aec->data=NULL;
	if(yang_get_winaec) yang_get_winaec(m_aec);
	if(yang_init_aec) yang_init_aec(m_aec);
}

YangWinAudioApiDevice::~YangWinAudioApiDevice() {
	yang_delete(m_ahandle);
	yang_delete(m_player);
	if (m_aec && yang_delete_winaec)		yang_delete_winaec(m_aec);
	m_lib.unloadObject();
	yang_delete_winaec = NULL;
	yang_get_aec_buffer = NULL;
	yang_init_aec=NULL;
	yang_get_winaec=NULL;
}

void YangWinAudioApiDevice::setCatureStart() {
	m_ahandle->isBuf = 1;
}
void YangWinAudioApiDevice::setCatureStop() {
	m_ahandle->isBuf = 0;
}
void YangWinAudioApiDevice::setOutAudioBuffer(YangAudioBuffer *pbuffer) {
	m_ahandle->setOutAudioBuffer(pbuffer);
}
void YangWinAudioApiDevice::setPlayAudoBuffer(YangAudioBuffer *pbuffer) {
	m_ahandle->m_aecPlayBuffer = pbuffer;
}
void YangWinAudioApiDevice::setAec(YangAecBase *paec) {
	m_ahandle->m_aec = paec;
}
void YangWinAudioApiDevice::setPreProcess(YangPreProcess *pp) {

}

void YangWinAudioApiDevice::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal) {
	m_player->setInAudioBuffer(pal);
}
void YangWinAudioApiDevice::stopLoop() {
	m_loops = 0;

}

void YangWinAudioApiDevice::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}

int YangWinAudioApiDevice::init() {
	if (m_isInit)
		return Yang_Ok;

	m_isInit = 1;
	return Yang_Ok;
}

void YangWinAudioApiDevice::startLoop() {
	int pindex = 0;
	//  if(m_aec==NULL) {
	// YangWinAecFactory fac;
	// m_aec=fac.getWinAec(pindex);
	// if(m_aec->initAec()) return;
	//  }

	m_loops = 1;
	int len = 0;
	uint8_t *audioBuf = new uint8_t[640];
	uint8_t *bufs = new uint8_t[2048];
	uint8_t *tmp = new uint8_t[2048];
	uint8_t *play_buf = new uint8_t[640];
	int bufLen = 0;
	YangAutoFreeA(uint8_t, audioBuf);
	YangAutoFreeA(uint8_t, bufs);
	YangAutoFreeA(uint8_t, tmp);
	YangAutoFreeA(uint8_t, play_buf);
	int first=1;
	//int captureData=0;
	int left=0;
	if(m_aec) m_aec->data=audioBuf;
	while (m_loops) {
		m_aec->bufLen=0;
		len=0;

		if(yang_get_aec_buffer) {
			int ret=yang_get_aec_buffer(m_aec);
			len=m_aec->bufLen;
			printf("%d-%d,",len,ret);
		}
         usleep(10000);
         if(len==0) continue;
        // captureData=0;
		if (len) {
            if(bufLen) memcpy(bufs, tmp, bufLen);
            memcpy(bufs+bufLen, audioBuf, len);
			bufLen += len;
			if (bufLen >= 640) {
				m_ahandle->putBuffer1(bufs, 640);
				bufLen -= 640;
				if(bufLen) memcpy(tmp, bufs + 640, bufLen);

				memset(play_buf, 0, 640);
				if (m_player->hasData())
					m_player->getData(play_buf);
	#if UsingApi
				m_player->render(play_buf);
	#else
                // m_player->playSDL(play_buf);
	#endif

			}


		}
	}
	//YANG_DELETES(pcm_read);
}
#endif
