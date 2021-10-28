/*
 * YangRtmpBase.cpp
 *
 *  Created on: 2019年10月13日
 *      Author: yang
 */

#include "yangutil/yang_unistd.h"
#include <stdio.h>
#include <malloc.h>

#ifndef _WIN32
#include "sys/socket.h"
#include <arpa/inet.h>
#endif

#include "pthread.h"
#include "stdint.h"
#include "fcntl.h"
#include <memory.h>
#include <yangmeeting/YangMeetingNet.h>


YangMeetingNet::YangMeetingNet(YangMeetingContext *pcontext) {
	m_context = pcontext;
	m_connState = Yang_Ok;
	m_publish_media = NULL;
	m_receive_media = NULL;
	m_dataConn = NULL;
	m_userData = NULL;
	m_out_al = NULL;
	m_out_vl = NULL;
	m_userList = new vector<YangUser*>();
}

YangMeetingNet::~YangMeetingNet() {
	stopAll();
	closeConnect();
	if (m_receive_media && m_receive_media->m_isStart) {
		m_receive_media->stop();
		while (m_receive_media->m_isStart) {
			yang_usleep(1000);
		}
	}

	if (m_publish_media && m_publish_media->m_isStart) {
		m_publish_media->stop();
		while (m_publish_media->m_isStart) {
			yang_usleep(1000);
		}
	}

	if (m_dataConn&&m_dataConn->m_isStart) {
		m_dataConn->stop();
		while (m_dataConn->m_isStart) {
			yang_usleep(1000);
		}

	}

	yang_delete(m_dataConn);
	yang_delete(m_userData);
	yang_delete(m_receive_media);
	yang_delete(m_publish_media);

	for (int32_t i = 0; i < (int) m_userList->size(); i++) {
		yang_delete(m_userList->at(i));
	}
	m_userList->clear();
	yang_delete(m_userList);
	yang_delete(m_out_al);
	yang_delete(m_out_vl);
	m_context = NULL;
}
void YangMeetingNet::netstateHandle(int32_t netstate){
	if(netstate==Yang_Ok){
		m_dataConn->sendRoomInfoUser();
	}
}
int32_t YangMeetingNet::connectServer(int32_t isPubAudio, int32_t isPubVideo) {
	int32_t ret = m_dataConn->connectServer();
	if (ret)	return ret;

	if (!m_dataConn->m_isStart) {
		m_dataConn->start();
		while (!m_dataConn->m_isStart) {
			yang_usleep(100);
		}
	}
	//initMediaNet();
	return m_dataConn->login(m_userData->getLoginstring());

}
int32_t YangMeetingNet::sendHeartbeat(){
    if(m_dataConn) return m_dataConn->sendHeartbeat();
    return Yang_Ok;
}

int32_t YangMeetingNet::connectPushServer(int32_t puid){
	return m_publish_media->connectServer(puid);
}
int32_t YangMeetingNet::connectPlayServer(int32_t puid){
	return m_receive_media->connectServer(puid);
}
int32_t YangMeetingNet::reconnectServer() {
	return Yang_Ok;
}

void YangMeetingNet::initNet() {
	initList();
	if (m_userData == NULL) {
		m_userData = new YangUserDataBase();
		m_userData->m_context = m_context;
		m_userData->m_userList = m_userList;
		m_userData->m_meeting = &m_context->meeting;
		m_userData->m_user = &m_context->user;
	}
	if (m_dataConn == NULL) {
		m_dataConn = new YangCommandHandle();
		m_dataConn->m_context=m_context;
		m_dataConn->m_userData = m_userData;
		m_dataConn->init(m_context->sys.dataServerIP, m_context->sys.dataPort);
	}
	initMediaNet();
}
void YangMeetingNet::stopAll() {
	if(m_dataConn) m_dataConn->stop();
	if (m_receive_media)
		m_receive_media->stop();
	if (m_publish_media)
		m_publish_media->stop();
}
void YangMeetingNet::closeConnect() {
	m_userData->clearData();
	m_receive_media->disConnectAll();
	m_publish_media->disConnectMediaServer();
	m_dataConn->disConnectServer();
}
int32_t YangMeetingNet::disconnectServer() {
	//stopAll();
	closeConnect();
	return Yang_Ok;
}
void YangMeetingNet::setNetworkState(int32_t pst) {
	m_connState = pst;
	if (m_connState != Yang_Ok) {
		closeConnect();
		if (m_context->receive)
			m_context->receive->receiveError(m_connState);
	}
}
void YangMeetingNet::startPublishAudio() {
	if (m_publish_media->m_isStart == 1)
		return;
	m_publish_media->start();
	while (!m_publish_media->isPublished) {
		yang_usleep(1000);
	}
}
void YangMeetingNet::startPublishVideo() {
	if (m_publish_media->m_isStart == 1)
		return;
	m_publish_media->start();
	while (!m_publish_media->isPublished) {
		yang_usleep(1000);
	}
}


