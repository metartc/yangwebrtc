
#include <yangcapture/win/api/YangCaptureAudioData.h>
#include <yangutil/sys/YangLog.h>
#include <yangavutil/audio/YangAudioUtil.h>
YangCaptureAudioData::YangCaptureAudioData() {
	m_cacheLen = 1024 * 4 * 4;
	m_cache = new uint8_t[m_cacheLen];
	m_size=0;
	m_pos=0;
	m_inSample = 48000;
	m_outSample = 48000;
	m_inChannel = 2;
	m_outChannel = 2;
	m_resBuf = NULL;
	m_resTmp = NULL;
	m_sampleTrans = false;
	m_channelTrans = false;
	m_isInitTrans=false;
	n20msframes_in=m_inSample/50;
	n20msframes_out=m_outSample/50;
	n20mslen_in=n20msframes_in*2*m_inChannel;
	n20mslen_out=n20msframes_out*2*m_outChannel;
	m_cb=NULL;

}

YangCaptureAudioData::~YangCaptureAudioData() {
	yang_deleteA(m_cache);
	yang_deleteA(m_resBuf);
	yang_deleteA(m_resTmp);
	m_cb=NULL;
}



void YangCaptureAudioData::initIn(int psample, int pchannel) {
	m_inSample = psample;
	m_inChannel = pchannel;
	m_sampleTrans = (m_inSample == m_outSample ? false : true);
	m_channelTrans = (m_inChannel == m_outChannel ? false : true);
	n20msframes_in=m_inSample/50;
	n20mslen_in=n20msframes_in*2*m_inChannel;

}
void YangCaptureAudioData::initOut(int psample, int pchannel) {
	m_outSample = psample;
	m_outChannel = pchannel;
	m_sampleTrans = (m_inSample == m_outSample ? false : true);
	m_channelTrans = (m_inChannel == m_outChannel ? false : true);

	n20msframes_out=m_outSample/50;
	n20mslen_out=n20msframes_out*2*m_outChannel;
}
void YangCaptureAudioData::caputure(YangFrame* audioFrame){

		if ((m_pos + m_size + audioFrame->nb) > m_cacheLen) {
			memmove(m_cache, m_cache + m_pos, m_size);
			m_pos = 0;
		}
		//setAudioData(&m_audioFrame);
		if(audioFrame->payload&&audioFrame->nb>0) {
			if(m_pos+n20mslen_in>m_cacheLen)	memmove(m_cache,m_cache+m_pos,m_size);
			memcpy(m_cache + m_pos + m_size, audioFrame->payload, audioFrame->nb);
			m_size +=  audioFrame->nb;
		}

		if(m_size>=n20mslen_in){

			captureData();

			m_pos+=n20mslen_in;
			m_size-=n20mslen_in;
		}

}
void YangCaptureAudioData::captureData(){
	if(!m_isInitTrans){
			initTrans();
			m_isInitTrans=true;
		}
	uint8_t* buf=m_cache+m_pos;
	uint8_t* res=NULL;
	if (m_sampleTrans) {
						uint32_t outlen = 6400;
						m_res.resample((short*)buf, n20msframes_in,	(short*) m_resTmp, &outlen);
						if (m_channelTrans)		MonoToStereo((short*) m_resTmp, (short*) m_resBuf, outlen);
						m_audioFrame.payload=m_resBuf;
	} else if (m_channelTrans) {
						if (m_channelTrans)	MonoToStereo((short*) buf, (short*) m_resBuf, n20msframes_in);
						m_audioFrame.payload=m_resBuf;

	} else {
		m_audioFrame.payload=buf;
	}
	m_audioFrame.nb=n20mslen_out;
	if(m_cb) m_cb->caputureAudioData(&m_audioFrame);

}
void YangCaptureAudioData::initTrans(){
	yang_trace("\nm_sampleTrans====%d,m_channelTrans==%d\n",m_sampleTrans,m_channelTrans);
	if (m_sampleTrans || m_channelTrans) {
		if (m_resTmp == NULL)
			m_resTmp = new uint8_t[8192 * 2];
		if (m_resBuf == NULL) {
			m_resBuf = new uint8_t[8192 * 2];
			if (m_sampleTrans)		m_res.init(m_inChannel, m_inSample, m_outSample);
		}
	}
}
