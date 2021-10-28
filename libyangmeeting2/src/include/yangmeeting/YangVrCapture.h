/*
 * YangVrCapture.h
 *
 *  Created on: 2019年10月29日
 *      Author: yang
 */

#ifndef YANGRECLIVING_INCLUDE_YANGVRCAPTURE_H_
#define YANGRECLIVING_INCLUDE_YANGVRCAPTURE_H_
#include <yangaudiodev/YangAudioCapture.h>
#include <yangcapture/YangCaptureFactory.h>
#include <yangcapture/YangMultiVideoCapture.h>
#include <yangrecliving/YangLivingType.h>
#include <yangutil/yangavinfotype.h>
#include <yangmeeting/YangMeetingCapture.h>

class YangVrCapture : public YangThread ,public YangMeetingCapture{
public:
	YangVrCapture(YangMeetingContext *pcontext);
	virtual ~YangVrCapture();
		void startVideoCaptureState();
		void stopVideoCaptureState();
		void initVideo();
		void startVideoCapture();
		YangVideoBuffer * getOutVideoBuffer();
		YangVideoBuffer * getPreVideoBuffer();
		void stopAll();
		void addVr();
		void delVr();

private:
	//YangCaptureFactory m_capture;
	YangVideoBuffer *m_out_videoBuffer;
	YangVideoBuffer *m_pre_videoBuffer;


private:
	vector<YangMultiVideoCapture *> m_vcs;
	int32_t m_cameraCount;
	int32_t m_pre_st;
public:
	void change(int32_t st);
	void stop();
	int32_t m_isStart;
protected:
	void run();

private:
	YangVideoBuffer *m_out_vr_pre_videoBuffer;
	int32_t m_isConvert;
	//int32_t m_hasVr;
	void startLoop();
	void stopLoop();
	//void initVr();

};
#endif /* YANGRECLIVING_INCLUDE_YANGVRCAPTURE_H_ */
