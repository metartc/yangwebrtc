/*
 * YangSrtBase.h
 *
 *  Created on: 2020年8月4日
 *      Author: yang
 */

#ifndef YANGSRTBASE_H_
#define YANGSRTBASE_H_
#include "stdint.h"
#ifdef __WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#endif
#include <unistd.h>
#include "srt/srt.h"
#include "yangutil/YangErrorCode.h"
#include "yangutil/sys/YangLoadLib.h"
//enum YangSrtStatus{
//	   YSRTS_CONNECTING,
//	   SRTS_CONNECTED,
//	   SRTS_BROKEN,
//	   SRTS_CLOSING,
	//   SRTS_CLOSED,
//};
#define YangSrtBase_strlen 128
#define YangSrtUnitLen 188
#define Yang_Srt_CacheSize 4096
class YangSrtBase {
public:
	YangSrtBase();
	virtual ~YangSrtBase();
	int init(char *pserver,int pport);
	int publish(char* message,int len);
	int receive(char *p,int *len);
	//int receive2(char *p,int *len);
	void startLoop();
	void closeSrt();
	int initConnect(char *streamId);
	int connectServer();
	int getSrtSocketStatus();
#ifdef _WIN32
	int yang_inet_pton(int af, const char * src, void * dst);
#endif
	int64_t get_bitrate();
	int m_errState;
	int m_init;
private:
	int m_port;
	int m_sfd;
	int m_eid;
	char m_server[YangSrtBase_strlen];
	int64_t m_dataCount;
	int64_t m_bitRate;
	int64_t m_beginTm;

	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
	int (*yang_srt_epoll_create)(void);
	int (*yang_srt_epoll_remove_usock)(int eid, SRTSOCKET u);
	int (*yang_srt_epoll_release)(int eid);
	int (*yang_srt_close)        (SRTSOCKET u);
	int (*yang_srt_cleanup)(void);
	 int (*yang_srt_startup)(void);
	 SRT_SOCKSTATUS (*yang_srt_getsockstate)(SRTSOCKET u);
	 void (*yang_srt_setloglevel)(int ll);
	 SRTSOCKET (*yang_srt_create_socket)(void);
	 int32_t (*yang_srt_epoll_set)(int eid, int32_t flags);
	 int (*yang_srt_epoll_add_usock)(int eid, SRTSOCKET u, const int* events);
	 int (*yang_srt_setsockopt)   (SRTSOCKET u, int level /*ignored*/, SRT_SOCKOPT optname, const void* optval, int optlen);
	 int (*yang_srt_setsockflag)  (SRTSOCKET u, SRT_SOCKOPT opt, const void* optval, int optlen);
	 int (*yang_srt_connect )     (SRTSOCKET u, const struct sockaddr* name, int namelen);
	 int (*yang_srt_epoll_wait)(int eid, SRTSOCKET* readfds, int* rnum, SRTSOCKET* writefds, int* wnum, int64_t msTimeOut,
	                            SYSSOCKET* lrfds, int* lrnum, SYSSOCKET* lwfds, int* lwnum);
	 int (*yang_srt_sendmsg) (SRTSOCKET u, const char* buf, int len, int ttl/* = -1*/, int inorder/* = false*/);
	 int (*yang_srt_recvmsg) (SRTSOCKET u, char* buf, int len);
	 int  (*yang_srt_getlasterror)(int* errno_loc);
	 const char* (*yang_srt_getlasterror_str)(void);
};

#endif /* YANGSRTBASE_H_ */