void YangMeetingNet::initMediaNet() {
	if (!m_receive_media) {
		m_receive_media = new YangReceive(m_context);
		m_receive_media->setOutAudioBuffer(m_out_al);
		m_receive_media->setOutVideoBuffer(m_out_vl);
		if(m_context->sys.transType!=Yang_Webrtc){
			m_receive_media->start();
			while (!m_receive_media->isReceived) {
				yang_usleep(1000);
			}
		}
	}

	if (!m_publish_media) {
		m_publish_media = new YangPublish(m_context);
		//m_publish_media->initPublish();
		//m_publish_media->setInAudioList(m_in_al)

	}

}
int32_t YangMeetingNet::getIsAudioPublished() {
	return m_publish_media->m_isPubAudio;
}
int32_t YangMeetingNet::getIsVideoPublished() {
	return m_publish_media->m_isPubAudio;
}

int32_t YangMeetingNet::login(int32_t puid){
	//m_userData->loginHandle(,1);
	return Yang_Ok;
}

int32_t YangMeetingNet::logout(int32_t puid){
	unPlayAudio(puid);
	unPlayVideo(puid);
	m_userData->logout(puid);
	return Yang_Ok;
}

int32_t YangMeetingNet::pubVideo() {
	startPublishVideo();
	//if (m_publish_media->m_isPubVideo)		return Yang_Ok;
	int32_t ret=Yang_Ok;
	ret=m_publish_media->pubVideoStream(m_userData->m_user->userId);
	if (ret)	return ret;
	m_publish_media->m_isPubVideo = 1;
	return m_dataConn->pubVideo();
}
int32_t YangMeetingNet::pubAudio() {
	//if (m_publish_media && m_shareRtmp && m_netState) return ERROR_RTMP_ConnectFailure;
	startPublishAudio();
	//if (m_publish_media->m_isPubAudio)
		//return Yang_Ok;
	int32_t ret=m_publish_media->pubAudioStream(m_userData->m_user->userId);
	if (ret)	return ret;
	m_publish_media->m_isPubAudio = 1;
	return m_dataConn->pubAudio();
}
int32_t YangMeetingNet::unPubAudio() {
	if (m_publish_media->unPublishAudioStream(m_userData->m_user->userId))
		return ERROR_RTMP_UnPubFailure;
	return m_dataConn->unPubAudio();
}
int32_t YangMeetingNet::unPubVideo() {
	//if (m_publish_media && m_shareRtmp && m_netState) return ERROR_RTMP_ConnectFailure;
	if (m_publish_media->unPublishVideoStream(m_userData->m_user->userId))
		return ERROR_RTMP_UnPubFailure;
	return m_dataConn->unPubVideo();

}
int32_t YangMeetingNet::playAudio(int32_t puid) {
	int32_t ret=m_receive_media->playAudio(puid);
	if(ret) return ret;
	return m_userData->playAudio(puid);
}
int32_t YangMeetingNet::playVideo(int32_t puid) {
	int32_t ret=m_receive_media->playVideo(puid);
	if(ret) return ret;
	return m_userData->playVideo(puid);
}

int32_t YangMeetingNet::unPlayAudio(int32_t puid) {
	int32_t ret=m_receive_media->unPlayAudio(puid);
	if(ret) return ret;
	return m_userData->unPlayAudio(puid);
}
int32_t YangMeetingNet::unPlayVideo(int32_t puid) {
	int32_t ret=m_receive_media->unPlayVideo(puid);
	if(ret) return ret;
	return m_userData->unPlayVideo(puid);
}

vector<YangUser*>* YangMeetingNet::getUserList() {
	return m_userList;
}
void YangMeetingNet::initList() {
	if (m_out_al == NULL)
		m_out_al = new YangAudioEncoderBuffer(m_context->audio.audioCacheNum);
	if (m_out_vl == NULL)
		m_out_vl = new YangVideoDecoderBuffer();
}
YangAudioEncoderBuffer* YangMeetingNet::getOutAudioList() {
	return m_out_al;
}
YangVideoDecoderBuffer* YangMeetingNet::getOutVideoList() {
	return m_out_vl;
}

void YangMeetingNet::setInAudioList(YangAudioEncoderBuffer *pal) {
	if (m_publish_media != NULL)
		m_publish_media->setInAudioList(pal);

}
void YangMeetingNet::setInVideoList(YangVideoEncoderBuffer *pvl) {
	if (m_publish_media != NULL)
		m_publish_media->setInVideoList(pvl);
}

void YangMeetingNet::setInVideoMetaData(YangVideoMeta *pvmd) {
	if (m_publish_media != NULL)
		m_publish_media->setInVideoMetaData(pvmd);

}
