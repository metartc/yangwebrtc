#include <stdio.h>
#include <yangutil/yang_unistd.h>
#include <yangpush/YangRtcPublish.h>
#include <yangstream/YangStreamCapture.h>
#include "yangstream/YangStreamFactory.h"
#include <yangstream/YangStreamType.h>

#include <yangutil/sys/YangLog.h>
YangRtcPublish::YangRtcPublish(YangContext *pcontext) {
	m_context = pcontext;

	m_in_videoBuffer = NULL;
	m_in_audioBuffer = NULL;
	m_isStart = 0;
	m_isConvert = 0;
	m_vmd = NULL;
	m_audioEncoderType = m_context->audio.audioEncoderType;
	m_netState = 1;
	m_isInit = 0;
	isPublished = 0;

	m_transType=Yang_Webrtc;
	notifyState=0;
}

YangRtcPublish::~YangRtcPublish() {
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


int32_t YangRtcPublish::connectServer(int32_t puid){
	return m_pushs.back()->connectServer();
}



int32_t YangRtcPublish::reconnectMediaServer() {
	return m_pushs.back()->reconnect();

}

int32_t YangRtcPublish::init(int32_t nettype, string server, string localIp,int32_t localPort,int32_t pport,
		string app,string stream) {
		YangStreamFactory sf;
		int32_t ret = 0;
		YangStreamConfig streamConf;
		streamConf.app=app;
		streamConf.streamOptType=Yang_Stream_Publish;
		streamConf.serverIp=server;
		streamConf.serverPort=pport;
		streamConf.stream=stream;
		streamConf.uid=0;//0 singleuser 1 multiuser

		streamConf.localIp=localIp;
		streamConf.localPort=localPort;

		if (m_pushs.size() == 0) {
				m_pushs.push_back(sf.createStreamHandle(m_context->sys.transType,0,m_context));
				m_pushs.back()->init(&streamConf);

			}


		if (m_pushs.back()->isconnected())		return Yang_Ok;
		ret = m_pushs.back()->connectServer();

		if (ret)		return ret;
		m_pushs.back()->m_streamInit = 1;
		yang_reindex(m_in_audioBuffer);
		yang_reindex(m_in_videoBuffer);
	return Yang_Ok;

}
int32_t YangRtcPublish::connectMediaServer() {
	if(m_pushs.size()>0) return Yang_Ok;

	return Yang_Ok;
}
int32_t YangRtcPublish::disConnectMediaServer() {
	if (m_pushs.size() > 0) {
		m_pushs.back()->m_streamInit = 0;
		delete m_pushs.back();
		m_pushs.back() = NULL;
		m_pushs.clear();
	}
	return Yang_Ok;
}
void YangRtcPublish::stop() {
	m_isConvert = 0;
}

void YangRtcPublish::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}

void YangRtcPublish::setInAudioList(YangAudioEncoderBuffer *pbuf) {
	m_in_audioBuffer = pbuf;
}
void YangRtcPublish::setInVideoList(YangVideoEncoderBuffer *pbuf) {
	m_in_videoBuffer = pbuf;
}
void YangRtcPublish::setInVideoMetaData(YangVideoMeta *pvmd) {
	m_vmd = pvmd;
}

void YangRtcPublish::startLoop() {

	isPublished = 0;
	m_isConvert = 1;

	YangStreamCapture data;

	YangFrame audioFrame;
	YangFrame videoFrame;
	data.initAudio(m_context->sys.transType,m_context->audio.sample, m_context->audio.channel,
			(YangAudioEncDecType) m_context->audio.audioEncoderType);
	data.initVideo(m_context->sys.transType);
	YangVideoEncDecType videoType =	(YangVideoEncDecType) m_context->video.videoEncoderType;

	int32_t ret = Yang_Ok;
	isPublished = 1;
	notifyState=1;
	while (m_isConvert == 1) {


		if ((m_in_videoBuffer && m_in_videoBuffer->size() == 0)
				&& (m_in_audioBuffer && m_in_audioBuffer->size() == 0)) {
			yang_usleep(2000);
			continue;
		}
		if (m_pushs.size() == 0 || !m_pushs.back()->m_streamInit) {
			yang_usleep(500);
			continue;
		}
		YangStreamHandle* stream=m_pushs.back();

		if(stream->isconnected()){
			if(notifyState&&m_transType<Yang_Webrtc){
						if(m_context) m_context->streams.sendRequest(0,0,Yang_Req_Connected);
						notifyState=0;
					}

		}else{
			yang_usleep(500);
			continue;
		}

		if (m_in_audioBuffer && m_in_audioBuffer->size() > 0) {

			audioFrame.payload = m_in_audioBuffer->getAudioRef(&audioFrame);
			data.setAudioData(&audioFrame);

			//for (i = 0; i < m_pushs.size(); i++) {
				ret = stream->publishAudioData(&data);
				if (ret&&!stream->isconnected()) {
					stream->m_streamInit = 0;
					stream->disConnectServer();
					//yang_post_message(YangM_Sys_PushMediaServerError,m_pushs.back()->m_uid,NULL);
				}
			//}
		}

		if (m_in_videoBuffer&& m_in_videoBuffer->size() > 0) {

			videoFrame.payload = m_in_videoBuffer->getEVideoRef(&videoFrame);

			if (videoFrame.frametype == YANG_Frametype_I && m_vmd) {
				data.setVideoMeta(m_vmd->livingMeta.buffer,m_vmd->livingMeta.bufLen,  videoType);
                data.setVideoFrametype(YANG_Frametype_Spspps);
				//for (i = 0; i < m_pushs.size(); i++) {
					ret = stream->publishVideoData(&data);

					if (ret&&!stream->isconnected()) {
						stream->m_streamInit = 0;
						stream->disConnectServer();
						//yang_post_message(YangM_Sys_PushMediaServerError,m_pushs.back()->m_uid,NULL);

					}
				//}
				if(ret) continue;

			}
			data.setVideoData(&videoFrame, videoType);
			//for (i = 0; i < m_pushs.size(); i++) {
				ret = stream->publishVideoData(&data);
				if (ret&&!stream->isconnected()) {
					stream->m_streamInit = 0;
					stream->disConnectServer();

				}
		//	}

		}


	}			//end
	isPublished = 0;
}
