#include <yangmeeting/include/YangReceive.h>
#include <yangmeeting/YangMeetingMessageI.h>
#include "unistd.h"
#include "yangutil/sys/YangLog.h"
#include "yangstream/YangStreamFactory.h"
#include <yangstream/YangStreamType.h>
YangReceive::YangReceive(YangMeetingContext *pini) {
	m_ini = pini;
	isReceiveConvert = 0;
	isReceived = 0;
	m_isStart = 0;
	m_out_videoBuffer = NULL;
	m_out_audioBuffer = NULL;
	isReceived = 0;
	isReceiveConvert = 0;
	m_headLen = pini->audio.audioDecoderType == 0 ? 2 : 1;
	m_cache = NULL;
}

YangReceive::~YangReceive() {

	if (isReceiveConvert) {
		stop();
		while (m_isStart) {
			usleep(1000);
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
	m_ini = NULL;
	m_out_audioBuffer = NULL;
	m_out_videoBuffer = NULL;
}

void YangReceive::receiveAudio(YangFrame *audioFrame) {
	audioFrame->payload += m_headLen;
	audioFrame->nb -= m_headLen;
	m_out_audioBuffer->putPlayAudio(audioFrame);

}
void YangReceive::receiveVideo(YangFrame *videoFrame) {
	uint8_t *temp = videoFrame->payload;
	if (temp && (temp[0] == 0x17 || temp[0] == 0x1c) && temp[1] == 0x00) {
		videoFrame->frametype = 9;
		m_out_videoBuffer->putEVideo(videoFrame);

	} else if (temp
			&& (temp[0] == 0x17 || temp[0] == 0x27 || temp[0] == 0x1c
					|| temp[0] == 0x2c) && temp[1] == 0x01) {


		if (temp[0] == 0x17 && temp[9] == 0x67) {
			int32_t spsLen = *(temp + 8) + 1;
			int32_t ppsLen = *(temp + 8 + spsLen + 2 + 1) + 1;
			int32_t allLen = 8 + spsLen + ppsLen + 2 + 1;
			int32_t kenLen = videoFrame->nb - allLen;
			/**if (m_cache == NULL) {
				m_cache = new char[256];
				memset(m_cache, 0, 256);
				//memcpy(m_cache,temp,11);
				memcpy(m_cache + 11, temp + 7, spsLen + 2);
				*(m_cache + 11 + spsLen + 2) = 0x01;
				memcpy(m_cache + 11 + spsLen + 2 + 1, temp + 7 + spsLen + 2 + 2,
						ppsLen + 2);
				//printf("\n");
				//for(int32_t i=0;i<ppsLen+2;i++) printf("%02x,",*(temp+7+spsLen+2+2+i));
				int32_t len = 11 + spsLen + ppsLen + 2 + 2 + 1;
				m_cache[0] = 0x17;
				m_cache[1] = 0x0;
				//printf("\nspsLen==%d,ppsLen==%d",spsLen,ppsLen);
				//printf("\n");
				//for(int32_t i=0;i<len;i++) printf("%02x,",(uint8_t)m_cache[i]);

			}**/

			videoFrame->frametype = 1;
			videoFrame->payload = temp + allLen;
			videoFrame->nb = kenLen;
			m_out_videoBuffer->putEVideo(videoFrame);


		} else {
			videoFrame->payload = temp + 5;
			videoFrame->nb -= 5;
			videoFrame->frametype = (*temp == 0x17 || *temp == 0x1c);
			m_out_videoBuffer->putEVideo(videoFrame);

		}
	}
}

void YangReceive::setMediaConfig(int32_t puid, YangAudioParam *audio,YangVideoParam *video) {
	m_ini->remoteAudio[puid] = audio;
	m_ini->remoteVideo[puid] = video;
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
	if (getPlayer(puid)->isconnected())		return Yang_Ok;
	int32_t ret = getPlayer(puid)->connectServer();
	if (ret)	return ret;
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
		streamConf.localPort = m_ini->sys.rtcLocalPort;	//8100;

		char s[128] = { 0 };
		m_players.push_back(sf.createStreamHandle(m_ini->sys.transType, puid));
		if (m_ini->sys.transType == Yang_Rtmp) {
			sprintf(s, "user%d", puid);
			streamConf.serverIp = m_ini->sys.rtmpServerIP;
			streamConf.serverPort = m_ini->sys.rtmpPort;
		} else if (m_ini->sys.transType == Yang_Srt) {
			sprintf(s, "%d.%d", m_ini->meeting.meetingId, puid);
			streamConf.serverIp = m_ini->sys.srtServerIP;
			streamConf.serverPort = m_ini->sys.srtPort;
		} else if (m_ini->sys.transType == Yang_Webrtc) {
			sprintf(s, "user%d", puid);
			streamConf.serverIp = m_ini->sys.rtcServerIP;
			streamConf.serverPort = m_ini->sys.rtcPort;
			m_headLen = 0;
		}

		streamConf.stream = s;
		m_players.back()->setReceiveCallback(this);
		m_players.back()->init(&streamConf);
		m_players.back()->setRemoteMediaParamCallback(this);
		m_players.back()->setLocalMediaParam(NULL,NULL,&m_ini->rtc);
		if (ret) {
			yang_error("player connect error" );
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
			usleep(10000);
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
			if (retCode&&!m_players.at(i)->isconnected()) {
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
			usleep(1000);
	}	            		//end while
	//yang_debug("\n***********stop data receive......***************************\n");

	isReceived = 0;
}
