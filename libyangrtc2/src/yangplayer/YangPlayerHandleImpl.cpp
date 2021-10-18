#include <yangutil/sys/YangLog.h>
#include <yangstream/YangStreamFactory.h>
#include <string>
#include <string.h>
#include <yangutil/yang_unistd.h>
#include "YangPlayerHandleImpl.h"
YangPlayerHandle* YangPlayerHandle::createPlayerHandle(YangContext* pcontext,YangSysMessageI* pmessage){
	return new YangPlayerHandleImpl(pcontext,pmessage);
}
YangPlayerHandleImpl::YangPlayerHandleImpl(YangContext* pcontext,YangSysMessageI* pmessage) {
	m_context=pcontext;
	m_message=pmessage;
	m_recv = NULL;
	m_play = NULL;
	m_rtcRecv=NULL;
	m_outVideoBuffer = NULL;
	m_outAudioBuffer = NULL;
	m_url.netType=0;
	m_url.port=1935;
}

YangPlayerHandleImpl::~YangPlayerHandleImpl() {
	if(m_rtcRecv) m_rtcRecv->disConnect();
	yang_delete(m_recv);
	yang_delete(m_play);
	yang_delete(m_rtcRecv);
	yang_delete(m_outVideoBuffer);
	yang_delete(m_outAudioBuffer);

}


//rtmp://host[:port]/app/stream
int32_t YangPlayerHandleImpl::parse(std::string purl) {

	char *url = (char*)purl.c_str();
	char *p = strstr(url, "://");
	if (!p) {
		yang_error("RTMP URL: No :// in url!");
		return 1;
	}
	int32_t len = (int) (p - url);
    if (len == 4 && memcmp(url, "rtmp", 4) == 0) {
		m_url.netType = Yang_Rtmp;
		m_url.port=1935;
    } else if (len == 6 && memcmp(url, "webrtc", 6) == 0) {
        m_url.netType = Yang_Webrtc;
        m_url.port=1985;
	} else {
		return 1;
	}
	p += 3;
	if (*p == 0) {
		yang_warn("No hostname in URL!");
		return 1;
	}

	char *end = p + strlen(p);
	char *col = strchr(p, ':');
	//schar *ques = strchr(p, '?');
	char *slash = strchr(p, '/');
	int32_t hostlen;
	if (slash)
		hostlen = slash - p;
	else
		hostlen = end - p;
	if (col && col - p < hostlen)
		hostlen = col - p;

	if (hostlen < 256) {
		m_url.server = std::string(p, hostlen);

	} else {
		yang_warn("Hostname exceeds 255 characters!");
	}

	p += hostlen;

	if (*p == ':') {
		uint32_t  p2;
		p++;
		p2 = atoi(p);
		if (p2 > 65535) {
			yang_warn("Invalid port number!");
		} else {
			m_url.port = p2;
		}
	}

	if (!slash) {
		yang_warn("No application or playpath in URL!");
		return 0;
	}
	p = slash + 1;
	//parse app
	char *slash2, *slash3 = NULL, *slash4 = NULL;
	int32_t applen, appnamelen;

	slash2 = strchr(p, '/');
	if (slash2)
		slash3 = strchr(slash2 + 1, '/');
	if (slash3)
		slash4 = strchr(slash3 + 1, '/');

	applen = end - p; /* ondemand, pass all parameters as app */
	appnamelen = applen; /* ondemand length */
	if (slash4)
		appnamelen = slash4 - p;
	else if (slash3)
		appnamelen = slash3 - p;
	else if (slash2)
		appnamelen = slash2 - p;

	applen = appnamelen;
	m_url.app = std::string(p, applen);
	p += appnamelen;

	//parse streamName
	if (*p == '/')
		p++;

	if (end - p) {
		m_url.stream = std::string(p, end - p);
	}
	//printf("\n************************webrtc://%s:%d/%s/%s\n",m_url.server.c_str(),m_url.port,m_url.app.c_str(),m_url.stream.c_str());
	return 0;
}
void YangPlayerHandleImpl::stopPlay(){
	if(m_rtcRecv) {
		m_rtcRecv->disConnect();
	}
	if(m_recv){
		m_recv->disConnect();
	}
	if(m_play) m_play->stopAll();
	yang_stop(m_rtcRecv);
	yang_stop_thread(m_rtcRecv);
	yang_delete(m_rtcRecv);

	//yang_delete(m_play);
    yang_stop(m_recv);
    yang_stop_thread(m_recv);
    yang_delete(m_recv);

    yang_delete(m_play);

}
int YangPlayerHandleImpl::play(string url,int32_t localPort) {
	m_url.app="";
	m_url.server="";
	m_url.stream="";
	m_url.port=0;
		if(parse(url)) return 1;

	stopPlay();
	printf("\nnetType==%d,server=%s,port=%d,app=%s,stream=%s\n",m_url.netType,m_url.server.c_str(),m_url.port,m_url.app.c_str(),m_url.stream.c_str());
	m_context->sys.transType=m_url.netType;
	if(m_context->streams.m_playBuffer) m_context->streams.m_playBuffer->setTranstype(m_url.netType);
    if(m_url.netType ==Yang_Webrtc){

        return playRtc(0,m_url.server,localPort,m_url.server,1985,m_url.app,m_url.stream);

    }
    return 1;
    /**
	if (!m_play)	{
		m_play = new YangPlayerBase();

        m_context->audio.sample=44100;
        m_context->audio.channel=2;
        m_context->audio.audioDecoderType=0;
        m_context->audio.usingMono=0;
        m_context->audio.aIndex=-1;
        m_play->init(m_context);
	}
	initList();
	m_play->startAudioDecoder(m_outAudioBuffer);
	m_play->startVideoDecoder(m_outVideoBuffer);
	m_play->startAudioPlay(m_context);
	//m_play->startVideoPlay();


	if (!m_recv) {

		m_recv = new YangPlayReceive(m_context);	//sf.createStreamBase(m_url.netType,0,m_context);
		m_recv->setBuffer(m_outAudioBuffer, m_outVideoBuffer);
	}
	if(m_recv->init(m_url.netType, m_url.server, m_url.port, m_url.stream)){
		printf("\n connect server failure!");
		return 1;
	}

	m_recv->start();
	return Yang_Ok;
	**/

}

