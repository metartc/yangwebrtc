#include <yangavutil/audio/YangAudioFactory.h>
#include <yangmeeting/src/YangMeetingHandleImpl.h>
#include "../include/YangMeetingNet.h"

#include "unistd.h"
#include "pthread.h"
#include <iostream>

#include <vector>



#ifndef _WIN32
//#include "yangutil/sys/YangReadDecFactory.h"
//YangReadDec* g_dec=NULL;
#endif

YangMeetingHandle::YangMeetingHandle(){
	m_state=1;
	m_ini=NULL;
}
YangMeetingHandle::~YangMeetingHandle(){
	m_ini=NULL;
}
YangMeetingHandleImpl::YangMeetingHandleImpl(YangMeetingContext *p_ini):YangMeetingHandle() {
	m_ini = p_ini;
	m_state=1;
	m_playMain = NULL;
	m_pubMain = NULL;
	m_netMain = NULL;
	//m_messageMain=NULL;
	m_hasAec = 0;
	m_aec=NULL;
	m_message=NULL;
	m_cer=YangCertificate::createCertificate();
	m_playvideo=NULL;
#ifndef _WIN32
	//YangReadDecFactory rdf;
	//if(g_dec==NULL) g_dec=rdf.createReadDec();
	//if(!g_dec->isValid()||g_dec->getSystemId()!=692701){
		//exit(0);
	//}
#endif

}

YangMeetingHandleImpl::~YangMeetingHandleImpl() {
	m_ini = NULL;
//	m_vidoeShowHandle=NULL;
	stopAll();
	if(m_state==Yang_Ok) disconnectServer();


	yang_delete(m_playMain);
	yang_delete(m_pubMain);
	yang_delete(m_netMain);
	yang_delete(m_cer);
}
void YangMeetingHandleImpl::setMeetingMessage(YangMeetingMessage* pms){
	m_message=pms;
}
void YangMeetingHandleImpl::setPlayvideoHandle(YangVideoPlayHandle* playvideo){
	m_playvideo=playvideo;
}
void YangMeetingHandleImpl::delMessage(){
	m_message=NULL;
}
void YangMeetingHandleImpl::stopAll(){
	if(m_pubMain) m_pubMain->setInAudioBuffer(NULL);
		if(m_pubMain) m_pubMain->stopAll();
		if(m_playMain) m_playMain->stopAll();
		if(m_netMain) m_netMain->stopAll();
}

void YangMeetingHandleImpl::change(int32_t st){
	if(m_pubMain) m_pubMain->change(st);
}
int32_t YangMeetingHandleImpl::sendHeartbeat(){
    if(m_netMain) return m_netMain->sendHeartbeat();
    return Yang_Ok;
}

void YangMeetingHandleImpl::sendKeyframe(){
	if(m_pubMain) m_pubMain->sendKeyframe();
}
void YangMeetingHandleImpl::notifyMediaSource(int32_t puid){
	if(m_playMain) {
		map<int,YangAudioParam*>::iterator ita=m_ini->remoteAudio.find(puid);
		map<int,YangVideoParam*>::iterator itv=m_ini->remoteVideo.find(puid);
		YangAudioParam* audio=NULL;
		YangVideoParam* video=NULL;
		if(ita!=m_ini->remoteAudio.end()) audio=ita->second;
		if(itv!=m_ini->remoteVideo.end()) video=itv->second;
		if(audio||video)	m_playMain->notifyMediaSource(puid,audio,video);
	}
}

void YangMeetingHandleImpl::initAec(YangMeetingContext *p_ini) {
	if (m_aec == NULL) {
		YangAudioFactory yaf;
		m_aec = yaf.createAec();
		m_aec->m_echoPath=p_ini->audio.echoPath;
		if(p_ini->audio.usingMono){
			m_aec->init(320, 16000,1);
		}else{
			m_aec->init(1024, p_ini->audio.sample,p_ini->audio.channel);
		}
	}
}

int32_t YangMeetingHandleImpl::getConectState(){
	return m_state;
}

int32_t YangMeetingHandleImpl::startAudioCapture() {
	if (m_pubMain == NULL)	m_pubMain = new YangMeetingPublish(m_ini);
	YangAudioFactory yaf;
	m_pubMain->startAudioCapture(yaf.createPreProcess());	//startAudioCapture();
	if (m_ini->audio.hasAec&&!m_aec) 	{
		initAec(m_ini);
		m_pubMain->setAec(m_aec);
	}
	return Yang_Ok;
}
int32_t YangMeetingHandleImpl::startVideoCapture() {

	if (m_pubMain == NULL)
		m_pubMain = new YangMeetingPublish(m_ini);

	m_pubMain->startVideoCapture();
	return Yang_Ok;
}

int32_t YangMeetingHandleImpl::init(){

	startAudioCapture();
	startVideoCapture();
   // if(1) return 0;
	if (!m_netMain) 		{
		m_netMain = new YangMeetingNet(m_ini);
	}
	m_netMain->initNet();
	if(!m_playMain) m_playMain=new YangMeetingPlay(m_ini);
	m_playMain->init(m_netMain);
	m_pubMain->setInAudioBuffer(m_playMain->m_ydb->getOutAudioBuffer());

	return Yang_Ok;
}





int32_t YangMeetingHandleImpl::reconnectServer(){
	return m_netMain->reconnectServer();
}

int32_t YangMeetingHandleImpl::connectServer() {
	return m_netMain->connectServer(0, 0);

}
int32_t YangMeetingHandleImpl::connectPushServer(int32_t puid){
	return m_netMain->connectPushServer(puid);
}
int32_t YangMeetingHandleImpl::connectPlayServer(int32_t puid){
	return m_netMain->connectPlayServer(puid);
}
void YangMeetingHandleImpl::loginServer(int32_t loginState){
	if(loginState==Yang_Ok){
		m_state=Yang_Ok;
	}else{
		m_state=1;
	}
	m_netMain->netstateHandle(loginState);
}
void YangMeetingHandleImpl::login(int32_t puid){
	if(m_netMain) m_netMain->login(puid);
}

