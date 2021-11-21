#ifndef YANGAPP_YangPushCapture_H_
#define YANGAPP_YangPushCapture_H_
#include <yangaudiodev/YangAudioCapture.h>
#include <yangutil/sys/YangThread.h>
#include <yangcapture/YangMultiVideoCapture.h>
#include <yangcapture/YangScreenCapture.h>
#include <yangutil/buffer/YangAudioBuffer.h>
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangavutil/audio/YangAecBase.h>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
class YangPushCapture :public YangThread {
public:
	YangPushCapture(YangContext *pcontext);
	virtual ~YangPushCapture();
public:
	 void startCamera();
	    void startScreen();
	    void stopCamera();
	    void stopScreen();
	    void setScreenInterval(int32_t pinterval);
	    void setDrawmouse(bool isDraw);
	void startVideoCaptureState();
	void startScreenCaptureState();
	void stopVideoCaptureState();
	void stopScreenCaptureState();
	int32_t initVideo();
	int32_t initScreen();
	void startVideoCapture();
	void startScreenCapture();
	YangVideoBuffer * getOutVideoBuffer();
	YangVideoBuffer * getPreVideoBuffer();

	YangVideoBuffer * getScreenOutVideoBuffer();
	YangVideoBuffer * getScreenPreVideoBuffer();
	void stopAll();
	void change(int32_t st);
	void addVr();
	void delVr();

private:
	YangVideoBuffer *m_out_videoBuffer;
	YangVideoBuffer *m_pre_videoBuffer;
	YangMultiVideoCapture *m_videoCapture;
	YangVideoBuffer *m_screen_pre_videoBuffer;
	YangVideoBuffer *m_screen_out_videoBuffer;
	YangScreenCapture* m_screenCapture;

	YangContext *m_context;
	YangAudioBuffer *m_out_audioBuffer;
	YangAudioCapture *m_audioCapture;
public:
	void stop();
	int32_t m_isStart;
protected:
	void run();
	void startLoop(){};
	void stopLoop();
	void initVr();

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
