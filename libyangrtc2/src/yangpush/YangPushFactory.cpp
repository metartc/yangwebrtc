
#include <yangpush/YangPushFactory.h>
#include "YangPushMessageHandle.h"
YangPushFactory::YangPushFactory() {


}

YangPushFactory::~YangPushFactory() {

}

YangPushHandle* YangPushFactory::createPushHandle(bool hasAudio,bool initVideo,int pvideotype, YangVideoInfo *pvideo, YangContext *pcontext,
		YangSysMessageI *pmessage) {
	return new YangPushHandleImpl(hasAudio,initVideo,pvideotype,pvideo,pcontext, pmessage);
}

YangSysMessageHandle* YangPushFactory::createPushMessageHandle(bool hasAudio,bool initVideo,
		int pvideotype, YangVideoInfo *pvideo, YangContext *pcontext,
        YangSysMessageI *pmessage,YangSysMessageHandleI* pmessagehandle) {
    return new YangPushMessageHandle(hasAudio,pvideotype,pvideo,pcontext, pmessage,pmessagehandle);
}

YangVideoBuffer* YangPushFactory::getPreVideoBuffer(YangSysMessageHandle* pmessageHandle){
	if(!pmessageHandle) return NULL;
	YangPushMessageHandle* mess=dynamic_cast<YangPushMessageHandle*>(pmessageHandle);
	if(mess&&mess->m_push) return mess->m_push->getPreVideoBuffer();
	return NULL;

}
