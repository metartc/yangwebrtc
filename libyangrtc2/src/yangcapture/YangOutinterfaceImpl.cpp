#include <yangcapture/YangOutinterfaceImpl.h>



YangOutInterfaceImpl::YangOutInterfaceImpl(YangContext *pcontext) {
	m_context = pcontext;
	m_width = 0;
	m_height = 0;
	//m_rgb = NULL;
	m_bufLen = 0;
	m_fd = 0;
	m_out_videoBuffer = nullptr;
	m_pre_videoBuffer = nullptr;
	isCapture = 0;
	m_loop = 0;
	m_state = 0;
    m_interval=40;
    m_isDrawmouse=false;
}

YangOutInterfaceImpl::~YangOutInterfaceImpl() {
    if(m_isStart){
        stop();
        yang_stop_thread(this);
    }
	m_context = NULL;
	yang_delete(m_pre_videoBuffer);
	yang_delete(m_out_videoBuffer);
}
int32_t YangOutInterfaceImpl::init() {

	if (m_out_videoBuffer == NULL)
		m_out_videoBuffer = new YangVideoBuffer(
				m_context->video.bitDepth == 8 ? 1 : 2);
	if (m_pre_videoBuffer == NULL)
		m_pre_videoBuffer = new YangVideoBuffer(
				m_context->video.bitDepth == 8 ? 1 : 2);
	return Yang_Ok;
}

void YangOutInterfaceImpl::setVideoCaptureStart() {
	m_state = 1;
}
void YangOutInterfaceImpl::setVideoCaptureStop() {
	m_state = 0;
}
int32_t YangOutInterfaceImpl::getVideoCaptureState(){
	return m_state;
}
YangVideoBuffer* YangOutInterfaceImpl::getPreVideoBuffer() {
	return m_pre_videoBuffer;
}
YangVideoBuffer* YangOutInterfaceImpl::getOutVideoBuffer() {
	return m_out_videoBuffer;
}
void YangOutInterfaceImpl::putBuffer(YangFrame *videoFrame) {
	m_out_videoBuffer->putVideo(videoFrame);
}

void YangOutInterfaceImpl::setDrawmouse(bool isDraw) {
	m_isDrawmouse=isDraw;

}

bool YangOutInterfaceImpl::getisDrawmouse() {
	return m_isDrawmouse;
}

void YangOutInterfaceImpl::stopLoop() {
	m_loop = 0;
}
void YangOutInterfaceImpl::startLoop() {

}
