#ifndef __yangmeetingtype1__
#define __yangmeetingtype1__
#include <yangmeeting/YangMeetingMessageI.h>
#include "yangutil/buffer/YangVideoBuffer.h"
#include <yangstream/YangStreamType.h>
#include <yangutil/yangavinfotype.h>
#include <vector>
#include <map>
struct YangUser {
        int32_t userId;
	char userName[30];
	char loginName[30];
	int32_t myId;
	int32_t roomAdmin;
	int32_t micFlag;
	int32_t camFlag;
};
struct YangMeeting {
	int32_t meetingId;
	char meetingName[35];
	int32_t departId;
	int32_t meetingTime = 0;
	int32_t roomAdmin = 0;
};

struct YangZbhdParam{
	int32_t meetingId;
	char app[20];
};


struct YangMeetingContext {
	YangAudioInfo audio;
	YangVideoInfo video;
    std::map<int,YangAudioParam*> remoteAudio;
    std::map<int,YangVideoParam*> remoteVideo;
	YangSysInfo sys;
	YangZbhdParam hd;
	YangVideoEncInfo enc;
	YangRtcInfo rtc;
	std::vector<int> audiouids;
	std::vector<int> videouids;
	YangUser user;
	YangMeeting meeting;
	std::vector<YangVideoBuffer*> *videoBuffers;
	YangReceiveMessageI *receive;
	char bgFilename[128];
};
class YangMeetingMessage{
public:
	YangMeetingMessage(){};
	virtual ~YangMeetingMessage(){};
	virtual void startMessage()=0;
	virtual void stopMessage()=0;
	virtual int32_t getMessageState()=0;
	//void delMessage();
};
class YangVideoPlayHandle{
public:
	YangVideoPlayHandle(){};
	virtual ~YangVideoPlayHandle(){};
	virtual void removePlayvideo(int32_t puid)=0;
};
#endif
