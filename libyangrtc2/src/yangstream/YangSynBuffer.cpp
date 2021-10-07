#include<cmath>
#include <string>
#include <string.h>
#include <yangstream/YangSynBuffer.h>
#include <yangutil/sys/YangLog.h>
YangSynBuffer::YangSynBuffer() {
	m_transtype = Yang_Webrtc;
	if (m_transtype == Yang_Webrtc) {
		m_videoClock = 90000;
		m_audioClock = 48000;
	} else if (m_transtype == Yang_Rtmp) {
		m_videoClock = 1000;
		m_audioClock = 1000;
	}

	m_audioBase = 0;
	m_videoBase = 0;
	m_audioTime = 0;
	m_videoTime = 0;
	m_pre_audioTime = 0;
	m_pre_videoTime = 0;
	m_audioMinus = 0;
	m_videoMinus = 0;


    m_isFirstAudio=false;
    m_isFirstVideo=false;
    m_video_time_state=0;
    m_video_startClock=0;
	m_baseClock = 0;
	m_startClock = 0;
	m_paused = 0;
	m_uid = 0;
	m_width = 0;
	m_height = 0;

    m_synType = YANG_SYNC_AUDIO_MASTER;
    m_maxAudioMinus = Yang_Max_Audio_Intervaltime;
    m_maxVideoMinus = Yang_Max_Video_Intervaltime;
	m_videoBuffer = NULL;
	m_audioBuffer = NULL;
}

YangSynBuffer::~YangSynBuffer() {

	m_videoBuffer = NULL;
	m_audioBuffer = NULL;
}

void YangSynBuffer::setInVideoBuffer(YangVideoBuffer *pbuf) {
	m_videoBuffer = pbuf;
}
void YangSynBuffer::setInAudioBuffer(YangAudioPlayBuffer *pbuf) {
	m_audioBuffer = pbuf;
}
void YangSynBuffer::setTranstype(int transtype) {
	m_transtype = transtype;

	if (m_transtype == Yang_Webrtc) {
		m_videoClock = 90000;
		m_audioClock = 48000;
	} else if (m_transtype == Yang_Rtmp) {
		m_videoClock = 1000;
		m_audioClock = 1000;
	}
}
uint8_t* YangSynBuffer::getVideoRef(YangFrame *pframe) {
	if (!m_videoBuffer || !m_videoBuffer->m_size)
		return NULL;
	uint8_t *tmp = NULL;
    int err=0;
    if ((err=playVideoFrame(m_videoBuffer->getNextTimestamp()))==Yang_Ok) {

		tmp = m_videoBuffer->getVideoRef(pframe);
		m_width = m_videoBuffer->m_width;
		m_height = m_videoBuffer->m_height;

    }else  if(err==-1){
        tmp = m_videoBuffer->getVideoRef(pframe);
        m_width = m_videoBuffer->m_width;
        m_height = m_videoBuffer->m_height;
        return getVideoRef(pframe);
    }
	return tmp;
}

uint8_t* YangSynBuffer::getAudioRef(YangFrame *audioFrame) {
	if (!m_audioBuffer || !m_audioBuffer->m_size)
		return NULL;
	if (playAudioFrame(m_audioBuffer->getNextTimestamp()))		return m_audioBuffer->getAudios(audioFrame);

	return NULL;

}

int32_t YangSynBuffer::getAudioSize() {
	if (m_audioBuffer)
		return m_audioBuffer->m_size;
	return 0;
}
int32_t YangSynBuffer::getVideoSize() {
	if (m_videoBuffer)
		return m_videoBuffer->m_size;
	return 0;
}

bool YangSynBuffer::playAudioFrame(int64_t pts) {

	if (m_audioBase == 0) {
        if(!m_isFirstAudio){
            //clear cache
            yang_reindex(m_audioBuffer);
            m_isFirstAudio=true;
            return false;
        }
		if (m_synType == YANG_SYNC_AUDIO_MASTER)
			setClock();
		m_audioBase = pts;
	}
    if (m_transtype == Yang_Webrtc)
        m_audioTime = (pts - m_audioBase) * 1000 / m_audioClock;
    else
        m_audioTime = (pts - m_audioBase);
	m_audioMinus = m_audioTime - (yang_get_milli_tick() - m_startClock);



	m_pre_audioTime = m_audioTime;

	return m_audioMinus <= m_maxAudioMinus;
}
int cou=0;
int YangSynBuffer::playVideoFrame(int64_t pts) {
    if(!m_startClock) return false;
	if (m_videoBase == 0) {
        if(!m_isFirstVideo){
            //clear cache
            yang_reindex(m_videoBuffer);
            m_isFirstVideo=true;
            return false;
        }


		m_videoBase = pts;
        m_video_startClock=yang_get_milli_tick();
		if (m_synType == YANG_SYNC_VIDEO_MASTER)
			setClock();
	}
	if (m_transtype == Yang_Webrtc)
		m_videoTime = (pts - m_videoBase) * 1000 / m_videoClock;
	else
		m_videoTime = pts - m_videoBase;

    m_videoMinus = m_videoTime+Yang_Video_Cache_time - (yang_get_milli_tick() - m_video_startClock);

    if(m_videoMinus<0) {
        m_video_time_state++;
        if(m_video_time_state>5){
            m_videoBase = pts;
            m_video_startClock=yang_get_milli_tick();
        }
        return -1;
    }else{
        m_video_time_state=0;
    }
    if(m_videoMinus <= m_maxVideoMinus) return Yang_Ok;


    return 1;

}
void YangSynBuffer::setClock() {
	m_startClock = yang_get_milli_tick();
}

