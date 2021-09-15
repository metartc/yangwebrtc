#include "malloc.h"
#include <unistd.h>
#include <yangdecoder/YangAudioDecoderHandle.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLog.h>
#include "stdlib.h"

#include "yangdecoder/YangDecoderFactory.h"

YangAudioDecoderHandle::YangAudioDecoderHandle(YangAudioInfo *pini) {
	m_isInit = 0;
	m_isStart = 0;
	m_isConvert = 1;
	m_in_audioBuffer = NULL;
	m_decs = NULL;
	m_out_audioBuffer = NULL;
	m_ini = pini;
	m_frameSize = 1024;
	m_channel = 2;
	m_param = new YangAudioParam();
	m_param->encode = (YangAudioEncDecType) pini->audioDecoderType;
	m_param->sample = pini->sample;
	m_param->channel = pini->channel;
}

YangAudioDecoderHandle::~YangAudioDecoderHandle(void) {
	if (m_isConvert) {
		stop();
		while (m_isStart) {
			usleep(1000);
		}
	}
	m_ini = NULL;
	m_in_audioBuffer = NULL;
	m_out_audioBuffer = NULL;

	if (m_decs) {

		delete m_decs;
		m_decs = NULL;

	}

}
void YangAudioDecoderHandle::stop() {
	stopLoop();
}

void YangAudioDecoderHandle::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}

YangAudioPlayBuffer* YangAudioDecoderHandle::getOutAudioBuffer() {

	return m_out_audioBuffer;
}

void YangAudioDecoderHandle::init() {
	m_isInit = 1;
}
void YangAudioDecoderHandle::setRemoteParam(YangAudioParam *para) {
	m_param->encode = para->encode;
	m_param->sample = para->sample;
	m_param->channel = para->channel;

}
void YangAudioDecoderHandle::setInAudioBuffer(YangAudioEncoderBuffer *pbuf) {
	m_in_audioBuffer = pbuf;
}
void YangAudioDecoderHandle::setOutAudioBuffer(YangAudioPlayBuffer *pbuf) {
	m_out_audioBuffer = pbuf;
}

void YangAudioDecoderHandle::onAudioData(YangFrame *pframe) {
	if (m_out_audioBuffer)
		m_out_audioBuffer->putAudio(pframe);
}
void YangAudioDecoderHandle::onVideoData(YangFrame *pframe) {

}
void YangAudioDecoderHandle::startLoop() {
	m_isConvert = 1;
	uint8_t srcAudioSource[600];
	int32_t audioCacheSize = m_ini->audioPlayCacheNum;
	YangDecoderFactory ydf;
	YangFrame audioFrame;
	while (m_isConvert == 1) {
		if (!m_in_audioBuffer) {
			usleep(1000);
			continue;
		}
		if (m_in_audioBuffer->m_size == 0) {
			usleep(200);
			continue;
		}
		audioFrame.payload = srcAudioSource;
		m_in_audioBuffer->getPlayAudio(&audioFrame);

		if (!m_decs) {
			m_decs = ydf.createAudioDecoder(m_param);

			m_decs->m_uid = audioFrame.uid;
			m_decs->init();
		}

		if (m_decs){
			if(m_decs->decode(&audioFrame, this)){
				yang_error("decode audio fail..");
			}
		}

		if (m_in_audioBuffer && m_in_audioBuffer->m_size > audioCacheSize)
			m_in_audioBuffer->resetIndex();

	}			//end

}

void YangAudioDecoderHandle::stopLoop() {
	m_isConvert = 0;

}
