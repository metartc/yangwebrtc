/*
 * YangPublish.cpp
 *
 *  Created on: 2019年9月11日
 *      Author: yang
 */
#include <stdio.h>
#include <yangutil/yang_unistd.h>
#include <yangmeeting/YangPublish.h>
#include <yangmeeting/YangMeetingMessageI.h>
#include <yangstream/YangStreamCapture.h>
#include "yangstream/YangStreamFactory.h"
#include <yangstream/YangStreamType.h>


YangPublish::YangPublish(YangMeetingContext *pcontext) {
	m_context = pcontext;
	m_context->streams.setSendRequestCallback(this);
	m_in_videoBuffer = NULL;
	m_in_audioBuffer = NULL;
	m_isStart = 0;
	m_isConvert = 0;
	m_vmd = NULL;
	m_audioEncoderType = m_context->audio.audioEncoderType;
	m_netState = 1;
	m_isInit = 0;
	isPublished = 0;
	m_isPubVideo = 0, m_isPubAudio = 0;
	m_transType=Yang_Webrtc;
	notifyState=0;
}

YangPublish::~YangPublish() {
	if (m_isConvert) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	m_context = NULL;
	m_in_videoBuffer = NULL;
	m_in_audioBuffer = NULL;
	m_vmd = NULL;
}
int32_t YangPublish::pubAudioStream(int32_t puid) {
	int32_t ret = Yang_Ok;
	ret = connectMediaServer();
	if (ret)
		return ret;
	m_isPubAudio = 1;
	if(m_pushs.back()->isconnected()) yang_post_message(YangM_Sys_StartAudioCapture,0,NULL);
	return Yang_Ok;
}
void YangPublish::sendRequest(int32_t puid,uint32_t ssrc,YangRequestType req){
	//	YangM_Sys_StartAudioCapture,	YangM_Sys_StartVideoCapture,
	//printf("\nsendRequest........req=%d,uid=%d.....",req,puid);
	if(req==Yang_Req_Sendkeyframe) yang_post_message(YangM_Sys_SendKeyframe,0,NULL);
	if(req==Yang_Req_Connected) {
		//printf("\nsendRequest........audio=%d,video=%d.....",m_isPubAudio,m_isPubVideo);
		if(m_isPubAudio) yang_post_message(YangM_Sys_StartAudioCapture,0,NULL);
		if(m_isPubVideo) yang_post_message(YangM_Sys_StartVideoCapture,0,NULL);
	}
}
int32_t YangPublish::connectServer(int32_t puid){
	return m_pushs.back()->connectServer();
}
int32_t YangPublish::pubVideoStream(int32_t puid) {
	int32_t ret = Yang_Ok;
	ret = connectMediaServer();
	if (ret)
		return ret;
	m_isPubVideo = 1;
	if(m_pushs.back()->isconnected()) yang_post_message(YangM_Sys_StartVideoCapture,0,NULL);
	return ret;
}
int32_t YangPublish::unPublishAudioStream(int32_t puid) {
	m_isPubAudio = 0;
	if (!m_isPubVideo)
		disConnectMediaServer();
	return Yang_Ok;
}
int32_t YangPublish::unPublishVideoStream(int32_t puid) {
	m_isPubVideo = 0;
	if (!m_isPubAudio)
		disConnectMediaServer();
	return Yang_Ok;
}

int32_t YangPublish::reconnectMediaServer() {
	return m_pushs.back()->reconnect();

}
int32_t YangPublish::connectMediaServer() {
	if(m_pushs.size()>0) return Yang_Ok;
	YangStreamFactory sf;
	int32_t ret = 0;
	YangStreamConfig streamConf;
	streamConf.app="live";
		streamConf.streamOptType=Yang_Stream_Publish;
		streamConf.app="live";
		streamConf.uid=m_context->user.userId;

		streamConf.localIp="127.0.0.1";
		streamConf.localPort= m_context->sys.rtcLocalPort;

	if (m_pushs.size() == 0) {
			char s[128] = { 0 };
            m_pushs.push_back(sf.createStreamHandle(m_context->sys.transType,	m_context->user.userId,m_context));
			if (m_context->sys.transType==Yang_Rtmp){
					sprintf(s, "user%d", m_context->user.userId);
					streamConf.serverIp=m_context->sys.rtmpServerIP;
					streamConf.serverPort=m_context->sys.rtmpPort;
			}else if(m_context->sys.transType==Yang_Srt){
				sprintf(s, "%d.%d", m_context->meeting.meetingId, m_context->user.userId);
				streamConf.serverIp=m_context->sys.srtServerIP;
				streamConf.serverPort=m_context->sys.srtPort;
			}else if(m_context->sys.transType==Yang_Webrtc){
				sprintf(s, "user%d", m_context->user.userId);
				streamConf.serverIp=m_context->sys.rtcServerIP;
				streamConf.serverPort=m_context->sys.rtcPort;

			}

			streamConf.stream=s;

			m_pushs.back()->init(&streamConf);

		}


	if (m_pushs.back()->isconnected())		return Yang_Ok;
	ret = m_pushs.back()->connectServer();
	//printf("\n...........connectServer..ret===%d",ret);
	if (ret)		return ret;
	m_pushs.back()->m_streamInit = 1;
	yang_reindex(m_in_audioBuffer);
	yang_reindex(m_in_videoBuffer);
	return Yang_Ok;
}
int32_t YangPublish::disConnectMediaServer() {
	if (m_pushs.size() > 0) {
		m_pushs.back()->m_streamInit = 0;
		delete m_pushs.back();
		m_pushs.back() = NULL;
		m_pushs.clear();
	}
	return Yang_Ok;
}
void YangPublish::stop() {
	m_isConvert = 0;
}

