#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <yangmeeting/src/YangMeetingHandleImpl.h>
#include <yangmeeting/src/YangMeetingMessageHandle.h>
#include <yangutil/yangavinfotype.h>
#include "yangutil/sys/YangLog.h"
#include "yangmeeting/YangMeetingFactory.h"



YangMeetingMessageHandle::YangMeetingMessageHandle(YangMeetingContext* pini,YangReceiveMessageI* prec) {

	m_receive = prec;
	m_ini=pini;
	m_videoMeeting = new YangMeetingHandleImpl(m_ini);

}

YangMeetingMessageHandle::~YangMeetingMessageHandle() {



}
void YangMeetingMessageHandle::initAll(){

}
	void YangMeetingMessageHandle::deleteAll(){
		if(m_videoMeeting){
			m_receive = NULL;
				m_ini=NULL;
			yang_delete(m_videoMeeting);
		}
	}

void YangMeetingMessageHandle::handleMessage(YangSysMessage *mss) {

	int32_t uid = mss->uid;
	int32_t ret = Yang_Ok;

	switch (mss->messageId) {
	case YangM_Sys_Shutdown:
		stopLoop();
		break;
	case YangM_Sys_ConnectServer:
		if(m_videoMeeting) ret = m_videoMeeting->connectServer();
		break;
	case YangM_Sys_DisconnectServer:
		if(m_videoMeeting) ret = m_videoMeeting->disconnectServer();
		break;
	case YangM_Sys_ConnectServerInterrupt:

		break;
	case YangM_Sys_PushMediaServerConnect:
		if(m_videoMeeting) ret = m_videoMeeting->connectPushServer(uid);
		break;
	case YangM_Sys_PlayMediaServerConnect:
		if(m_videoMeeting) ret = m_videoMeeting->connectPlayServer(uid);
		break;
	case YangM_Sys_PushMediaServerError:
		break;
	case YangM_Sys_PlayMediaServerError:
		break;
	case YangM_Sys_Login:
		if(mss->uid==m_ini->user.userId||mss->uid==0){
			if(m_videoMeeting) m_videoMeeting->loginServer(mss->handleState);
			ret=mss->handleState;
		}else
			if(m_videoMeeting) m_videoMeeting->login(uid);
		break;
	case YangM_Sys_Logout:
		if(m_videoMeeting) m_videoMeeting->logout(mss->uid);
		break;
	case YangM_Sys_GetMeetinginfo:
		ret=Yang_Ok;
		break;
	case YangM_Sys_ShowMessage:
		if(m_videoMeeting->m_message&&m_videoMeeting->m_message->getMessageState()==0)  m_videoMeeting->m_message->startMessage();
		break;
	case YangM_Sys_Setvr:
		ret =Yang_Ok;
		if(m_videoMeeting) m_videoMeeting->addVr();
		break;
	case YangM_Sys_UnSetvr:
		ret = Yang_Ok;
		if(m_videoMeeting) m_videoMeeting->delVr();
		break;
	case YangM_Sys_Pubaudio:
		if(mss->uid==m_ini->user.userId||mss->uid==0)
			if(m_videoMeeting) ret = m_videoMeeting->pushAudio();
		break;
	case YangM_Sys_Pubvideo:
		if(mss->uid==m_ini->user.userId||mss->uid==0)
			if(m_videoMeeting) ret = m_videoMeeting->pushVideo();
		break;
	case YangM_Sys_StartAudioCapture:
			if(m_videoMeeting) m_videoMeeting->startAudioCaptureState();
		break;
	case YangM_Sys_StartVideoCapture:
			if(m_videoMeeting) m_videoMeeting->startVideoCaptureState();
		break;

	case YangM_Sys_UnPubaudio:
		if(mss->uid==m_ini->user.userId||mss->uid==0)
			if(m_videoMeeting) ret = m_videoMeeting->unPushAudio();

		break;
	case YangM_Sys_UnPubvideo:
		if(mss->uid==m_ini->user.userId||mss->uid==0)
			if(m_videoMeeting) ret = m_videoMeeting->unPushVideo();

		break;
	case YangM_Sys_Playaudio:
		if(m_videoMeeting) ret = m_videoMeeting->playAudio(uid);
		break;
	case YangM_Sys_Playvideo:
		 if(m_videoMeeting) ret = m_videoMeeting->playVideo(uid);
		break;
	case YangM_Sys_UnPlayaudio:
		if(m_videoMeeting) ret = m_videoMeeting->unPlayAudio(uid);
		break;
	case YangM_Sys_UnPlayvideo:
		if(m_videoMeeting) ret = m_videoMeeting->unPlayVideo(uid);
		break;
	case YangM_Sys_NofityMediaParam:
		if(m_videoMeeting) m_videoMeeting->notifyMediaSource(uid);
		break;
	case YangM_Sys_SendKeyframe:
			if(m_videoMeeting) m_videoMeeting->sendKeyframe();
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

