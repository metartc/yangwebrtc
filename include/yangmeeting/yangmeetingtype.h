#ifndef __yangmeetingtype1__
#define __yangmeetingtype1__
#include <yangmeeting/YangMeetingMessageI.h>
#include "yangutil/yangtype.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include <yangstream/YangStreamType.h>
#include <vector>
#include <map>
struct YangUser {
        int userId;
	char userName[30];
	char loginName[30];
	int myId;
	int roomAdmin;
	int micFlag;
	int camFlag;
};
struct YangMeeting {
	int meetingId;
	char meetingName[35];
	int departId;
	int meetingTime = 0;
	int roomAdmin = 0;
};

struct YangZbhdParam{
	int meetingId;
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
	std::vector<int> audiouids;
	std::vector<int> videouids;
	YangUser user;
	YangMeeting meeting;
	std::vector<YangVideoBuffer*> *videoBuffers;
	YangReceiveMessageI *receive;
	char bgFilename[128];
};

#endif
