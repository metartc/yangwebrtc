#include <unistd.h>
#include <yangplayer/YangPlayerBase.h>
#include <yangplayer/YangPlayReceive.h>
#include "yangutil/sys/YangLog.h"
#include "yangstream/YangStreamFactory.h"




YangPlayReceive::YangPlayReceive() {
	m_isStart = 0;
	m_out_videoBuffer = NULL;
	m_out_audioBuffer = NULL;
	isReceived = 0;
	isReceiveConvert = 0;
	m_headLen = 1; //pini->audio.audioDecoderType == 0 ? 2 : 1;
	m_recv = NULL;
}

YangPlayReceive::~YangPlayReceive() {
	disConnect();
	if (isReceiveConvert) {
		stop();
		while (m_isStart) {
			usleep(1000);
		}
	}

	if (m_recv)
		delete m_recv;
	m_recv = NULL;

	m_out_audioBuffer = NULL;
	m_out_videoBuffer = NULL;
}

void YangPlayReceive::disConnect() {
	m_recv->disConnectServer();
	delete m_recv;
	m_recv = NULL;
}
void YangPlayReceive::setBuffer(YangAudioEncoderBuffer *al,	YangVideoDecoderBuffer *vl) {
	m_out_audioBuffer = al;
	m_out_videoBuffer = vl;
}
void YangPlayReceive::receiveAudio(YangFrame* audioFrame) {
	audioFrame->payload+=m_headLen;
	audioFrame->nb-=m_headLen;
    m_out_audioBuffer->putPlayAudio(audioFrame);
}
void YangPlayReceive::receiveVideo(YangFrame* videoFrame) {
	uint8_t* temp=videoFrame->payload;
    if (temp && (temp[0] == 0x17 || temp[0] == 0x1c) && temp[1] == 0x00) {
		//printf("\n0-");
		//for(int32_t i=0;i<plen;i++) printf("%02x,",temp[i]);
		//printf("s%d-%d,",plen,keyframe);
    	videoFrame->frametype=9;
		m_out_videoBuffer->putEVideo(videoFrame);
	} else if (temp
			&& (temp[0] == 0x17 || temp[0] == 0x27 || temp[0] == 0x1c
					|| temp[0] == 0x2c) && temp[1] == 0x01) {
		//printf("s%d-%d,",plen,keyframe);
		//printf("\n1-");
		//for(int32_t i=0;i<50;i++) printf("%02x,",temp[i]);
		//m_out_vl->putEVideo(t_uid, temp + 9,tpk->m_nBodySize - 9, t_timestamp,	t_isKeyframe);
		videoFrame->payload=temp+5;
		videoFrame->nb-=5;
		videoFrame->frametype=(*temp == 0x17 || *temp == 0x1c);
		m_out_videoBuffer->putEVideo(videoFrame);
	}
}

int32_t YangPlayReceive::init(int32_t nettype, string server, int32_t pport,
		string stream) {
	//m_play=
	YangStreamFactory sf;
	if (!m_recv)
		m_recv = sf.createStreamHandle(nettype, 0);
	YangStreamConfig streamConf;
	streamConf.app="live";
	streamConf.streamOptType=Yang_Stream_Play;
	streamConf.uid=0;

	streamConf.localIp="127.0.0.1";
	streamConf.localPort=8100;
	streamConf.serverIp=server;
	streamConf.serverPort=pport;
	streamConf.stream=stream;
	m_recv->init(&streamConf);
	m_recv->m_videoStream=1;
	m_recv->setReceiveCallback(this);
	m_recv->m_uid=0;
	int32_t ret=m_recv->connectServer();
	if(ret) return ret;
	m_recv->m_streamInit=1;
	return ret;

}
void YangPlayReceive::stop() {
	isReceiveConvert = 0;
}
void YangPlayReceive::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangPlayReceive::startLoop() {

	yang_reindex(m_out_audioBuffer);
	yang_reindex(m_out_videoBuffer);
	//int32_t receiveBytes = 0;
	int32_t bufLen = 0;
	int32_t retCode = Yang_Ok;
	isReceiveConvert = 1;
	isReceived = 1;
	//int32_t tuid = 0;
	printf("\n**********************************play receive ...start....");
	while (isReceiveConvert == 1) {
		if (!m_recv) {
			usleep(10000);
			continue;
		}
		/**	if(retCode){
		 retCode=reconnect();
		 if(!isReceiveConvert) break;
		 if(retCode) sleep(3);
		 continue;
		 }**/
	//	receiveBytes = 0;

		if (!m_recv->m_streamInit)
			continue;
		bufLen = 0;
		//tuid=m_players.at(i)->m_uid;
		retCode = m_recv->receiveData(&bufLen);
		//printf("%d",bufLen);

		//printf("r%d",bufLen);
		//receiveBytes += bufLen;

		//if (retCode) {
		//	yang_error("Receive Data Error:%d", retCode);
		//	break;
		//}
		if (bufLen == 0)
			usleep(2000);
	}	            		//end while
	printf("\n**********************************play receive ...end....");
	//yang_debug("\n***********stop data receive......***************************\n");

	isReceived = 0;
}
