#include <yangmeeting/YangReceive.h>
#include <yangmeeting/YangMeetingMessageI.h>
#include <yangutil/yang_unistd.h>
#include <yangutil/sys/YangLog.h>
#include <yangstream/YangStreamFactory.h>
#include <yangstream/YangStreamType.h>

YangReceive::YangReceive(YangMeetingContext *pcontext) {
	m_context = pcontext;
	m_context->streams.setDecoderMediaConfigCallback(this);
	isReceiveConvert = 0;
	isReceived = 0;
	m_isStart = 0;
	m_out_videoBuffer = NULL;
	m_out_audioBuffer = NULL;
	isReceived = 0;
	isReceiveConvert = 0;
	m_headLen = pcontext->audio.audioDecoderType == 0 ? 2 : 1;
	m_cache = NULL;
}

YangReceive::~YangReceive() {

	if (isReceiveConvert) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	disConnectAll();
//	for (uint32_t i = 0; i < m_players.size(); i++) {
	//	if (m_players.at(i)) {
//			delete m_players.at(i);
	//		m_players.at(i) = NULL;
	//	}
	//}
	m_players.clear();
	yang_delete(m_cache);
	m_context = NULL;
	m_out_audioBuffer = NULL;
	m_out_videoBuffer = NULL;
}

void YangReceive::receiveAudio(YangFrame *audioFrame) {
	audioFrame->payload += m_headLen;
	audioFrame->nb -= m_headLen;
	m_out_audioBuffer->putPlayAudio(audioFrame);

}
void YangReceive::receiveVideo(YangFrame *videoFrame) {
    if(videoFrame==NULL||videoFrame->payload==NULL) return;
    uint8_t *temp = videoFrame->payload;
    int videoLen=videoFrame->nb;

    if( (temp[0] == 0x27|| temp[0] == 0x2c)&&temp[1] == 0x01){
        videoFrame->payload = temp + 5;
        videoFrame->nb -= 5;
        videoFrame->frametype = YANG_Frametype_P;
        if(yang_hasPframe(videoFrame->payload)) m_out_videoBuffer->putEVideo(videoFrame);
         return;

    }
    if ((temp[0] == 0x17 || temp[0] == 0x1c) ) {
        if(temp[1] == 0x00){
            videoFrame->frametype = YANG_Frametype_Spspps;
            m_out_videoBuffer->putEVideo(videoFrame);
            return;
        }
        if(temp[1] == 0x01){


            videoFrame->payload=temp+5;
            videoFrame->nb=videoLen-5;

            yang_parseNalue(videoFrame,&m_nalu);

            if(m_nalu.spsppsPos>-1){
                  uint8_t meta[200] = { 0 };
                  videoFrame->payload=meta;
                  yang_getSpsppseNalu(videoFrame,temp+5+m_nalu.spsppsPos);
                  videoFrame->frametype = YANG_Frametype_Spspps;
                  m_out_videoBuffer->putEVideo(videoFrame);
              }
              if(m_nalu.keyframePos>-1){
                  videoFrame->payload = temp + 5+m_nalu.keyframePos;
                  videoFrame->nb = videoLen-5-m_nalu.keyframePos;
                  videoFrame->frametype =YANG_Frametype_I;
                  m_out_videoBuffer->putEVideo(videoFrame);

              }
        }
    }
}

void YangReceive::setMediaConfig(int32_t puid, YangAudioParam *audio,
		YangVideoParam *video) {

	if (m_context)
		m_context->remoteAudio[puid] = audio;
	if (m_context)
		m_context->remoteVideo[puid] = video;
	yang_post_message(YangM_Sys_NofityMediaParam, puid, NULL);
}

int32_t YangReceive::connectServer(int32_t puid) {
	if (getPlayer(puid)) {
		return getPlayer(puid)->connectServer();
	}
	return 1;
}

