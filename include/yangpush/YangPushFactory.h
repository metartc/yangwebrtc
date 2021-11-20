#ifndef INCLUDE_YANGPUSH_YANGPUSHFACTORY_H_
#define INCLUDE_YANGPUSH_YANGPUSHFACTORY_H_
#include <yangpush/YangPushHandle.h>
#include <yangutil/sys/YangSysMessageHandle.h>
class YangPushFactory {
public:
	YangPushFactory();
	virtual ~YangPushFactory();
	YangPushHandle* createPushHandle(bool hasAudio,bool initVideo,int pvideotype,YangVideoInfo* pvideo,YangContext* pcontext,YangSysMessageI* pmessage);
        YangSysMessageHandle* createPushMessageHandle(bool hasAudio,bool initVideo,int pvideotype,YangVideoInfo* pvideo,YangContext* pcontext,YangSysMessageI* pmessage,YangSysMessageHandleI* pmessagehandle);
	YangVideoBuffer* getPreVideoBuffer(YangSysMessageHandle* pmessageHandle);
};

#endif /* INCLUDE_YANGPUSH_YANGPUSHFACTORY_H_ */
