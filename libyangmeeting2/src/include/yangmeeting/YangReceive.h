/*
 * YangReceive.h
 *
 *  Created on: 2019年10月11日
 *      Author: yang
 */
#ifndef SRC_YANGMEETING_INCLUDE_YANGRECEIVE_H_
#define SRC_YANGMEETING_INCLUDE_YANGRECEIVE_H_
#include <yangmeeting/yangmeetingtype.h>
#include <yangstream/YangStreamHandle.h>
#include <yangutil/sys/YangThread.h>
#include <yangavutil/video/YangNalu.h>
#include <yangutil/buffer/YangAudioEncoderBuffer.h>
#include <yangutil/buffer/YangVideoDecoderBuffer.h>
#include <vector>

using namespace std;

class YangReceive:public YangThread,public YangReceiveCallback,public YangMediaConfigCallback {
public:
	YangReceive(YangMeetingContext *pcontext);
	virtual ~YangReceive();

	int32_t isReceived;//,isHandled;
	int32_t isReceiveConvert;//,isHandleAllInvoke;

	void setOutAudioBuffer(YangAudioEncoderBuffer *pbuf);
	void setOutVideoBuffer(YangVideoDecoderBuffer *pbuf);
	void receiveAudio(YangFrame* audioFrame);
	void receiveVideo(YangFrame* videoFrame);
	void setMediaConfig(int32_t puid,YangAudioParam* audio,YangVideoParam* video);
public:
	int32_t playAudio(int32_t puid);
	int32_t playVideo(int32_t puid);
	int32_t unPlayAudio(int32_t puid);
	int32_t unPlayVideo(int32_t puid);
	void removePlay(int32_t puid);
	void disConnect(int32_t puid);
	void disConnectAll();
	int32_t reconnect();
	int32_t connectServer(int32_t puid);
	int32_t m_isStart;
	void stop();
protected:
	void run();
	 void startLoop();
	 void addPlay(int32_t puid);
	void setReceiveBytes(int32_t pbytes);
	YangStreamHandle* getPlayer(int32_t puid);
	int32_t getPlayIndex(int32_t puid);
	vector<YangStreamHandle*> m_players;
protected:
    YangNaluData m_nalu;
	YangMeetingContext *m_context;
	int32_t m_headLen;
	YangAudioEncoderBuffer *m_out_audioBuffer;
	YangVideoDecoderBuffer *m_out_videoBuffer;
	char* m_cache;

};

#endif /* SRC_YANGMEETING_INCLUDE_YANGRECEIVE_H_ */
