#include "YangPushMessageHandle.h"
#include <yangutil/yang_unistd.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSocket.h>
#include <yangpush/YangPushCommon.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>









YangPushMessageHandle::YangPushMessageHandle(bool hasAudio,int pvideotype,YangVideoInfo* pvideo,YangContext* pcontext,YangSysMessageI* pmessage,YangSysMessageHandleI* pmessageHandle) {

//	m_receive = prec;
	m_context=pcontext;
	m_receive= pmessageHandle;
    m_push = new YangPushHandleImpl(hasAudio,false,pvideotype,pvideo,pcontext,pmessage);

}

YangPushMessageHandle::~YangPushMessageHandle() {
	deleteAll();


}
void YangPushMessageHandle::initAll(){

}
	void YangPushMessageHandle::deleteAll(){

			m_context=NULL;
			yang_delete(m_push);

	}

void YangPushMessageHandle::handleMessage(YangSysMessage *mss) {

	int32_t uid = mss->uid;
	int32_t ret = Yang_Ok;

	switch (mss->messageId) {
	case YangM_Push_StartVideoCapture:
	{
		//m_push->init();
		m_push->switchToCamera(false);
		break;
	}
	case YangM_Push_StartScreenCapture:
	{
		//m_push->init();
		m_push->switchToScreen(false);
		break;
	}
	case YangM_Push_Publish_Start:	
        if(mss->user&&m_push) ret = m_push->publish((char*)mss->user,yang_getLocalInfo().c_str(),m_context->sys.rtcLocalPort);
		break;
	case YangM_Push_Publish_Stop:
            if(m_push)  m_push->disconnect();
			break;
	case YangM_Push_SwitchToCamera:
            if(m_push) m_push->switchToCamera(false);
			break;
	case YangM_Push_SwitchToScreen:
            if(m_push)  m_push->switchToScreen(false);
			break;


	}

	if (mss->handle) {
		if (ret)
			mss->handle->failure(ret);
		else
			mss->handle->success();
	}

	if (m_receive)
			m_receive->receiveSysMessage(mss,ret);
}

