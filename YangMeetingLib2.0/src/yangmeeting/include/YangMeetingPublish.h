#ifndef MeetingPubBase_H
#define MeetingPubBase_H
#include <yangmeeting/include/YangMeetingCapture.h>
#include <yangmeeting/include/YangMeetingEncoder.h>
#include <yangmeeting/include/YangMeetingNet.h>
#include <yangmeeting/yangmeetingtype.h>
#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangAudioBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/buffer/YangVideoEncoderBuffer.h"
#include "yangutil/buffer/YangVideoBuffer.h"
class YangMeetingPublish
{
    public:
        YangMeetingPublish(YangMeetingContext *pini);
        virtual ~YangMeetingPublish();


    void startPubVideo();
    void startPubAudio();
	void initAudioEncoding(YangMeetingNet *prr);
	void initVideoEncoding(YangMeetingNet *prr);
	void startAudioCapture(YangPreProcess *pp);
	void startVideoCapture();
	void setNetList(YangMeetingNet *prr);
	void initVideoMeeting();
	void startAudioEncoding();
	void startVideoEncoding();
	void startAudioCaptureState();
	void startVideoCaptureState();
	void stopAudioCaptureState();
	void stopVideoCaptureState();
	void setNetBuffer(YangMeetingNet *prr);
	YangVideoBuffer * getPreVideoBuffer();
	void stopAll();
	void setAec(YangAecBase *paec);
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf);
	void change(int32_t st);
	void addVr();
	void delVr();

	void sendKeyframe();
    protected:

    private:
	YangMeetingContext *m_ini;
	YangMeetingEncoder *m_encoder;
	YangMeetingCapture *m_capture;
	 int32_t isStartAudioCapture,isStartVideoCapture;
	 int32_t isStartAudioEncoder,isStartVideoEncoder;
	void stopAudioState();
	void stopVideoState();
	void initCapture();
};

#endif //