void YangMeetingHandleImpl::logout(int32_t puid){
	if(m_playMain) {
		m_playMain->unPlayAudio(puid);
		m_playMain->unPlayVideo(puid);
	}
	if(m_netMain) m_netMain->logout(puid);
}

int32_t YangMeetingHandleImpl::disconnectServer(){
	m_state=Yang_LeaveRoom;
	unPushAudio();
	unPushVideo();
	if(m_playMain) m_playMain->removeAllStream();
	if(m_netMain)  m_netMain->disconnectServer();
	return m_state;
}


int32_t YangMeetingHandleImpl::pushAudio() {
	start_audioEncoder();
	if(m_netMain->pubAudio())	return ERROR_RTMP_PubFailure;
	m_ini->user.micFlag=2;
	//startAudioCaptureState();
	return Yang_Ok;

}

int32_t YangMeetingHandleImpl::pushVideo() {
	start_videoEncoder();
	if(m_netMain->pubVideo()) 	return ERROR_RTMP_PubFailure;
	m_ini->user.camFlag=2;
	//startVideoCaptureState();
	return Yang_Ok;

}
int32_t YangMeetingHandleImpl::unPushAudio() {
	stopAudioCaptureState();
	if (m_netMain)	return	m_netMain->unPubAudio();
	return 1;

}
int32_t YangMeetingHandleImpl::unPushVideo() {
	stopVideoCaptureState();
	if (m_netMain)	return	m_netMain->unPubVideo();
	return 1;
}

void YangMeetingHandleImpl::start_audioEncoder() {
	m_pubMain->initAudioEncoding(m_netMain);
	m_pubMain->startAudioEncoding();
}


void YangMeetingHandleImpl::start_videoEncoder() {
	m_pubMain->initVideoEncoding(m_netMain);
	m_pubMain->startVideoEncoding();

}
void YangMeetingHandleImpl::startAudioDecoder() {
	if (m_playMain==NULL)		m_playMain = new YangMeetingPlay(m_ini);
		m_playMain->startAudioDecoder(m_netMain);

}
void YangMeetingHandleImpl::startVideoDecoder() {
	if (m_playMain == NULL)
		m_playMain = new YangMeetingPlay(m_ini);
	m_playMain->startVideoDecoder(m_netMain);
}


YangVideoBuffer* YangMeetingHandleImpl::getPreviewVideo() {
	if (m_pubMain)	return m_pubMain->getPreVideoBuffer();
	return NULL;
}
YangVideoBuffer* YangMeetingHandleImpl::getVideoBuffer(int32_t uid) {
	vector<YangVideoBuffer*> *des = m_playMain->m_ydb->getOutVideoBuffer();
	for (uint32_t  i = 0; i < des->size(); i++) {
		if (des->at(i)->m_uid == uid)
			return des->at(i);
	}
	return NULL;
}

vector<YangVideoBuffer*>* YangMeetingHandleImpl::getDecoderVideoBuffers(){
	if (m_playMain == NULL)		m_playMain = new YangMeetingPlay(m_ini);
	if(m_playMain->m_ydb) return m_playMain->m_ydb->getOutVideoBuffer();
	return NULL;
}

vector<YangUser*>* YangMeetingHandleImpl::getUserList() {
	if (m_netMain)
		return m_netMain->getUserList();
	return NULL;
}

int32_t YangMeetingHandleImpl::playAudio(int32_t puid) {
	startAudioDecoder();
	if(m_netMain&&m_netMain->playAudio(puid)) 		return ERROR_RTMP_PlayFailure;
	return Yang_Ok;
}
int32_t YangMeetingHandleImpl::playVideo(int32_t puid) {
	startVideoDecoder();
	if (m_netMain&&	m_netMain->playVideo(puid)) return ERROR_RTMP_PlayFailure;
	return Yang_Ok;
}
int32_t YangMeetingHandleImpl::unPlayAudio(int32_t puid) {
	if(m_playMain) m_playMain->unPlayAudio(puid);
	if (m_netMain&&m_netMain->unPlayAudio(puid))	return ERROR_RTMP_PlayFailure;
	return Yang_Ok;
}
int32_t YangMeetingHandleImpl::unPlayVideo(int32_t puid) {
	if(m_playvideo) m_playvideo->removePlayvideo(puid);
	if(m_playMain) m_playMain->unPlayVideo(puid);
	if (m_netMain&&m_netMain->unPlayVideo(puid))		return ERROR_RTMP_PlayFailure;

	return Yang_Ok;
}


void YangMeetingHandleImpl::addVr(){
	if (m_pubMain) m_pubMain->addVr();
}
void YangMeetingHandleImpl::delVr(){
	if (m_pubMain) m_pubMain->delVr();
}

void YangMeetingHandleImpl::startAudioCaptureState() {
	//printf("\nstartAudioCaptureState...................................");
	if (m_pubMain)
		m_pubMain->startAudioCaptureState();
}
void YangMeetingHandleImpl::startVideoCaptureState() {
	if (m_pubMain)		m_pubMain->startVideoCaptureState();
}
void YangMeetingHandleImpl::stopAudioCaptureState() {
	if (m_pubMain)
		m_pubMain->stopAudioCaptureState();
}
void YangMeetingHandleImpl::stopVideoCaptureState() {
	if (m_pubMain)
		m_pubMain->stopVideoCaptureState();
}
