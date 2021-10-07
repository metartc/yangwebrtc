/*
 * YangStreamManager.cpp
 *
 *  Created on: 2021年10月2日
 *      Author: yang
 */

#include <yangstream/YangStreamManager.h>

YangStreamManager::YangStreamManager() {
	m_playBuffers=NULL;
	m_playBuffer=NULL;
	m_sendPli=NULL;
	m_mediaConfig_dec=NULL;
	m_mediaConfig_render=NULL;
}

YangStreamManager::~YangStreamManager() {
	yang_delete(m_playBuffer);
	if(m_playBuffers){
		for(size_t i=0;i<m_playBuffers->size();i++){
			yang_delete(m_playBuffers->at(i));
		}
		m_playBuffers->clear();
	}
	m_sendPli=NULL;
	m_mediaConfig_dec=NULL;
	m_mediaConfig_render=NULL;
}

void YangStreamManager::setSendRequestCallback(YangSendRequestCallback* pli){
	m_sendPli=pli;
}
	void YangStreamManager::setDecoderMediaConfigCallback(YangMediaConfigCallback* dec){
		m_mediaConfig_dec=dec;
	}
	void YangStreamManager::setRenderMediaConfigCallback(YangMediaConfigCallback* render){
		m_mediaConfig_render=render;
	}
void YangStreamManager::setMediaConfig(int32_t puid,YangAudioParam* audio,YangVideoParam* video){
	if(m_mediaConfig_dec) m_mediaConfig_dec->setMediaConfig(puid,audio,video);
	if(m_mediaConfig_render) m_mediaConfig_render->setMediaConfig(puid,audio,video);
}
	void YangStreamManager::sendRequest(int32_t puid,uint32_t ssrc,YangRequestType req){
		if(m_sendPli) m_sendPli->sendRequest(puid,ssrc,req);
	}
int YangStreamManager::getIndex(int puid){
	if(m_playBuffers){
		for(size_t i=0;i<m_playBuffers->size();i++){
			if(m_playBuffers->at(i)->m_uid==puid) return i;
		}
	}
	return -1;
}

YangSynBuffer* YangStreamManager::getSynBuffer(int puid){
	int ind=getIndex(puid);
	if(ind==-1) return NULL;
	return m_playBuffers->at(ind);
}
