#include <yangpush/YangPushPublish.h>



YangPushPublish::YangPushPublish(YangContext *pini) {
	m_ini = pini;
	m_encoder = NULL;
	m_capture = NULL;
	isStartAudioCapture = 0, isStartVideoCapture = 0;
	isStartAudioEncoder = 0, isStartVideoEncoder = 0;
}

YangPushPublish::~YangPushPublish() {
	stopAll();
	m_ini = NULL;

	yang_delete(m_encoder);
	yang_delete(m_capture);
}

void YangPushPublish::stopAll(){
	if(m_capture) m_capture->stopAll();
	if(m_encoder) m_encoder->stopAll();
}
void YangPushPublish::sendKeyframe(){
	if(m_encoder) m_encoder->sendKeyframe();
}



void YangPushPublish::startAudioCapture() {
	if (isStartAudioCapture == 1)	return;
	if (m_capture == NULL)		m_capture=new YangPushCapture(m_ini);
	m_capture->initAudio(NULL);
	m_capture->startAudioCapture();
	isStartAudioCapture = 1;

}
void YangPushPublish::startVideoCapture() {
	if (isStartVideoCapture == 1)	return;
	if (m_capture == NULL)		m_capture=new YangPushCapture(m_ini);
	m_capture->initVideo();
	m_capture->startVideoCapture();
	isStartVideoCapture = 1;

}
void YangPushPublish::setNetBuffer(YangRtcPublish *prr){
	yang_reindex(m_encoder->getOutAudioBuffer());
	yang_reindex(m_encoder->getOutVideoBuffer());
	m_encoder->getOutVideoBuffer()->resetIndex();
	prr->setInAudioList(m_encoder->getOutAudioBuffer());
	prr->setInVideoList(m_encoder->getOutVideoBuffer());
	prr->setInVideoMetaData(m_encoder->getOutVideoMetaData());
}
void YangPushPublish::initAudioEncoding() {

	if (isStartAudioEncoder == 1)		return;
	if (m_encoder == NULL)
		m_encoder = new YangPushEncoder(m_ini);
	m_encoder->initAudioEncoder();
	m_encoder->setInAudioBuffer(m_capture->getOutAudioBuffer());
	//prr->setInAudioList(m_encoder->getOutAudioBuffer());
	isStartAudioEncoder = 1;
}

void YangPushPublish::change(int32_t st){
	if(m_capture) m_capture->change(st);
}
void YangPushPublish::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf){
	if(m_capture) m_capture->setInAudioBuffer(pbuf);
}
void YangPushPublish::initVideoEncoding() {
	if (isStartVideoEncoder == 1)	return;
	if (m_encoder == NULL)
		m_encoder = new YangPushEncoder(m_ini);
	m_encoder->initVideoEncoder();
	m_encoder->setInVideoBuffer(m_capture->getOutVideoBuffer());
	isStartVideoEncoder = 1;
}
void YangPushPublish::startAudioEncoding() {
	if (m_encoder)
		m_encoder->startAudioEncoder();
}
void YangPushPublish::startVideoEncoding() {
	if (m_encoder)
		m_encoder->startVideoEncoder();
}
void YangPushPublish::startAudioCaptureState() {
	if (m_capture )
		m_capture->startAudioCaptureState();
}
YangVideoBuffer* YangPushPublish::getPreVideoBuffer(){
	if (m_capture )  return	m_capture->getPreVideoBuffer();
	return NULL;
}

void YangPushPublish::startVideoCaptureState() {
	if (m_capture )
		m_capture->startVideoCaptureState();
}

void YangPushPublish::stopAudioCaptureState() {
	if (m_capture )
		m_capture->stopAudioCaptureState();
}
void YangPushPublish::stopVideoCaptureState() {
	if (m_capture )
		m_capture->stopVideoCaptureState();
}

void YangPushPublish::addVr(){
	if (m_capture ) m_capture->addVr();
}
void YangPushPublish::delVr(){
	if (m_capture ) m_capture->delVr();
}
