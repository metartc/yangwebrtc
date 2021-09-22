#include "YangWinAudioApiDevice.h"
#ifdef _WIN32
#include <unistd.h>
#include <yangutil/yangtype.h>
#include <yangavutil/audio/YangAudioUtil.h>
#include "api/YangWinAudioApiAec.h"
YangWinAudioApiDevice::YangWinAudioApiDevice(YangAudioInfo *pini,
		bool usingBuiltinAec) {
	m_usingBuiltinAec = usingBuiltinAec;
	m_isStart = 0;
	m_isInit = 0;
	m_loops = 0;
	if (m_usingBuiltinAec)
		m_capture = new YangWinAudioApiAec(pini);
	else
		m_capture = new YangWinAudioApiCapture(pini);
	m_ahandle = new YangAudioCaptureHandle(pini);
	m_player = new YangWinAudioApiRenders(pini);

	m_player->init();
	m_capture->initCapture();
	m_lock = PTHREAD_MUTEX_INITIALIZER;
	m_cond_mess = PTHREAD_COND_INITIALIZER;

}

YangWinAudioApiDevice::~YangWinAudioApiDevice() {
	yang_stop_thread(this);
	yang_stop_thread(m_player);
	yang_delete(m_ahandle);
	yang_delete(m_player);
	yang_delete(m_capture);

}

void YangWinAudioApiDevice::setCatureStart() {
	if(m_ahandle) m_ahandle->isBuf = 1;
}
void YangWinAudioApiDevice::setCatureStop() {
	if(m_ahandle) m_ahandle->isBuf = 0;
}
void YangWinAudioApiDevice::setOutAudioBuffer(YangAudioBuffer *pbuffer) {
	if(m_ahandle) m_ahandle->setOutAudioBuffer(pbuffer);
}
void YangWinAudioApiDevice::setPlayAudoBuffer(YangAudioBuffer *pbuffer) {
	//m_player->m_audioData.setAudioList(pbuffer);
	if(m_ahandle) m_ahandle->m_aecPlayBuffer = pbuffer;
}
void YangWinAudioApiDevice::setAec(YangAecBase *paec) {
	if(!m_usingBuiltinAec){
		if(m_ahandle) m_ahandle->m_aec = paec;
		if (m_player)
			m_player->setAec(paec);
	}

}
void YangWinAudioApiDevice::setPreProcess(YangPreProcess *pp) {
if(!m_usingBuiltinAec)	m_player->m_audioData.m_preProcess = pp;

}

void YangWinAudioApiDevice::setInAudioBuffer(
		vector<YangAudioPlayBuffer*> *pal) {
	m_player->m_audioData.setInAudioBuffer(pal);

}
void YangWinAudioApiDevice::caputureAudioData(YangFrame *audioFrame) {
	if (m_usingBuiltinAec)
		if(m_ahandle) m_ahandle->putBuffer1(audioFrame->payload, 640);
	else {
		if (m_player->m_hasRenderEcho){
			if(m_ahandle) m_ahandle->putEchoBuffer(audioFrame->payload, 640); //aec filter render data
		}else{
			if(m_ahandle) m_ahandle->putBuffer1(audioFrame->payload, 640);
		}
	}
}
void YangWinAudioApiDevice::stop() {
	stopLoop();
}

void YangWinAudioApiDevice::stopLoop() {
	m_loops = 0;
	pthread_mutex_lock(&m_lock);
	pthread_cond_signal(&m_cond_mess);
	pthread_mutex_unlock(&m_lock);

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
	m_loops = 1;
	m_capture->setCaptureCallback(this);
	if (m_capture)		m_capture->start();
	if (m_player)		m_player->start();
	while (m_loops) {
		pthread_cond_wait(&m_cond_mess, &m_lock);
	}
	if (m_player)
		m_player->stop();

	if (m_capture)
		m_capture->stopCapture();

}

#endif
