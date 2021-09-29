#include "YangStreamRtc.h"

#include <string.h>
#include <yangutil/yangavinfotype.h>
#include "yangutil/yang_unistd.h"
YangStreamRtc::YangStreamRtc(int32_t puid) :YangStreamHandle(puid) {
	m_rtc = YangRtcHandle::createRtcHandle();

}

YangStreamRtc::~YangStreamRtc() {
	yang_delete(m_rtc);

}
int32_t YangStreamRtc::isconnected(){
	if(m_rtc==NULL) return 0;
		return m_rtc->getState();
}
int32_t YangStreamRtc::getConnectState(){

	return m_rtc->getState()?Yang_Ok:ERROR_SOCKET;
}
int32_t YangStreamRtc::disConnectMediaServer(){

	return m_rtc->disConnectServer();
}
int32_t YangStreamRtc::connectMediaServer() {
	if(isconnected()) return Yang_Ok;

	m_rtc->setReceiveCallback(m_data);
	m_rtc->init(&m_conf);

	return reconnect();
}
void YangStreamRtc::setLocalMediaParam(YangAudioInfo* audioConf,YangVideoInfo* videoConf,YangRtcInfo* rtcInfo){
	if(m_rtc) m_rtc->setMediaParam(audioConf,videoConf,rtcInfo);
}
void YangStreamRtc::setRemoteMediaParamCallback(YangMediaConfigCallback* remoteCb){
	if(m_rtc) m_rtc->setRemoteMediaConfigCallback(remoteCb);
}
void YangStreamRtc::setSendRequestCallback(YangSendRequestCallback *cb){

	if(m_rtc) m_rtc->setSendRequestCallback(cb);
}

int32_t YangStreamRtc::publishAudioData(YangAudioStreamFrame *audioFrame) {
	if(m_rtc) return m_rtc->publishAudio(audioFrame);
	return Yang_Ok;
}

int32_t YangStreamRtc::publishVideoData(YangVideoStreamFrame* videoFrame) {
	if(m_rtc) return m_rtc->publishVideo(videoFrame);
	return Yang_Ok;
}

int32_t YangStreamRtc::reconnect() {

	return m_rtc->connectRtcServer();
}


int32_t YangStreamRtc::receiveData(int32_t *plen) {


	return Yang_Ok;
}
