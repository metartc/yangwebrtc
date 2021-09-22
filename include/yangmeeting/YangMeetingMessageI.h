#ifndef INCLUDE_YANGMEETING_YANGMEETINGMESSAGEI_H_
#define INCLUDE_YANGMEETING_YANGMEETINGMESSAGEI_H_
#include "yangutil/sys/YangSysMessageI.h"
enum YangSysMessageType {
	YangM_Sys_Shutdown,
	YangM_Sys_ConnectServer,
	YangM_Sys_DisconnectServer,
	YangM_Sys_ConnectServerInterrupt,

	YangM_Sys_PushMediaServerConnect,
	YangM_Sys_PlayMediaServerConnect,
	YangM_Sys_PushMediaServerError,
	YangM_Sys_PlayMediaServerError,

	YangM_Sys_Login,
	YangM_Sys_Logout,  //9
	YangM_Sys_GetMeetinginfo,
	YangM_Sys_ShowMessage,
	YangM_Sys_Setvr,
	YangM_Sys_UnSetvr,
	YangM_Sys_Pubaudio,
	YangM_Sys_Pubvideo,
	YangM_Sys_StartAudioCapture,
	YangM_Sys_StartVideoCapture,
	YangM_Sys_UnPubaudio,
	YangM_Sys_UnPubvideo,
	YangM_Sys_Playaudio,
	YangM_Sys_Playvideo,
	YangM_Sys_UnPlayaudio,
	YangM_Sys_UnPlayvideo,

	YangM_Sys_NofityMediaParam,
	YangM_Sys_SendKeyframe
};

class YangReceiveMessageI {
public:
	YangReceiveMessageI() {
	};
	virtual ~YangReceiveMessageI() {
	};
	virtual void receiveError(int32_t pet)=0;
	virtual void receiveMessage(char *pstr)=0;
	virtual void receiveSysMessage(YangSysMessage *psm, int32_t phandleRet)=0;
};
//void yang_post_message(YangSysMessageType st, int32_t uid,YangSysMessageI *mhandle);
//void yang_post_message1(YangSysMessageType st, int32_t uid, int32_t handleState,YangSysMessageI *mhandle);
#endif /* INCLUDE_YANGMEETING_YANGMEETINGMESSAGEI_H_ */
