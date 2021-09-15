/*
 * YangRtmpBase.h
 *
 *  Created on: 2019年10月13日
 *      Author: yang
 */

#ifndef YANGAPP_MeetingNetBase_H_
#define YANGAPP_MeetingNetBase_H_
#include <yangmeeting/include/YangPublish.h>
#include <yangmeeting/include/YangReceive.h>
#include <yanguserdata/include/YangCommandHandle.h>
#include <yanguserdata/include/YangUserDataBase.h>
#include <yangmeeting/yangmeetingtype.h>
#include <vector>
#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangVideoDecoderBuffer.h"
#include "yangutil/buffer/YangVideoEncoderBuffer.h"
#include "yangutil/sys/YangIni.h"

using namespace std;
class YangMeetingNet {
public:
	YangMeetingNet(YangMeetingContext *pini);
	virtual ~YangMeetingNet();
	void initNet();
	YangAudioEncoderBuffer* getOutAudioList();
	YangVideoDecoderBuffer* getOutVideoList();
	int32_t connectServer(int32_t isPubAudio, int32_t isPubVideo);
	vector<YangUser*>* getUserList();
	void netstateHandle(int32_t netstate);
	void initMediaNet();

	int32_t connectMediaServer(int32_t isPubAudio, int32_t isPubVideo);
	int32_t disconnectServer();

	int32_t reconnectServer();
	void closeConnect();
	int32_t connectPushServer(int32_t puid);
	int32_t connectPlayServer(int32_t puid);

	int32_t login(int32_t puid);
	int32_t logout(int32_t puid);

	int32_t pubAudio();
	int32_t pubVideo();
	int32_t playAudio(int32_t puid);
	int32_t playVideo(int32_t puid);

	int32_t unPubAudio();
	int32_t unPubVideo();
	int32_t unPlayAudio(int32_t puid);
	int32_t unPlayVideo(int32_t puid);

	// void setPlayAudioI(YangSysMessageHandleI *ppai);
	void setInAudioList(YangAudioEncoderBuffer *pal);
	void setInVideoList(YangVideoEncoderBuffer *pvl);
	void setInVideoMetaData(YangVideoMeta *pvmd);

	int32_t getIsAudioPublished();
	int32_t getIsVideoPublished();
	void setNetworkState(int32_t pst);
	void stopAll();

    int32_t sendHeartbeat();

protected:
	YangCommandHandle *m_dataConn;
	YangUserDataBase *m_userData;
	YangReceive *m_receive_media;
	YangPublish *m_publish_media;

	vector<YangUser*> *m_userList;
	YangAudioEncoderBuffer *m_out_al;
	YangVideoDecoderBuffer *m_out_vl;

	YangMeetingContext *m_ini;
	void startPublishAudio();
	void startPublishVideo();
private:
	void initList();
	int32_t m_connState;

};

#endif /* YANGAPP_YANGRTMPAPP_H_ */
