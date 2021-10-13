#ifndef YangPushPublish_H
#define YangPushPublish_H
#include <yangpush/YangPushCapture.h>
#include <yangpush/YangPushEncoder.h>
#include <yangpush/YangRtcPublish.h>

#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangAudioBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/buffer/YangVideoEncoderBuffer.h"
#include "yangutil/buffer/YangVideoBuffer.h"
class YangPushPublish
{
    public:
        YangPushPublish(YangContext *pcontext);
        virtual ~YangPushPublish();


    void setNetBuffer(YangRtcPublish *prr);
    void startPubVideo();
    void startPubAudio();
	void initAudioEncoding();
	void initVideoEncoding();
	void startAudioCapture();
	void startVideoCapture();

	void initVideoMeeting();
	void startAudioEncoding();
	void startVideoEncoding();
	void startAudioCaptureState();
	void startVideoCaptureState();
	void stopAudioCaptureState();
	void stopVideoCaptureState();

	YangVideoBuffer * getPreVideoBuffer();
	void stopAll();
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf);
	void change(int32_t st);
	void addVr();
	void delVr();

	void sendKeyframe();
    protected:

    private:
	YangContext *m_context;
	YangPushEncoder *m_encoder;
	YangPushCapture *m_capture;
	 int32_t isStartAudioCapture,isStartVideoCapture;
	 int32_t isStartAudioEncoder,isStartVideoEncoder;
	void stopAudioState();
	void stopVideoState();
	void initCapture();
};

#endif //
