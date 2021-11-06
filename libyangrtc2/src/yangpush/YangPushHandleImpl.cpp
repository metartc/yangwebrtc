#include <yangpush/YangPushHandleImpl.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yang_unistd.h>

#include <yangutil/sys/YangUrl.h>
#define Yang_Video_Camera 0
#define Yang_Video_Screen 1
YangPushHandle::YangPushHandle() {

}
YangPushHandle::~YangPushHandle() {

}
YangPushHandle* YangPushHandle::createPushHandle(YangContext *pcontext,
		YangSysMessageI *pmessage) {
	return new YangPushHandleImpl(pcontext, pmessage);
}
YangPushHandleImpl::YangPushHandleImpl(YangContext *pcontext,
		YangSysMessageI *pmessage) {
	m_cap = NULL;
	m_pub = NULL;
	m_context = pcontext;
	m_message = pmessage;
	m_cap = new YangPushPublish(m_context);
	m_hasAudio = true;
	m_cap->startVideoCapture();
	m_screenInfo=NULL;
	m_videoState = 0;
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
		m_cap->stopAudioCaptureState();
		m_cap->stopVideoCaptureState();
		m_cap->stopScreenCaptureState();
	}
	stopPublish();
}
void YangPushHandleImpl::init() {
	if (m_cap == NULL) {

	}
}
void YangPushHandleImpl::startCapture() {

}

void YangPushHandleImpl::switchToCamera() {
	m_videoState = Yang_Video_Camera;
	m_cap->setCaptureType(m_videoState);
	m_cap->setVideoInfo(&m_context->video);
}

void YangPushHandleImpl::switchToScreen() {
	m_videoState = Yang_Video_Screen;
	m_cap->setCaptureType(m_videoState);
	m_cap->setVideoInfo(m_screenInfo);
	m_cap->startScreenCapture();

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
	if (m_videoState == Yang_Video_Camera) {
		if (m_cap)
			return m_cap->getPreVideoBuffer();
	} else if (m_videoState == Yang_Video_Screen) {
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

	if (m_hasAudio) {
		m_cap->startAudioCapture();
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
	if (m_videoState == Yang_Video_Camera)
		m_cap->startVideoCaptureState();
	else
		m_cap->startScreenCaptureState();
	return err;

}

