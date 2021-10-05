#include<cmath>
#include <string>
#include <string.h>
#include <yangstream/YangSynBuffer.h>
#include <yangutil/sys/YangLog.h>
YangSynBuffer::YangSynBuffer() {
	m_maxMinus=20;
	m_baseClock=0;
	m_startClock=0;
	m_paused=0;
	 m_uid=0;
	 m_width=0;
	 m_height=0;

    m_videoBuffer=NULL;
    m_audioBuffer=NULL;


}

YangSynBuffer::~YangSynBuffer() {

    m_videoBuffer=NULL;
    m_audioBuffer=NULL;
}

void YangSynBuffer::setInVideoBuffer(YangVideoBuffer *pbuf){
    m_videoBuffer=pbuf;
}
void YangSynBuffer::setInAudioBuffer(YangAudioPlayBuffer *pbuf){
	m_audioBuffer=pbuf;
}

uint8_t* YangSynBuffer::getVideoRef(YangFrame* pframe){
    uint8_t* tmp=NULL;
    if(m_videoBuffer) {
        tmp= m_videoBuffer->getVideoRef(pframe);
        m_width=m_videoBuffer->m_width;
        m_height=m_videoBuffer->m_height;
    }
    return tmp;
}

uint8_t* YangSynBuffer::getAudioRef(YangFrame* audioFrame){
    //printf("audio_%d,",m_audioBuffer->m_size);
	if(m_audioBuffer) return m_audioBuffer->getAudios(audioFrame);
	return NULL;
}


int32_t YangSynBuffer::getAudioSize(){
	if(m_audioBuffer) return m_audioBuffer->m_size;
	return 0;
}
    int32_t YangSynBuffer::getVideoSize(){
    	if(m_videoBuffer) return m_videoBuffer->m_size;
    		return 0;
    }

int32_t YangSynBuffer::playAudioFrame(int64_t pts){
	updateBaseTimestamp(pts);
	//int32_t minusClock=YangTime::getMilliTick()-m_startClock;
	int32_t minusClock=yang_get_milli_tick()-m_startClock;
	int32_t minusPts=pts-m_baseClock;
	int32_t minus=abs(minusPts-minusClock);
	if(abs(minus)<=m_maxMinus) return 0;
	return 1;
}
int32_t YangSynBuffer::playVideoFrame(int64_t pts){
	updateBaseTimestamp(pts);
	int32_t minusClock=yang_get_milli_tick()-m_startClock;
	int32_t minusPts=pts-m_baseClock;
	int32_t minus=abs(minusPts-minusClock);
	if(abs(minus)<=m_maxMinus) return 0;
	return 1;
}
void YangSynBuffer::updateBaseTimestamp(int64_t pts){
	if(m_baseClock==0) {
		setClock();
		m_baseClock=pts;
	}
}
void YangSynBuffer::setClock(){
	m_startClock=yang_get_milli_tick();
}