int32_t YangReceive::playAudio(int32_t puid) {
	addPlay(puid);
	getPlayer(puid)->m_audioStream = 1;
	if (getPlayer(puid)->isconnected())
		return Yang_Ok;
	int32_t ret = getPlayer(puid)->connectServer();
	if (ret)
		return ret;
	getPlayer(puid)->m_streamInit = 1;
	return ret;
}

int32_t YangReceive::playVideo(int32_t puid) {
	addPlay(puid);
	getPlayer(puid)->m_videoStream = 1;
	if (getPlayer(puid)->isconnected())
		return Yang_Ok;
	int32_t ret = getPlayer(puid)->connectServer();
	if (ret)
		return ret;
	getPlayer(puid)->m_streamInit = 1;
	return ret;
}

int32_t YangReceive::unPlayAudio(int32_t puid) {
	int32_t index = getPlayIndex(puid);
	if (index > -1) {
		YangStreamHandle *srt = m_players.at(index);
		if (srt) {
			srt->m_audioStream = 0;
			removePlay(puid);
		}
		srt = NULL;

	}
	return Yang_Ok;
}

int32_t YangReceive::unPlayVideo(int32_t puid) {
	int32_t index = getPlayIndex(puid);
	if (index > -1) {
		YangStreamHandle *srt = m_players.at(index);
		if (srt) {
			srt->m_videoStream = 0;
			removePlay(puid);
		}
		srt = NULL;
	}
	return Yang_Ok;
}

void YangReceive::disConnectAll() {
	for (size_t i = 0; i < m_players.size(); i++) {
		m_players.at(i)->disConnectServer();
		delete m_players.at(i);
		m_players.at(i) = NULL;
	}
	m_players.clear();
}

void YangReceive::disConnect(int32_t puid) {
	for (size_t i = 0; i < m_players.size(); i++) {
		if (m_players.at(i)->m_uid == puid) {
			m_players.at(i)->disConnectServer();
			delete m_players.at(i);
			m_players.at(i) = NULL;
			m_players.erase(m_players.begin() + i);
			return;
		}
	}
}
void YangReceive::removePlay(int32_t puid) {
	int ind=-1;
	if(m_context) ind=m_context->streams.getIndex(puid);
	if(ind>-1){
		yang_delete(m_context->streams.m_playBuffers->at(ind));
		m_context->streams.m_playBuffers->erase(m_context->streams.m_playBuffers->begin()+ind);
	}

	for (int32_t index = 0; index < (int) m_players.size(); index++) {
		if (m_players.at(index)->m_uid == puid) {
			if (m_players.at(index)->m_audioStream
					|| m_players.at(index)->m_videoStream)
				return;
			m_players.at(index)->m_streamInit = 0;
			m_players.at(index)->disConnectServer();
			delete m_players.at(index);
			m_players.at(index) = NULL;
			m_players.erase(m_players.begin() + index);
			return;
		}

	}
}
int32_t YangReceive::getPlayIndex(int32_t puid) {
	for (int32_t i = 0; i < (int) m_players.size(); i++) {
		if (m_players.at(i)->m_uid == puid)
			return i;
	}
	return -1;
}

YangStreamHandle* YangReceive::getPlayer(int32_t puid) {
	for (size_t i = 0; i < m_players.size(); i++) {
		if (m_players.at(i)->m_uid == puid)
			return m_players.at(i);
	}
	return NULL;
}
void YangReceive::setOutAudioBuffer(YangAudioEncoderBuffer *pbuf) {
	m_out_audioBuffer = pbuf;
}
void YangReceive::setOutVideoBuffer(YangVideoDecoderBuffer *pbuf) {
	m_out_videoBuffer = pbuf;
}
void YangReceive::stop() {
	isReceiveConvert = 0;
}

