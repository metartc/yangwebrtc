#ifndef YANGPUSH_YANGPUSHHANDLEIMPL_H_
#define YANGPUSH_YANGPUSHHANDLEIMPL_H_
#include <yangpush/YangPushPublish.h>
#include <yangpush/YangPushHandle.h>
#include <yangpush/YangRtcPublish.h>
#include <yangutil/sys/YangUrl.h>

class YangPushHandleImpl :public YangPushHandle{
public:
	YangPushHandleImpl(YangContext* pcontext,YangSysMessageI* pmessage);
	virtual ~YangPushHandleImpl();
	void init();
	void startCapture();
    int publish(string url,string localIp,int32_t localport);
	void setScreenVideoInfo(YangVideoInfo* pvideo);
	void setScreenInterval(int32_t pinterval);
	YangVideoBuffer* getPreVideoBuffer();

	void disconnect();
	void switchToCamera();
	void switchToScreen();
	  void setDrawmouse(bool isDraw);

private:
    void startCamera();
    void startScreen();
    void stopCamera();
    void stopScreen();
	void stopPublish();

private:
	YangPushPublish* m_cap;
	YangRtcPublish* m_pub;
	YangContext* m_context;
	YangUrlData m_url;
	YangSysMessageI* m_message;
	int m_videoState;
	YangVideoInfo* m_screenInfo;
	bool m_hasAudio;


};

#endif /* YANGPUSH_YANGPUSHHANDLEIMPL_H_ */
