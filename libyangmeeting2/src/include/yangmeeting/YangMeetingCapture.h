/*
 * YangMeetingCapture.h
 *
 *  Created on: 2019年10月20日
 *      Author: yang
 */

#ifndef YANGVIDEOMEETING_INCLUDE_YANGMEETINGCAPTURE_H_
#define YANGVIDEOMEETING_INCLUDE_YANGMEETINGCAPTURE_H_
#include <vector>
#include <yangutil/yang_unistd.h>
#include <yangaudiodev/YangAudioCapture.h>
#include <yangcapture/YangCaptureFactory.h>
#include <yangcapture/YangMultiVideoCapture.h>
#include <yangmeeting/yangmeetingtype.h>
#include <yangutil/yangavtype.h>
#include <yangutil/sys/YangIni.h>
#include <yangutil/buffer/YangAudioBuffer.h>
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangavutil/audio/YangAecBase.h>
#include <yangutil/buffer/YangAudioPlayBuffer.h>

using namespace std;


class YangMeetingCapture {
public:
	YangMeetingCapture();
	virtual ~YangMeetingCapture();
	int32_t initAudio(YangPreProcess *pp);
	void startAudioCapture();
	YangAudioBuffer* getOutAudioBuffer();
	void stopAudioCaptureState();
	void startAudioCaptureState();
	void setAec(YangAecBase *paec);
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf);
public:
	virtual void startVideoCaptureState()=0;
	virtual void stopVideoCaptureState()=0;
	virtual void initVideo()=0;
	virtual void startVideoCapture()=0;
	virtual YangVideoBuffer* getOutVideoBuffer()=0;
	virtual YangVideoBuffer* getPreVideoBuffer()=0;
	virtual void stopAll()=0;
	virtual void change(int32_t st)=0;
#if HavingVr
	virtual void addVr()=0;
	virtual void delVr()=0;
#endif
protected:
	YangMeetingContext *m_context;
	YangAudioBuffer *m_out_audioBuffer;
	YangAudioCapture *m_audioCapture;
};

#endif /* YANGVIDEOMEETING_INCLUDE_YANGMEETINGCAPTURE_H_ */
