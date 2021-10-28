#ifndef __yangmeetingtype2__
#define __yangmeetingtype2__
#include <yangmeeting/YangMeetingMessageI.h>
#include "yangutil/buffer/YangVideoBuffer.h"
#include <yangstream/YangStreamType.h>
#include <yangutil/yangavinfotype.h>
#include <vector>
#include <map>
#define HavingVr 0
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


class YangMeetingContext:public YangContext {
public:
	YangMeetingContext();
	~YangMeetingContext();
	void initExt(void *filename);
    std::map<int,YangAudioParam*> remoteAudio;
    std::map<int,YangVideoParam*> remoteVideo;

	std::vector<int> audiouids;
	std::vector<int> videouids;
	YangZbhdParam hd;
	YangUser user;
	YangMeeting meeting;
	YangReceiveMessageI *receive;
#if HavingVr
	char bgFilename[128];
#endif
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
