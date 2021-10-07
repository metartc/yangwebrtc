#ifndef YANGPUSH_YANGPUSHHANDLEIMPL_H_
#define YANGPUSH_YANGPUSHHANDLEIMPL_H_
#include <yangpush/YangPushPublish.h>
#include <yangpush/YangPushHandle.h>
#include <yangpush/YangRtcPublish.h>
#define Yang_Rtmp 0
#define Yang_Srt 1
#define Yang_Webrtc 2
struct YangPublishUrlType{
	int32_t netType;
	int32_t port;
	string server;
	string app;
	string stream;
};
class YangPushHandleImpl :public YangPushHandle{
public:
	YangPushHandleImpl(YangContext* pcontext);
	virtual ~YangPushHandleImpl();
	void init();
	void startCapture();
	void publish(string url,string localIp,int32_t localport);
	YangVideoBuffer* getPreVideoBuffer();
	void disconnect();
private:
	void stopPublish();
	int32_t parse(std::string purl);
private:
	YangPushPublish* m_cap;
	YangRtcPublish* m_pub;
	YangContext* m_context;
	YangPublishUrlType m_url;


};

#endif /* YANGPUSH_YANGPUSHHANDLEIMPL_H_ */