void YangReceive::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangReceive::setReceiveBytes(int32_t pbytes) {

}
int32_t YangReceive::reconnect() {
	int32_t ret = Yang_Ok;
	int32_t retCode = Yang_Ok;
	for (size_t i = 0; i < m_players.size(); i++) {
		if (m_players.at(i)->getConnectState()) {
			retCode = m_players.at(i)->reconnect();
			if (retCode)
				ret = retCode;
		}

	}
	return ret;
}
void YangReceive::addPlay(int32_t puid) {
	if (getPlayIndex(puid) == -1) {
		YangStreamFactory sf;
		int32_t ret = 0;
		YangStreamConfig streamConf;
		streamConf.streamOptType = Yang_Stream_Play;
		streamConf.app = "live";
		streamConf.uid = puid;

		streamConf.localIp = "127.0.0.1";
		streamConf.localPort = m_context->sys.rtcLocalPort;	//8100;

		char s[128] = { 0 };
		m_players.push_back(sf.createStreamHandle(m_context->sys.transType, puid,m_context));
		if (m_context->sys.transType == Yang_Rtmp) {
			sprintf(s, "user%d", puid);
			streamConf.serverIp = m_context->sys.rtmpServerIP;
			streamConf.serverPort = m_context->sys.rtmpPort;
		} else if (m_context->sys.transType == Yang_Srt) {
			sprintf(s, "%d.%d", m_context->meeting.meetingId, puid);
			streamConf.serverIp = m_context->sys.srtServerIP;
			streamConf.serverPort = m_context->sys.srtPort;
		} else if (m_context->sys.transType == Yang_Webrtc) {
			sprintf(s, "user%d", puid);
			streamConf.serverIp = m_context->sys.rtcServerIP;
			streamConf.serverPort = m_context->sys.rtcPort;
			m_headLen = 0;
		}

		streamConf.stream = s;
		m_players.back()->setReceiveCallback(this);
		m_players.back()->init(&streamConf);

		if(m_context&&m_context->streams.getIndex(puid)==-1){
			m_context->streams.m_playBuffers->push_back(new YangSynBuffer());
			m_context->streams.m_playBuffers->back()->m_uid=puid;
		}
		if (ret) {
			yang_error("player connect error");
		}

		//printf("\n**************************************");
		//	printf("\nAdd player uid==============%d",puid);
		//	printf("\n*****************************************");
	}
}

void YangReceive::startLoop() {

	yang_reindex(m_out_audioBuffer);
	yang_reindex(m_out_videoBuffer);
	int32_t receiveBytes = 0;
	int32_t bufLen = 0;
	int32_t retCode = Yang_Ok;
	isReceiveConvert = 1;
	isReceived = 1;
	int32_t tuid = 0;
	while (isReceiveConvert == 1) {
		if (m_players.size() == 0) {
			yang_usleep(10000);
			continue;
		}
		/**	if(retCode){
		 retCode=reconnect();
		 if(!isReceiveConvert) break;
		 if(retCode) sleep(3);
		 continue;
		 }**/
		receiveBytes = 0;
		for (uint32_t i = 0; i < m_players.size(); i++) {
			if (!m_players.at(i)->m_streamInit)
				continue;
			bufLen = 0;
			tuid = m_players.at(i)->m_uid;
			retCode = m_players.at(i)->receiveData(&bufLen);
			if (retCode && !m_players.at(i)->isconnected()) {
				if (i < m_players.size() && m_players.at(i)->m_uid == tuid) {
					m_players.at(i)->m_streamInit = 0;
					m_players.at(i)->disConnectServer();
					yang_post_message(YangM_Sys_PlayMediaServerError,
							m_players.at(i)->m_uid, NULL);
				}
			}
			//printf("r%d",bufLen);
			receiveBytes += bufLen;
		}
		//if (retCode) {
		//	yang_error("Receive Data Error:%d", retCode);
		//	break;
		//}
		if (receiveBytes == 0)
			yang_usleep(1000);
	}	            		//end while
	//yang_debug("\n***********stop data receive......***************************\n");

	isReceived = 0;
}
