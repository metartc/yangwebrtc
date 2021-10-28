#include <yangmeeting/yangmeetingtype.h>
#include <yangutil/sys/YangIni.h>

YangMeetingContext::YangMeetingContext() {
	//videoBuffers=NULL;
	receive = NULL;
}
YangMeetingContext::~YangMeetingContext() {
	//videoBuffers=NULL;
	receive = NULL;
//	  for(std::map<int, YangAudioParam*>::iterator it = remoteAudio.begin(); it != remoteAudio.end(); ++it) {
	//        	yang_delete(it->second);
	//    }
	if (remoteAudio.size() > 0)
		remoteAudio.clear();

	// for(std::map<int, YangVideoParam*>::iterator it = remoteVideo.begin(); it != remoteVideo.end(); ++it) {
	// 	        	yang_delete(it->second);
	//	    }
	if (remoteVideo.size() > 0)
		remoteVideo.clear();

}

void YangMeetingContext::initExt(void *filename) {
	YangIni *ini = (YangIni*) filename;
	memset(&hd, 0, sizeof(YangZbhdParam));

	hd.meetingId = ini->readIntValue("hd", "meetingId", 1001);
	//hd->userId = readIntValue("hd", "userId", 1001);
	memset(hd.app, 0, sizeof(hd.app));
	ini->readStringValue("hd", "app", hd.app, "live");

#if HavingVr
	memset(bgFilename, 0, sizeof(bgFilename));
	yi.readStringValue("sys", "bgFilename", bgFilename, "/home/yang/bmp/jpg/03.jpeg");
#endif

	memset(&user,0,sizeof(YangUser));
	memset(&meeting,0,sizeof(YangMeeting));

}