int32_t YangPlayerHandleImpl::playRtc(int32_t puid,std::string localIp,int32_t localPort, std::string server, int32_t pport,std::string app,std::string stream){

	stopPlay();
	if (!m_play)	{
		m_play = new YangPlayerBase();

		m_context->audio.sample=48000;
		m_context->audio.channel=2;
		m_context->audio.audioDecoderType=3;
		m_context->audio.usingMono=0;
        m_context->audio.aIndex=-1;
		m_play->init(m_context);
	}
	initList();
	m_play->startAudioDecoder(m_outAudioBuffer);
	m_play->startVideoDecoder(m_outVideoBuffer);

	m_play->startAudioPlay(m_context);


	if(m_rtcRecv==NULL) {
		m_rtcRecv=new YangRtcReceive(m_context,m_message);
		m_rtcRecv->setBuffer(m_outAudioBuffer, m_outVideoBuffer);
		m_rtcRecv->init(puid,localIp,localPort,server,pport,app,stream);
	}

	 m_rtcRecv->start();

	 return Yang_Ok;
}

YangVideoBuffer* YangPlayerHandleImpl::getVideoBuffer(){
	if(m_play) return m_play->m_ydb->getOutVideoBuffer();
	return NULL;
}

void YangPlayerHandleImpl::initList() {
	if (m_outAudioBuffer == NULL) {
		m_outAudioBuffer = new YangAudioEncoderBuffer(10);
	}
	if (m_outVideoBuffer == NULL)
		m_outVideoBuffer = new YangVideoDecoderBuffer();

}
