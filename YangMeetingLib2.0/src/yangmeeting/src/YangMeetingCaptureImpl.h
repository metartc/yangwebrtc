/*
 * YangCaptureBase.h
 *
 *  Created on: 2019年10月11日
 *      Author: yang
 */

#ifndef YANGAPP_YangMeetingCaptureImpl_H_
#define YANGAPP_YangMeetingCaptureImpl_H_
#include "../include/YangMeetingCapture.h"
using namespace std;
class YangMeetingCaptureImpl :public YangThread ,public YangMeetingCapture{
public:
	YangMeetingCaptureImpl(YangMeetingContext *pini);
	virtual ~YangMeetingCaptureImpl();
public:
	void startVideoCaptureState();
	void stopVideoCaptureState();
	void initVideo();
	void startVideoCapture();
	YangVideoBuffer * getOutVideoBuffer();
	YangVideoBuffer * getPreVideoBuffer();
	void stopAll();
	void change(int32_t st);
	void addVr();
	void delVr();

private:
	YangVideoBuffer *m_out_videoBuffer;
	YangVideoBuffer *m_pre_videoBuffer;
	YangMultiVideoCapture *m_videoCapture=NULL;
public:
	void stop();
	int32_t m_isStart;
protected:
	void run();
	void startLoop();
	void stopLoop();
	void initVr();
	YangVideoBuffer *m_out_vr_pre_videoBuffer;
	int32_t m_isConvert;
};

#endif /* YANGAPP_YANGCAPTUREAPP_H_ */
