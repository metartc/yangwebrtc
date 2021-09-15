#include <yangmeeting/include/YangMeetingPublish.h>
#include <yangmeeting/include/YangVrCapture.h>
#include "YangMeetingCaptureImpl.h"

YangMeetingPublish::YangMeetingPublish(YangMeetingContext *pini) {
	m_ini = pini;
	m_encoder = NULL;
	m_capture = NULL;
	isStartAudioCapture = 0, isStartVideoCapture = 0;
	isStartAudioEncoder = 0, isStartVideoEncoder = 0;
}

YangMeetingPublish::~YangMeetingPublish() {
	stopAll();
	m_ini = NULL;

	yang_delete(m_encoder);
	yang_delete(m_capture);
}

void YangMeetingPublish::stopAll(){
	if(m_capture) m_capture->stopAll();
	if(m_encoder) m_encoder->stopAll();
}
void YangMeetingPublish::sendKeyframe(){
	if(m_encoder) m_encoder->sendKeyframe();
}

void YangMeetingPublish::initCapture(){
	if (m_capture == NULL){
			//if(m_ini->sys.hasVr){
			//	m_capture=new YangVrCapture(m_ini);
			//}else{
				if(m_ini->sys.isMultCamera)
					m_capture=new YangVrCapture(m_ini);
				else
					m_capture = new YangMeetingCaptureImpl(m_ini);
			//}
		}
}

void YangMeetingPublish::startAudioCapture(YangPreProcess *pp) {
	if (isStartAudioCapture == 1)	return;
	initCapture();
	m_capture->initAudio(pp);
	m_capture->startAudioCapture();
	isStartAudioCapture = 1;

}
void YangMeetingPublish::startVideoCapture() {
	if (isStartVideoCapture == 1)	return;
	initCapture();

	m_capture->initVideo();
	m_capture->startVideoCapture();
	isStartVideoCapture = 1;

}
void YangMeetingPublish::setNetBuffer(YangMeetingNet *prr){
	yang_reindex(m_encoder->getOutAudioBuffer());
	yang_reindex(m_encoder->getOutVideoBuffer());
	m_encoder->getOutVideoBuffer()->resetIndex();
	prr->setInAudioList(m_encoder->getOutAudioBuffer());
	prr->setInVideoList(m_encoder->getOutVideoBuffer());
	prr->setInVideoMetaData(m_encoder->getOutVideoMetaData());
}
void YangMeetingPublish::initAudioEncoding(YangMeetingNet *prr) {

	if (isStartAudioEncoder == 1)		return;
	if (m_encoder == NULL)
		m_encoder = new YangMeetingEncoder(m_ini);
	m_encoder->initAudioEncoder();
	m_encoder->setInAudioBuffer(m_capture->getOutAudioBuffer());
	prr->setInAudioList(m_encoder->getOutAudioBuffer());
	isStartAudioEncoder = 1;
}
void YangMeetingPublish::setAec(YangAecBase *paec){
	if(m_capture) m_capture->setAec(paec);
}
void YangMeetingPublish::change(int32_t st){
	if(m_capture) m_capture->change(st);
}
void YangMeetingPublish::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf){
	if(m_capture) m_capture->setInAudioBuffer(pbuf);
}
void YangMeetingPublish::initVideoEncoding(YangMeetingNet *prr) {
	if (isStartVideoEncoder == 1)	return;
	if (m_encoder == NULL)
		m_encoder = new YangMeetingEncoder(m_ini);
	m_encoder->initVideoEncoder();
	m_encoder->setInVideoBuffer(m_capture->getOutVideoBuffer());
	prr->setInVideoList(m_encoder->getOutVideoBuffer());
	prr->setInVideoMetaData(m_encoder->getOutVideoMetaData());
	isStartVideoEncoder = 1;

}
void YangMeetingPublish::startAudioEncoding() {
	if (m_encoder)
		m_encoder->startAudioEncoder();
}
void YangMeetingPublish::startVideoEncoding() {
	if (m_encoder)
		m_encoder->startVideoEncoder();
}
void YangMeetingPublish::startAudioCaptureState() {
	if (m_capture)
		m_capture->startAudioCaptureState();
}
YangVideoBuffer* YangMeetingPublish::getPreVideoBuffer(){
	if (m_capture)  return	m_capture->getPreVideoBuffer();
	return NULL;
}

void YangMeetingPublish::startVideoCaptureState() {
	if (m_capture)
		m_capture->startVideoCaptureState();
}

void YangMeetingPublish::stopAudioCaptureState() {
	if (m_capture)
		m_capture->stopAudioCaptureState();
}
void YangMeetingPublish::stopVideoCaptureState() {
	if (m_capture)
		m_capture->stopVideoCaptureState();
}

void YangMeetingPublish::addVr(){
	if (m_capture) m_capture->addVr();
}
void YangMeetingPublish::delVr(){
	if (m_capture) m_capture->delVr();
}
