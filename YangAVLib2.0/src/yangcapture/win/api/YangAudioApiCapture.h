/*
 * YangAudioApiCapture.h
 *
 *  Created on: 2021年9月20日
 *      Author: yang
 */
#ifndef SRC_YANGCAPTURE_WIN_API_YANGAUDIOAPICAPTURE_H_
#define SRC_YANGCAPTURE_WIN_API_YANGAUDIOAPICAPTURE_H_
#include <yangutil/sys/YangThread.h>
#include <yangutil/yangavinfotype.h>
#include "YangCaptureCallback.h"
class YangAudioApiCapture:public YangThread{
public:
	YangAudioApiCapture(){};
	virtual ~YangAudioApiCapture(){};
	virtual void setCaptureCallback(YangCaptureCallback* cb)=0;
    virtual int initCapture()=0;
    virtual int startCpature()=0;
    virtual int stopCapture()=0;
};



#endif /* SRC_YANGCAPTURE_WIN_API_YANGAUDIOAPICAPTURE_H_ */
