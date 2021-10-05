#include "YangRtcReceive.h"

#include <yangutil/yang_unistd.h>
#include <yangutil/sys/YangLog.h>
YangRtcReceive::YangRtcReceive(YangContext* pcontext) {
	m_context=pcontext;
	m_isStart = 0;
	m_out_videoBuffer = NULL;
	m_out_audioBuffer = NULL;
	m_isReceived = 0;
	m_loops = 0;
	m_headLen = 1; //pcontext->audio.audioDecoderType == 0 ? 2 : 1;
	m_recv = NULL;
	m_waitState = 0;
	pthread_mutex_init(&m_lock,NULL);
	pthread_cond_init(&m_cond_mess,NULL);
//	m_lock = PTHREAD_MUTEX_INITIALIZER;
//	m_cond_mess = PTHREAD_COND_INITIALIZER;

}

YangRtcReceive::~YangRtcReceive() {
	disConnect();
	if (m_loops) {
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	yang_delete(m_recv);

	//yang_deleteA(m_keyBuf);
	m_out_audioBuffer = NULL;
	m_out_videoBuffer = NULL;
	pthread_mutex_destroy(&m_lock);
	pthread_cond_destroy(&m_cond_mess);
}

void YangRtcReceive::disConnect() {
	if (m_recv)
		m_recv->disConnectServer();
	stop();

	yang_delete(m_recv);
}
void YangRtcReceive::setBuffer(YangAudioEncoderBuffer *al,
		YangVideoDecoderBuffer *vl) {
	m_out_audioBuffer = al;
	m_out_videoBuffer = vl;
}
void YangRtcReceive::setMediaConfig(int32_t puid, YangAudioParam *audio,YangVideoParam *video){

}
void YangRtcReceive::receiveAudio(YangFrame *audioFrame) {

	m_out_audioBuffer->putPlayAudio(audioFrame);
}
void YangRtcReceive::receiveVideo(YangFrame *videoFrame) {
	uint8_t *temp = videoFrame->payload;
    //yang_trace("%d,",videoFrame->nb);
	if (temp && (temp[0] == 0x17 || temp[0] == 0x1c) && temp[1] == 0x00) {
		videoFrame->frametype = YANG_Frametype_Spspps;
		m_out_videoBuffer->putEVideo(videoFrame);
	} else if (temp
			&& (temp[0] == 0x17 || temp[0] == 0x27 || temp[0] == 0x1c
					|| temp[0] == 0x2c) && temp[1] == 0x01) {
		if (temp[0] == 0x17 && temp[9] == 0x67) {
			int32_t spsLen = *(temp + 8) + 1;
			int32_t ppsLen = *(temp + 8 + spsLen + 2 + 1) + 1;
			int32_t allLen = 8 + spsLen + ppsLen + 2 + 1;
			int32_t kenLen = videoFrame->nb - allLen;

			uint8_t *sps = temp + 8 + 1;
			uint8_t *pps = temp + 8 + spsLen + 1 + 1 + 2;
			uint8_t meta[200] = { 0 };
			uint8_t *p = meta;
			*p++ = 0x17;
			*p++ = 0x0; // avc_type: sequence header
			*p++ = 0x0; // composition time
			*p++ = 0x0;
			*p++ = 0x0;
			*p++ = 0x01; // version
			*p++ = sps[1];
			*p++ = sps[2];
			*p++ = sps[3];
			*p++ = 0xff;
			*p++ = 0xe1;
			*p++ = 0x00;
			*p++ = spsLen;
			memcpy(p, sps, spsLen);
			p += spsLen;
			*p++ = 0x01;
			*p++ = 0x00;
			*p++ = ppsLen;
			memcpy(p, pps, ppsLen);
			p += ppsLen;
			videoFrame->payload = meta;
			videoFrame->nb = p - meta;
			videoFrame->frametype = YANG_Frametype_Spspps;
			m_out_videoBuffer->putEVideo(videoFrame);
			videoFrame->frametype = YANG_Frametype_I;
			videoFrame->payload = temp + allLen;
			videoFrame->nb = kenLen;
			m_out_videoBuffer->putEVideo(videoFrame);
		} else {
			videoFrame->payload = temp + 5;
			videoFrame->nb -= 5;
			videoFrame->frametype =(*temp == 0x17 || *temp == 0x1c) ?YANG_Frametype_I : YANG_Frametype_P;
			m_out_videoBuffer->putEVideo(videoFrame);

		}
	}
}

int32_t YangRtcReceive::init(int32_t puid, string localIp, int32_t localPort,
		string server, int32_t pport, string app, string stream) {
	m_conf.localIp = localIp;
	m_conf.localPort = localPort;
	m_conf.serverIp = server;
	m_conf.serverPort = pport;
	m_conf.app = app;
	m_conf.stream = stream;
	m_conf.uid = puid;
	m_conf.streamOptType = Yang_Stream_Play;
	if (!m_recv)
		m_recv = YangRtcHandle::createRtcHandle(m_context);
	m_recv->setReceiveCallback(this);

	//m_recv->setMediaParam(NULL, NULL, &m_rtcinfo);

	m_recv->init(&m_conf);
	return Yang_Ok;

}
void YangRtcReceive::stop() {
	m_loops = 0;
	if (m_recv)
		m_recv->disConnectServer();
	//pthread_mutex_unlock(&m_lock);

	if (m_waitState) {
		pthread_mutex_lock(&m_lock);
		pthread_cond_signal(&m_cond_mess);
		pthread_mutex_unlock(&m_lock);

	}
}
void YangRtcReceive::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangRtcReceive::startLoop() {

	yang_reindex(m_out_audioBuffer);
	yang_reindex(m_out_videoBuffer);
	m_loops = 1;
	m_isReceived = 1;
	if (m_recv->connectRtcServer()) {

	}

	pthread_mutex_lock(&m_lock);
	while (m_loops == 1) {
		m_waitState = 1;

		pthread_cond_wait(&m_cond_mess, &m_lock);
		m_waitState = 0;
	}	            		//end while

	if (m_recv)
		m_recv->disConnectServer();
	m_isReceived = 0;
	pthread_mutex_unlock(&m_lock);
}
