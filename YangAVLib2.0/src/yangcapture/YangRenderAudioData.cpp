/*
 * YangAudioDataManager.cpp
 *
 *  Created on: 2021年9月21日
 *      Author: yang
 */

#include <yangavutil/audio/YangAudioUtil.h>
#include <yangcapture/YangRenderAudioData.h>
#include <yangutil/sys/YangLog.h>
using namespace std;

YangRenderAudioData::YangRenderAudioData() {
	m_cacheLen = 1024 * 4 * 10;
	m_cache = new uint8_t[m_cacheLen];
	m_size = 0;
	m_pos = 0;

	m_bufLen = 0;


    m_render20ms=false;
	m_in_audioBuffer = NULL;
	m_in_audioBuffers=NULL;
	m_mixBuf=NULL;
	m_preProcess=NULL;
    m_res.init(48000, 2, 48000, 2, 20);
}

YangRenderAudioData::~YangRenderAudioData() {
	yang_deleteA(m_cache);
	yang_deleteA(m_mixBuf);
	m_in_audioBuffer = NULL;
	m_in_audioBuffers = NULL;
	m_preProcess=NULL;

}

void YangRenderAudioData::setAudioList(YangAudioPlayBuffer *pal) {
	m_in_audioBuffer = pal;
}

void YangRenderAudioData::setInAudioBuffer(std::vector<YangAudioPlayBuffer*> *pal) {
    if(m_in_audioBuffers) return;
    m_in_audioBuffers = pal;
    if(m_mixBuf==NULL){
    	m_mixBuf=new uint8_t[m_res.getInSample()*m_res.getInChannel()*2/50];
    }
}

void YangRenderAudioData::initRender(int psample, int pchannel) {
	m_res.initOut(psample, pchannel);
}

void YangRenderAudioData::initPlay(int psample, int pchannel) {
	m_res.initIn(psample, pchannel);
}


bool YangRenderAudioData::hasData() {
    if (!m_in_audioBuffers)
        return 0;
    for (int32_t i = 0; i < (int) m_in_audioBuffers->size(); i++) {

        if (m_in_audioBuffers->at(i)->m_size > 0)
            return true;
    }

    return false;
}

uint8_t* YangRenderAudioData::getAecAudioData(){
	return m_mixBuf;
}

uint8_t* YangRenderAudioData::getAudioData(YangFrame* frame){
	if(m_in_audioBuffer&&m_in_audioBuffer->m_size>0){
				return m_in_audioBuffer->getAudios(frame);
	}
	if(m_bufLen>0){
		uint8_t* p=m_mixBuf+m_bufLen;
		frame->nb=m_bufLen;
		m_bufLen=0;
        m_render20ms=true;
		return p;
	}else	if(m_in_audioBuffers&&hasData()){
		   uint8_t *tmp = NULL;
		    for (size_t i = 0; i < m_in_audioBuffers->size(); i++) {
		        if (m_in_audioBuffers->at(i) && m_in_audioBuffers->at(i)->m_size > 0) {
		            //YangFrame* frame=m_in_audioBuffer->at(i)->getAudios();

		            tmp = m_in_audioBuffers->at(i)->getAudios(frame);
		            if (tmp) {
		               // if (m_preProcess)	m_preProcess->preprocess_run((short*) tmp);
		                if (i == 0) {
		                    memcpy(m_mixBuf, tmp, frame->nb);
		                } else {
		                    m_mix.yangMix1((short*)m_mixBuf, (short*)tmp, frame->nb, 128);
		                }
		            }
		            tmp = NULL;

		            // if (m_in_audioBuffer->at(i)->m_size > m_audioPlayCacheNum)                m_in_audioBuffer->at(i)->resetIndex();
		        }
		    }
		    frame->nb= frame->nb>>1;
		    m_bufLen=frame->nb;
            m_render20ms=false;
		    return m_mixBuf;
	}
	return NULL;
}

void YangRenderAudioData::setAudioData(YangFrame* frame){
			uint8_t *buf =getAudioData(frame);
			frame->payload=buf;
			m_res.resample(frame);
}

uint8_t* YangRenderAudioData::getRenderAudioData(int len) {
	if ((m_pos + m_size + len) >= m_cacheLen) {
		memmove(m_cache, m_cache + m_pos, m_size);
		m_pos = 0;
	}

	m_audioFrame.payload=NULL;
	m_audioFrame.nb=0;
	setAudioData(&m_audioFrame);
	if(m_audioFrame.payload&&m_audioFrame.nb>0) {
		if ((m_pos + m_size + m_audioFrame.nb) >= m_cacheLen) {
					memmove(m_cache, m_cache + m_pos, m_size);
					m_pos = 0;
		}
		memcpy(m_cache + m_pos + m_size, m_audioFrame.payload, m_audioFrame.nb);
		m_size += m_audioFrame.nb;
	}

	if (len > m_size)	return NULL;
	uint8_t *p = m_cache + m_pos;
	m_pos += len;
	m_size -= len;
	return p;

}

