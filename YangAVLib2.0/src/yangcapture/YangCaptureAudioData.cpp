#include <yangutil/sys/YangLog.h>
#include <yangavutil/audio/YangAudioUtil.h>
#include <yangcapture/YangCaptureAudioData.h>

YangCaptureAudioData::YangCaptureAudioData() {
	m_cacheLen = 1024 * 4 * 4;
	m_cache = new uint8_t[m_cacheLen];
	m_size = 0;
	m_pos = 0;


	m_cb = NULL;
	m_res.init(48000, 2, 48000, 2, 20);

}

YangCaptureAudioData::~YangCaptureAudioData() {
	yang_deleteA(m_cache);

	m_cb = NULL;
}

void YangCaptureAudioData::initIn(int psample, int pchannel) {
	m_res.initIn(psample, pchannel);

}

void YangCaptureAudioData::initOut(int psample, int pchannel) {
	m_res.initOut(psample, pchannel);
}

void YangCaptureAudioData::caputure(YangFrame *audioFrame) {

	if ((m_pos + m_size + audioFrame->nb) >= m_cacheLen) {
		memmove(m_cache, m_cache + m_pos, m_size);
		m_pos = 0;
	}

	if (audioFrame->payload && audioFrame->nb > 0) {

		memcpy(m_cache + m_pos + m_size, audioFrame->payload, audioFrame->nb);
		m_size += audioFrame->nb;
	}

	while(m_size >= m_res.getInBytes()) {
		if (m_pos + m_res.getInBytes() >= m_cacheLen) {
			memmove(m_cache, m_cache + m_pos, m_size);
			m_pos = 0;
		}
		captureData();

		m_pos += m_res.getInBytes();
		m_size -= m_res.getInBytes();
	}

}

int YangCaptureAudioData::getOutLength(){
	return m_res.getOutBytes();
}

void YangCaptureAudioData::captureData() {
	m_audioFrame.payload=m_cache + m_pos;
	m_audioFrame.nb=m_res.getInBytes();
	m_res.resample(&m_audioFrame);

	if (m_cb)
		m_cb->caputureAudioData(&m_audioFrame);

}

