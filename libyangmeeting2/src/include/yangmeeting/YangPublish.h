/*
 * YangPublish.h
 *
 *  Created on: 2019年9月11日
 *      Author: yang
 */
#ifndef SRC_YANGMEETING_INCLUDE_YANGPUBLISH_H_
#define SRC_YANGMEETING_INCLUDE_YANGPUBLISH_H_
#include <yangstream/YangStreamHandle.h>
#include <yangmeeting/yangmeetingtype.h>
#include <yangutil/sys/YangThread.h>

#include <yangutil/buffer/YangAudioEncoderBuffer.h>
#include <yangutil/buffer/YangVideoEncoderBuffer.h>
#include <vector>
using namespace std;

class YangPublish: public YangThread,public YangSendRequestCallback {
public:
	YangPublish(YangMeetingContext *pcontext);
	virtual ~YangPublish();
	int32_t pubAudioStream(int32_t puid);
	int32_t pubVideoStream(int32_t puid);
	int32_t unPublishAudioStream(int32_t puid);
	int32_t unPublishVideoStream(int32_t puid);

	int32_t connectServer(int32_t puid);
	int32_t connectMediaServer();
	int32_t disConnectMediaServer();
	int32_t reconnectMediaServer();
	void setInVideoMetaData(YangVideoMeta *pvmd);
	void setInAudioList(YangAudioEncoderBuffer *pbuf);
	void setInVideoList(YangVideoEncoderBuffer *pbuf);
	void sendRequest(int32_t puid,uint32_t ssrc,YangRequestType req);
	int32_t stopPublishAudioData();
	int32_t stopPublishVideoData();
	int32_t m_isPubVideo,m_isPubAudio;
	int32_t m_netState;
	int32_t isPublished;
	int32_t m_isStart;
	void stop();
protected:
	void run();
	void handleError(int32_t perrCode);
	void startLoop();
	YangMeetingContext *m_context;
	YangVideoMeta *m_vmd;
	YangVideoEncoderBuffer *m_in_videoBuffer;
	YangAudioEncoderBuffer *m_in_audioBuffer;

	int32_t m_isConvert;
	int32_t m_isInit;
	int32_t m_audioEncoderType;
	std::vector<YangStreamHandle*> m_pushs;
private:
	int32_t m_transType;
	int32_t notifyState;
};

#endif /* SRC_YANGMEETING_INCLUDE_YANGPUBLISH_H_ */
