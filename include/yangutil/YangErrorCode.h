/*
 * YangErrorCode.h
 *
 *  Created on: 2019年3月4日
 *      Author: yang
 */

#ifndef INCLUDE_YANGUTIL_YANGERRORCODE_H_
#define INCLUDE_YANGUTIL_YANGERRORCODE_H_

#define Yang_Ok 0
#define Yang_LeaveRoom 1


#define Yang_Sys_NoAudioDevice 110
#define Yang_Sys_NoVideoDevice 111
#define Yang_Sys_NoAudioCaptureDevice 112
#define Yang_Sys_NoAudioPlayDevice 113

#define Yang_Sys_Linux_VideoDeveceOpenFailure 120
#define Yang_Sys_Linux_NoVideoDriver 122
#define Yang_Sys_Linux_NoVideoCatpureInterface 124

#define Yang_Sys_Win_VideoDeveceOpenFailure 150
#define Yang_Sys_Win_NoVideoDriver 151
#define Yang_Sys_Win_NoVideoCatpureInterface 152

#define Yang_Sys_Linux_ScreenDeviceOpenFailure 163
#define Yang_Sys_Linux_ScreenCaptureFailure 164
#define Yang_Sys_Win_ScreenCaptureFailure 165


#define Yang_Err_Socket 201
#define Yang_Err_Socket_Timeout 202
#define Yang_Err_Socket_Close 203
#define Yang_Err_Socket_Close_Wr 204
#define Yang_Err_Socket_Negotiation 205
#define Yang_Err_Connect_Handshaked 206
#define Yang_Err_Connect_Fail 207
#define Yang_Err_Request_Close 208
#define Yang_Err_Server_Reject 209
#define Yang_Err_Server_Logout 210
#define Yang_Err_Server_ConnectFailure 211





#define Yang_Err_Rtmp_ConnectFailure 2100
#define Yang_Err_Rtmp_PubFailure 2101
#define Yang_Err_Rtmp_UnPubFailure 2102
#define Yang_Err_Rtmp_PlayFailure 2103
#define Yang_Err_Rtmp_UnPlayFailure 2104
#define Yang_Err_Rtmp_SendCommandFailure 2105
#define Yang_Err_Rtmp_UserInfosResult 2106

#define Yang_SRTS_SocketBase 3000
#define Yang_Err_Srt_NotInit 3001
#define Yang_SRTS_CONNECTING 3004
#define Yang_SRTS_BROKEN 3006
#define Yang_SRTS_CLOSING 3007
#define Yang_SRTS_CLOSED 3008
#define Yang_SRTS_NONEXIST 3009
#define Yang_SRTS_NONSRTSERVER 3010
#define Yang_Err_Srt_PushFailure 3100
#define Yang_Err_Srt_PullFailure 3101
#define Yang_Err_Srt_StreamIdSetFailure 3102
#define Yang_Err_Srt_EpollCreateFailure 3103
#define Yang_Err_Srt_EpollSetFailure 3104
#define Yang_Err_Srt_EpollSelectFailure 3105
#define Yang_Err_Srt_SocketConnectCreate 3201
#define Yang_Err_Srt_SocketConnect 3202
#define Yang_Err_Srt_WriteSocket 3210
#define Yang_Err_Srt_ReadSocket 3211

//char* yang_getErrorStr(int errcode);


#endif /* INCLUDE_YANGUTIL_YANGERRORCODE_H_ */
