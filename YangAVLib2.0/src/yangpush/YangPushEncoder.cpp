#include "pthread.h"
#include <iostream>
#include <yangutil/yang_unistd.h>
#include <yangpush/YangPushEncoder.h>
#include "yangencoder/YangEncoderFactory.h"

YangPushEncoder::YangPushEncoder(YangContext *pini) {
	m_ae=NULL;
	m_ve=NULL;
	m_out_videoBuffer = NULL;
	m_out_auidoBuffer = NULL;

	m_ini = pini;
	m_vmd = NULL;
}

YangPushEncoder::~YangPushEncoder() {
	stopAll();
	yang_stop_thread(m_ae);
	yang_stop_thread(m_ve);

	yang_delete(m_ae);
	yang_delete(m_ve);

	yang_delete(m_out_videoBuffer);	//=NULL;
	yang_delete(m_out_auidoBuffer);	//=NULL;

	yang_free(m_vmd);
	m_ini = NULL;
}
void YangPushEncoder::stopAll() {
	if (m_ae)	m_ae->stop();
	if (m_ve)	m_ve->stop();
}
YangVideoMeta* YangPushEncoder::getOutVideoMetaData() {
	return m_vmd;
}

void YangPushEncoder::initAudioEncoder() {
	if (m_out_auidoBuffer == NULL)
		m_out_auidoBuffer = new YangAudioEncoderBuffer(m_ini->audio.audioCacheNum);
	if (m_ae == NULL) {
		//	YangEncoderFactory yf;
		m_ae = new YangAudioEncoderHandle(&m_ini->audio);
		m_ae->setOutAudioBuffer(m_out_auidoBuffer);
		m_ae->init();
	}

}
void YangPushEncoder::initVideoEncoder() {
	if (m_out_videoBuffer == NULL)
		m_out_videoBuffer = new YangVideoEncoderBuffer(m_ini->video.evideoCacheNum);
	if (m_vmd == NULL)
		m_vmd = (YangVideoMeta*) calloc(1, sizeof(YangVideoMeta));
	YangEncoderFactory fac;
	YangVideoEncoderMeta *yvh = fac.createVideoEncoderMeta(&m_ini->video);
	yvh->yang_initVmd(m_vmd, &m_ini->video,&m_ini->enc);
	yang_delete(yvh);
	if (m_ve == NULL) {
		//	YangEncoderFactory yf;
		m_ve = new YangVideoEncoderHandle(&m_ini->video, &m_ini->enc);
		m_ve->setOutVideoBuffer(m_out_videoBuffer);
		m_ve->init();
		m_ve->setVideoMetaData(m_vmd);
	}
}
void YangPushEncoder::sendKeyframe(){
	if(m_ve) m_ve->sendKeyframe();
}
void YangPushEncoder::startAudioEncoder() {
	if (m_ae && !m_ae->m_isStart) {
		m_ae->start();
		yang_usleep(1000);
	}
}
void YangPushEncoder::startVideoEncoder() {
	if (m_ve && !m_ve->m_isStart) {
		m_ve->start();
		yang_usleep(2000);
	}
}
void YangPushEncoder::setInAudioBuffer(YangAudioBuffer *pbuf) {
	if (m_ae != NULL)
		m_ae->setInAudioBuffer(pbuf);
}
void YangPushEncoder::setInVideoBuffer(YangVideoBuffer *pbuf) {
	if (m_ve != NULL)
		m_ve->setInVideoBuffer(pbuf);
}
YangAudioEncoderBuffer* YangPushEncoder::getOutAudioBuffer() {
	return m_out_auidoBuffer;
}
YangVideoEncoderBuffer* YangPushEncoder::getOutVideoBuffer() {
	return m_out_videoBuffer;
}

