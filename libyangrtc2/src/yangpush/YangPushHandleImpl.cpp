#include <yangpush/YangPushHandleImpl.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yang_unistd.h>

#include <yangutil/sys/YangUrl.h>

YangPushHandle::YangPushHandle() {

}
YangPushHandle::~YangPushHandle() {

}
//YangPushHandle* YangPushHandle::createPushHandle(bool initVideo,int pvideotype,YangVideoInfo* pvideo,YangContext *pcontext,
//		YangSysMessageI *pmessage) {
//	return new YangPushHandleImpl(initVideo,pvideotype,pvideo,pcontext, pmessage);
//}
YangPushHandleImpl::YangPushHandleImpl(bool phasAudio,bool initVideo,int pvideotype,YangVideoInfo* pvideo,YangContext *pcontext,
		YangSysMessageI *pmessage) {
	//m_cap = NULL;
	m_pub = NULL;
	m_videoState=pvideotype;
	m_screenInfo=pvideo;
	m_context = pcontext;
	m_message = pmessage;
	m_cap = new YangPushPublish(m_context);
	m_cap->setCaptureType(m_videoState);
	m_hasAudio = phasAudio;
	m_isInit=initVideo;
	if(m_isInit){
		if(m_videoState==Yang_VideoSrc_Camera){
			switchToCamera(true);
		}else{
			switchToScreen(true);
		}
	}

	//m_cap->startVideoCapture();


}

YangPushHandleImpl::~YangPushHandleImpl() {
	if (m_pub)
		m_pub->disConnectMediaServer();
	if (m_cap)
		m_cap->stopAll();
	yang_delete(m_pub);
	yang_delete(m_cap);

}
void YangPushHandleImpl::disconnect() {
	if (m_cap) {
		if(m_hasAudio) m_cap->stopAudioCaptureState();
		m_cap->stopVideoCaptureState();
		m_cap->stopScreenCaptureState();
	}
	stopPublish();
}
void YangPushHandleImpl::init() {
	if(m_isInit) return;
	if(m_videoState==Yang_VideoSrc_Camera){
			switchToCamera(true);
		}else{
			switchToScreen(true);
		}
	m_isInit=true;
}
void YangPushHandleImpl::startCapture() {

}

void YangPushHandleImpl::switchToCamera(bool pisinit) {
	m_videoState = Yang_VideoSrc_Camera;
	if(m_cap) m_cap->setCaptureType(m_videoState);
	if(m_cap) m_cap->setVideoInfo(&m_context->video);
	if(!pisinit) stopScreen();

	startCamera();
}

void YangPushHandleImpl::switchToScreen(bool pisinit) {
	m_videoState = Yang_VideoSrc_Screen;
	if(m_cap) m_cap->setCaptureType(m_videoState);
	if(m_cap) m_cap->setVideoInfo(m_screenInfo);

	if(!pisinit) stopCamera();

	startScreen();

}

void YangPushHandleImpl::setScreenInterval(int32_t pinterval) {
	if(m_cap) m_cap->setScreenInterval(pinterval);
}

void YangPushHandleImpl::setDrawmouse(bool isDraw) {
	if(m_cap) m_cap->setDrawmouse(isDraw);
}

void YangPushHandleImpl::stopPublish() {
	if (m_pub) {
		m_pub->disConnectMediaServer();
	}
	yang_stop(m_pub);
	yang_stop_thread(m_pub);
	yang_delete(m_pub);
	if(m_cap) m_cap->deleteVideoEncoding();
}
YangVideoBuffer* YangPushHandleImpl::getPreVideoBuffer() {
	if (m_videoState == Yang_VideoSrc_Camera) {
		if (m_cap)
			return m_cap->getPreVideoBuffer();
	} else if (m_videoState == Yang_VideoSrc_Screen) {
		if (m_cap)
			return m_cap->getScreenPreVideoBuffer();
	}
	return NULL;
}
void YangPushHandleImpl::setScreenVideoInfo(YangVideoInfo* pvideo){
	m_screenInfo=pvideo;
}
int YangPushHandleImpl::publish(string url, string localIp, int32_t localPort) {

	int err = Yang_Ok;
	if (yang_srs_url_parse((char*) url.c_str(), &m_url))
		return 1;

	stopPublish();
	yang_trace("\nnetType==%d,server=%s,port=%d,app=%s,stream=%s\n",
			m_url.netType, m_url.server.c_str(), m_url.port, m_url.app.c_str(),
			m_url.stream.c_str());

	if (m_pub == NULL) {
		m_pub = new YangRtcPublish(m_context);

	}


	//initList();
    if(m_hasAudio) {
        m_hasAudio=bool(m_cap->startAudioCapture()==Yang_Ok);
    }
	if (m_hasAudio) {

		m_cap->initAudioEncoding();
	}

	m_cap->initVideoEncoding();

	m_cap->setNetBuffer(m_pub);
	if (m_hasAudio)
		m_cap->startAudioEncoding();
	m_cap->startVideoEncoding();
	if ((err = m_pub->init(m_url.netType, m_url.server, localIp, localPort,
			1985, m_url.app, m_url.stream)) != Yang_Ok) {
		return yang_error_wrap(err, " connect server failure!");
	}

	m_pub->start();
	if (m_hasAudio)
		m_cap->startAudioCaptureState();
	if (m_videoState == Yang_VideoSrc_Camera)
		m_cap->startVideoCaptureState();
	else
		m_cap->startScreenCaptureState();
	return err;

}

void YangPushHandleImpl::startCamera() {
	if(m_cap) m_cap->startCamera();
}

void YangPushHandleImpl::startScreen() {
	if(m_cap) m_cap->startScreen();
}

void YangPushHandleImpl::stopCamera() {
	if(m_cap) m_cap->stopCamera();
}

void YangPushHandleImpl::stopScreen() {
	if(m_cap) m_cap->stopScreen();
}
