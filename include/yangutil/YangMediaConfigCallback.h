#ifndef INCLUDE_YANGUTIL_YANGMEDIACONFIGCALLBACK_H_
#define INCLUDE_YANGUTIL_YANGMEDIACONFIGCALLBACK_H_
#include <yangstream/YangStreamType.h>
#include <yangutil/yangavinfotype.h>
enum YangRequestType{
	Yang_Req_Sendkeyframe,
	Yang_Req_Connected,
	Yang_Req_Disconnected
};
class YangMediaConfigCallback{
public:
	YangMediaConfigCallback(){};
	virtual ~YangMediaConfigCallback(){};
	virtual void setMediaConfig(int32_t puid,YangAudioParam* audio,YangVideoParam* video)=0;
};
class YangSendRequestCallback{
public:
	YangSendRequestCallback(){};
		virtual ~YangSendRequestCallback(){};
		virtual void sendRequest(int32_t puid,uint32_t ssrc,YangRequestType req)=0;
};


#endif /* INCLUDE_YANGUTIL_YANGMEDIACONFIGCALLBACK_H_ */
