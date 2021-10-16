#ifndef YANGAPP_YangPushCapture_H_
#define YANGAPP_YangPushCapture_H_
#include <yangaudiodev/YangAudioCapture.h>
#include <yangutil/sys/YangThread.h>
#include <yangcapture/YangVideoCapture.h>
#include <yangutil/buffer/YangAudioBuffer.h>
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangavutil/audio/YangAecBase.h>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
class YangPushCapture :public YangThread {
public:
	YangPushCapture(YangContext *pcontext);
	virtual ~YangPushCapture();
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
	YangVideoCapture *m_videoCapture;

	YangContext *m_context;
	YangAudioBuffer *m_out_audioBuffer;
	YangAudioCapture *m_audioCapture;
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
public:
	int32_t initAudio(YangPreProcess *pp);
	void startAudioCapture();
	YangAudioBuffer* getOutAudioBuffer();
	void stopAudioCaptureState();
	void startAudioCaptureState();
	void setAec(YangAecBase *paec);
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf);
};

#endif /* YANGAPP_YANGCAPTUREAPP_H_ */
