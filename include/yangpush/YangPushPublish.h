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
class YangPushPublish :public YangSendRequestCallback
{
    public:
        YangPushPublish(YangContext *pcontext);
        virtual ~YangPushPublish();

    void setCaptureType(int pct);

    void setNetBuffer(YangRtcPublish *prr);
    void startPubVideo();
    void startPubAudio();
	void initAudioEncoding();
	void initVideoEncoding();
	void setVideoInfo(YangVideoInfo* pvideo);
	void startAudioCapture();
	void startVideoCapture();
	void startScreenCapture();

	void initVideoMeeting();
	void startAudioEncoding();
	void startVideoEncoding();
	void deleteVideoEncoding();
	void startAudioCaptureState();
	void startVideoCaptureState();
	void startScreenCaptureState();
	void stopAudioCaptureState();
	void stopVideoCaptureState();
	void stopScreenCaptureState();
	YangVideoBuffer * getPreVideoBuffer();
	YangVideoBuffer * getScreenPreVideoBuffer();
	YangVideoBuffer * getScreenOutVideoBuffer();
	void stopAll();
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf);
	void change(int32_t st);
	void addVr();
	void delVr();
	void sendRequest(int32_t puid,uint32_t ssrc,YangRequestType req);
	void sendKeyframe();
    protected:

    private:
	YangContext *m_context;
	YangPushEncoder *m_encoder;
	YangPushCapture *m_capture;
	YangVideoInfo* m_videoInfo;
	 int32_t isStartAudioCapture,isStartVideoCapture,isStartScreenCapture;
	 int32_t isStartAudioEncoder,isStartVideoEncoder;
	void stopAudioState();
	void stopVideoState();
	void initCapture();
	int m_captureType;
};

#endif //
