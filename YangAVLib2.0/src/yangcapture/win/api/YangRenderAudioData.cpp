/*
 * YangAudioDataManager.cpp
 *
 *  Created on: 2021年9月21日
 *      Author: yang
 */

#include <yangavutil/audio/YangAudioUtil.h>
#include <yangcapture/win/api/YangRenderAudioData.h>
#include <yangutil/sys/YangLog.h>
using namespace std;
YangRenderAudioData::YangRenderAudioData() {
	m_cacheLen = 1024 * 4 * 10;
	m_cache = new uint8_t[m_cacheLen];
	m_size = 0;
	m_pos = 0;

	m_renderSample = 48000;
	m_playSample = 48000;
	m_renderChannel = 2;
	m_playChannel = 2;
	m_bufLen = 0;
	m_sampleTrans = false;
	m_channelTrans = false;
	m_isInitTrans=false;
	n20ms=false;
	m_in_audioBuffer = NULL;
	m_in_audioBuffers=NULL;
	m_resBuf = NULL;
	m_resTmp = NULL;
	m_mixBuf=NULL;
	m_preProcess=NULL;
}

YangRenderAudioData::~YangRenderAudioData() {
	yang_deleteA(m_cache);
	yang_deleteA(m_resBuf);
	yang_deleteA(m_resTmp);
	yang_deleteA(m_mixBuf);
	m_in_audioBuffer = NULL;
	m_in_audioBuffers = NULL;
	m_preProcess=NULL;
    //m_aec=NULL;
}
void YangRenderAudioData::setAudioList(YangAudioPlayBuffer *pal) {
	m_in_audioBuffer = pal;
}
void YangRenderAudioData::setInAudioBuffer(std::vector<YangAudioPlayBuffer*> *pal) {
    if(m_in_audioBuffers) return;
    m_in_audioBuffers = pal;
    if(m_mixBuf==NULL){
    	m_mixBuf=new uint8_t[m_playSample*m_playChannel*2/50];
    }
}
void YangRenderAudioData::initRender(int psample, int pchannel) {
	m_renderSample = psample;
	m_renderChannel = pchannel;
	m_sampleTrans = (m_renderSample == m_playSample ? false : true);
	m_channelTrans = (m_renderChannel == m_playChannel ? false : true);
}
void YangRenderAudioData::initPlay(int psample, int pchannel) {
	m_playSample = psample;
	m_playChannel = pchannel;
	m_sampleTrans = (m_renderSample == m_playSample ? false : true);
	m_channelTrans = (m_renderChannel == m_playChannel ? false : true);
}
void YangRenderAudioData::initTrans(){
	yang_trace("\nm_sampleTrans====%d,m_channelTrans==%d\n",m_sampleTrans,m_channelTrans);
	if (m_sampleTrans || m_channelTrans) {
		if (m_resTmp == NULL)
			m_resTmp = new uint8_t[8192 * 2];
		if (m_resBuf == NULL) {
			m_resBuf = new uint8_t[8192 * 2];
			if (m_sampleTrans)		m_res.init(m_playChannel, m_playSample, m_renderSample);
		}
	}
}

int32_t YangRenderAudioData::hasData() {
    if (!m_in_audioBuffers)
        return 0;
    for (int32_t i = 0; i < (int) m_in_audioBuffers->size(); i++) {

        if (m_in_audioBuffers->at(i)->m_size > 0)
            return 1;
    }

    return 0;
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
		n20ms=true;
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
		    n20ms=false;
		    return m_mixBuf;
	}
	return NULL;
}
void YangRenderAudioData::setAudioData(YangFrame* frame){

			uint8_t *buf =getAudioData(frame);
			if(buf){
				if (m_sampleTrans) {
					uint32_t outlen = 6400;
					m_res.resample((short*)buf, frame->nb >> 1 / m_playChannel,	(short*) m_resTmp, &outlen);
					if (m_channelTrans)		MonoToStereo((short*) m_resTmp, (short*) m_resBuf, outlen);
					frame->payload = m_resBuf;
					frame->nb = outlen << 2;
				} else if (m_channelTrans) {
					if (m_channelTrans)	MonoToStereo((short*) buf, (short*) m_resBuf, frame->nb >> 1);
					frame->payload = m_resBuf;
					frame->nb = frame->nb << 1;
				} else {
					frame->payload=buf;
				}

			}

}
uint8_t* YangRenderAudioData::getRenderAudioData(int len) {
	if(!m_isInitTrans){
		initTrans();
		m_isInitTrans=true;
	}
	if ((m_pos + m_size + len) > m_cacheLen) {
		memmove(m_cache, m_cache + m_pos, m_size);
		m_pos = 0;
	}

	m_audioFrame.payload=NULL;
	m_audioFrame.nb=0;
	setAudioData(&m_audioFrame);
	if(m_audioFrame.payload&&m_audioFrame.nb>0) {
		if ((m_pos + m_size + m_audioFrame.nb) > m_cacheLen) {
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

