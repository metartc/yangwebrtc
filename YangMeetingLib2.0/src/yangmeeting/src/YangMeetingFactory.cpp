/*
 * YangUtilFactory.cpp
 *
 *  Created on: 2020年10月17日
 *      Author: yang
 */
#include <yangmeeting/src/YangMeetingHandleImpl.h>
#include <yangmeeting/yangmeetingtype.h>
#include <yangmeeting/YangMeetingFactory.h>
#include "yangutil/sys/YangIni.h"
#include "YangMeetingMessageHandle.h"
class YangMeetingConfigIni:public YangIni{
public:
	YangMeetingConfigIni();
	~YangMeetingConfigIni();
	void initHd(YangZbhdParam *hd);
};


YangMeetingFactory::YangMeetingFactory() {

}
YangSysMessageHandle* YangMeetingFactory::createSysMessageHandle(YangMeetingContext *pini,YangReceiveMessageI* prec){
	return new YangMeetingMessageHandle(pini,prec);
}
YangMeetingHandle* YangMeetingFactory::createMeetingHandle(YangMeetingContext *pini){
	return new YangMeetingHandleImpl(pini);
}

YangMeetingHandle* YangMeetingFactory::getMeetingHandle(YangSysMessageHandle *phandle){
	return ((YangMeetingMessageHandle*)phandle)->m_videoMeeting;
}

void YangMeetingFactory::createIni(const char* p_filename,YangMeetingContext *pini){
	YangMeetingConfigIni yi;
	yi.init(p_filename);
	//pini->m_log=new YangLog();
	yi.initAudio(&pini->audio);
	yi.initVideo(&pini->video);
	yi.initSys(&pini->sys);
	yi.initHd(&pini->hd);
	yi.initEnc(&pini->enc);
	yi.initRtc(&pini->rtc);
	//yi.initCamera(&pini->camera);
	//pini->hasVr=yi.readIntValue("sys","hasVr",0);
	memset(pini->bgFilename, 0, sizeof(pini->bgFilename));
	yi.readStringValue("sys", "bgFilename", pini->bgFilename, "/home/yang/bmp/jpg/03.jpeg");
	//pini->isMulterCamera=0;
	//pini->context=new YangMeetingContext1();


}

YangMeetingConfigIni::YangMeetingConfigIni(){

}

YangMeetingConfigIni::~YangMeetingConfigIni(){

}

void YangMeetingConfigIni::initHd(YangZbhdParam* hd){
	   memset(hd,0,sizeof(YangZbhdParam));

		hd->meetingId = readIntValue("hd", "meetingId", 1001);
		//hd->userId = readIntValue("hd", "userId", 1001);
		memset(hd->app, 0, sizeof(hd->app));
		readStringValue("hd", "app", hd->app, "live");

}