void YangPublish::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}

void YangPublish::setInAudioList(YangAudioEncoderBuffer *pbuf) {
	m_in_audioBuffer = pbuf;
}
void YangPublish::setInVideoList(YangVideoEncoderBuffer *pbuf) {
	m_in_videoBuffer = pbuf;
}
void YangPublish::setInVideoMetaData(YangVideoMeta *pvmd) {
	m_vmd = pvmd;
}

void YangPublish::startLoop() {

	isPublished = 0;
	m_isConvert = 1;
	YangStreamCapture data,metaFrame;
	//YangVideoStreamFrame vdata,metaFrame;
	//YangAudioStreamFrame adata;
	YangFrame audioFrame;
	YangFrame videoFrame;
	data.initAudio(m_context->sys.transType,m_context->audio.sample, m_context->audio.channel,
			(YangAudioEncDecType) m_context->audio.audioEncoderType);
	data.initVideo(m_context->sys.transType);
	metaFrame.initVideo(m_context->sys.transType);
	YangVideoEncDecType videoType =	(YangVideoEncDecType) m_context->video.videoEncoderType;

	size_t i = 0;
	int32_t ret = Yang_Ok;
	isPublished = 1;
	notifyState=1;
	while (m_isConvert == 1) {
		if ((!m_isPubVideo && !m_isPubAudio) || (m_pushs.size() == 0)) {
			yang_usleep(20000);
			continue;
		}
		if (!m_in_audioBuffer && m_in_videoBuffer
				&& m_in_videoBuffer->size() == 0) {
			yang_usleep(1000);
			continue;
		}
		if (!m_in_videoBuffer && m_in_audioBuffer
				&& m_in_audioBuffer->size() == 0) {
			yang_usleep(2000);
			continue;
		}
		if ((m_in_videoBuffer && m_in_videoBuffer->size() == 0)
				&& (m_in_audioBuffer && m_in_audioBuffer->size() == 0)) {
			yang_usleep(2000);
			continue;
		}
		if (m_pushs.size() == 0 || !m_pushs.back()->m_streamInit) {
			yang_usleep(500);
			continue;
		}
		if(m_pushs.back()->isconnected()){
			if(notifyState&&m_transType<Yang_Webrtc){
						sendRequest(0,0,Yang_Req_Connected);
						notifyState=0;
					}

		}else{
			yang_usleep(500);
			continue;
		}
		/**if(ret){
		 ret=reconnectMediaServer();
		 if(!m_isConvert) break;
		 sleep(3);
		 if(!ret){
		 yang_reindex(m_in_audioBuffer);
		 yang_reindex(m_in_videoBuffer);
		 }
		 continue;
		 }**/

		if (m_isPubAudio && m_in_audioBuffer && m_in_audioBuffer->size() > 0) {
			if (m_in_audioBuffer->size() > 100)
				printf("encode cache big..%d\n", m_in_audioBuffer->size());

			audioFrame.payload = m_in_audioBuffer->getAudioRef(&audioFrame);
			data.setAudioData(&audioFrame);
			//printf("ad%d-%d,",audioBufLen,adata.getAudioLen());
			for (i = 0; i < m_pushs.size(); i++) {
				ret = m_pushs.at(i)->publishAudioData(&data);
				if (ret&&!m_pushs.at(i)->isconnected()) {
					m_pushs.back()->m_streamInit = 0;
					m_pushs.back()->disConnectServer();
					yang_post_message(YangM_Sys_PushMediaServerError,m_pushs.back()->m_uid,NULL);
				}
			}
		}

		if (!ret && m_isPubVideo && m_in_videoBuffer
				&& m_in_videoBuffer->size() > 0) {
			videoFrame.payload = m_in_videoBuffer->getEVideoRef(&videoFrame);


			data.setVideoData(&videoFrame, videoType);

			for (i = 0; i < m_pushs.size(); i++) {
				if (videoFrame.frametype == YANG_Frametype_I && m_vmd) {
							metaFrame.setVideoMeta(m_vmd->livingMeta.buffer,m_vmd->livingMeta.bufLen,  videoType);
							metaFrame.setVideoFrametype(YANG_Frametype_Spspps);
							metaFrame.setMetaTimestamp(data.getVideoTimestamp());
							ret = m_pushs.at(i)->publishVideoData(&metaFrame);
						}
				ret = m_pushs.at(i)->publishVideoData(&data);
				if (ret&&!m_pushs.at(i)->isconnected()) {
					m_pushs.back()->m_streamInit = 0;
					m_pushs.back()->disConnectServer();
					yang_post_message(YangM_Sys_PushMediaServerError,m_pushs.back()->m_uid,NULL);
				}
			}

		}

	}			//end
	isPublished = 0;
}
