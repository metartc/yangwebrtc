/*
 * YangLibRtmp.h
 *
 *  Created on: 2019年9月2日
 *      Author: yang
 */

#ifndef YANGRTMP_IMPL_YANGRTMPBASE_H_
#define YANGRTMP_IMPL_YANGRTMPBASE_H_
#include <errno.h>
#include <vector>

#include "yangutil/sys/YangAmf.h"
#include "yangutil/yangtype.h"
//#include "yangutil/YangRtmpInterface.h"
#include "yangutil/sys/YangLog.h"
#include <yangstream/YangStreamType.h>
using namespace std;

#define RTMP_LIB_VERSION	0x020300	/* 2.3 */
#define RTMP_FEATURE_HTTP	0x01
#define RTMP_FEATURE_ENC	0x02
#define RTMP_FEATURE_SSL	0x04
#define RTMP_FEATURE_MFP	0x08	/* not yet supported */
#define RTMP_FEATURE_WRITE	0x10	/* publish, not play */
#define RTMP_FEATURE_HTTP2	0x20	/* server-side rtmpt */
#define RTMP_PROTOCOL_UNDEFINED	-1
#define RTMP_PROTOCOL_RTMP      0
#define RTMP_PROTOCOL_RTMPE     RTMP_FEATURE_ENC
#define RTMP_PROTOCOL_RTMPT     RTMP_FEATURE_HTTP
#define RTMP_PROTOCOL_RTMPS     RTMP_FEATURE_SSL
#define RTMP_PROTOCOL_RTMPTE    (RTMP_FEATURE_HTTP|RTMP_FEATURE_ENC)
#define RTMP_PROTOCOL_RTMPTS    (RTMP_FEATURE_HTTP|RTMP_FEATURE_SSL)
#define RTMP_PROTOCOL_RTMFP     RTMP_FEATURE_MFP
#define RTMP_DEFAULT_CHUNKSIZE	128
#define	RTMP_CHANNELS	65600
/*      RTMP_PACKET_TYPE_...                0x00 */
#define RTMP_PACKET_TYPE_CHUNK_SIZE         0x01
/*      RTMP_PACKET_TYPE_...                0x02 */
#define RTMP_PACKET_TYPE_BYTES_READ_REPORT  0x03
#define RTMP_PACKET_TYPE_CONTROL            0x04
#define RTMP_PACKET_TYPE_SERVER_BW          0x05
#define RTMP_PACKET_TYPE_CLIENT_BW          0x06
#define RTMP_PACKET_TYPE_AUDIO              0x08
#define RTMP_PACKET_TYPE_VIDEO              0x09
/*      RTMP_PACKET_TYPE_...                0x07 */

/*      RTMP_PACKET_TYPE_...                0x0A */
/*      RTMP_PACKET_TYPE_...                0x0B */
/*      RTMP_PACKET_TYPE_...                0x0C */
/*      RTMP_PACKET_TYPE_...                0x0D */
/*      RTMP_PACKET_TYPE_...                0x0E */
#define RTMP_PACKET_TYPE_FLEX_STREAM_SEND   0x0F
#define RTMP_PACKET_TYPE_FLEX_SHARED_OBJECT 0x10
#define RTMP_PACKET_TYPE_FLEX_MESSAGE       0x11
#define RTMP_PACKET_TYPE_INFO               0x12
#define RTMP_PACKET_TYPE_SHARED_OBJECT      0x13
#define RTMP_PACKET_TYPE_INVOKE             0x14
/*      RTMP_PACKET_TYPE_...                0x15 */
#define RTMP_PACKET_TYPE_FLASH_VIDEO        0x16
#define RTMP_PACKET_SIZE_LARGE    0
#define RTMP_PACKET_SIZE_MEDIUM   1
#define RTMP_PACKET_SIZE_SMALL    2
#define RTMP_PACKET_SIZE_MINIMUM  3
/* needs to fit largest number of bytes recv() may return */
#define RTMP_BUFFER_CACHE_SIZE (16*1024)
#define RTMP_MAX_HEADER_SIZE 18
#define RTMP_MAX_BODY_SIZE 512000
#define RTMP_SIG_SIZE 1536
#define RTMP_LARGE_HEADER_SIZE 12

#define RTMPPacket_IsReady(a)	((a)->m_nBytesRead == (a)->m_nBodySize)

#define GetSockError()	errno
#define SetSockError(e)	errno = e
#undef closesocket
#ifdef _WIN32

#else
#define closesocket(s)	close(s)
#endif
#define msleep(n)	usleep(n*1000)
#define SET_RCVTIMEO(tv,s)	struct timeval tv = {s,0}
//#endif



#define SAVC(x)	 const AVal av_##x = AVC(#x)
#define HEX2BIN(a)	(((a)&0x40)?((a)&0xf)+9:((a)&0xf))
#define MAX_IGNORED_FRAMES	50
#define HEADERBUF	(128*1024)
#define OFF(x)	offsetof(struct RTMP,x)


enum {
	OPT_STR = 0, OPT_INT, OPT_BOOL, OPT_CONN
};
struct RTMPChunk {
	int c_headerSize;
	int c_chunkSize;
	char *c_chunk;
	char c_header[RTMP_MAX_HEADER_SIZE];
};

struct RTMPPacket {
	uint8_t m_headerType;
	uint8_t m_packetType;
	uint8_t m_hasAbsTimestamp; /* timestamp absolute or relative? */
	int m_nChannel;
	uint32_t m_nTimeStamp; /* timestamp */
	int32_t m_nInfoField2; /* last 4 bytes in a long header */
	uint32_t m_nBodySize;
	uint32_t m_nBytesRead;
	RTMPChunk *m_chunk;
	char *m_body;
};

struct RTMPSockBuf {
	int sb_socket;
	int sb_size; /* number of unprocessed bytes in buffer */
	char *sb_start; /* pointer into sb_pBuffer of next byte to process */
	char sb_buf[RTMP_BUFFER_CACHE_SIZE]; /* data read from socket */
	int sb_timedout;
	void *sb_ssl;
};

struct RTMP_LNK {
	AVal hostname;
	AVal sockshost;
	AVal curPath;
	AVal curPatht;
	char streamType;
	AVal tcUrl;
	AVal swfUrl;
	AVal pageUrl;
	AVal app;
	AVal auth;
	AVal flashVer;
	AVal subscribepath;
	AVal usherToken;
	AVal token;
	AVal pubUser;
	AVal pubPasswd;
	AMFObject extras;
	int edepth;

	int seekTime;
	int stopTime;

#define RTMP_LF_AUTH	0x0001	/* using auth param */
//#define RTMP_LF_LIVE	0x0002	/* stream is live */
#define RTMP_LF_SWFV	0x0004	/* do SWF verification */
#define RTMP_LF_PLST	0x0008	/* send playlist before play */
#define RTMP_LF_BUFX	0x0010	/* toggle stream on BufferEmpty msg */
#define RTMP_LF_FTCU	0x0020	/* free tcUrl on close */
#define RTMP_LF_FAPU	0x0040	/* free app on close */
	int lFlags;

	int swfAge;

	int protocol;
	int timeout; /* connection timeout in seconds */

	int pFlags; /* unused, but kept to avoid breaking ABI */

	unsigned short socksport;
	unsigned short port;
};
struct RTMP_READ {
	char *buf;
	char *bufpos;
	unsigned int buflen;
	uint32_t timestamp;
	uint8_t dataType;
	uint8_t flags;
#define RTMP_READ_HEADER	0x01
#define RTMP_READ_RESUME	0x02
#define RTMP_READ_NO_IGNORE	0x04
#define RTMP_READ_GOTKF		0x08
#define RTMP_READ_GOTFLVK	0x10
#define RTMP_READ_SEEKING	0x20
	int8_t status;
#define RTMP_READ_COMPLETE	-3
#define RTMP_READ_ERROR	-2
#define RTMP_READ_EOF	-1
#define RTMP_READ_IGNORE	0

	/* if bResume == TRUE */
	uint8_t initialFrameType;
	uint32_t nResumeTS;
	char *metaHeader;
	char *initialFrame;
	uint32_t nMetaHeaderSize;
	uint32_t nInitialFrameSize;
	uint32_t nIgnoredFrameCounter;
	uint32_t nIgnoredFlvFrameCounter;
};

struct RTMP_METHOD {
	AVal name;
	int num;
};

struct YangNetStatus{
	char code[64];
	char description[64];
	char fmsVer[64];
};

struct RTMP {
	int m_inChunkSize;
	int m_outChunkSize;
	int m_nBWCheckCounter;
	int m_nBytesIn;
	int m_nBytesInSent;
	int m_nBufferMS;
	int m_stream_id; /* returned in _result from createStream */
	int m_mediaChannel;
	uint32_t m_mediaStamp;
	uint32_t m_pauseStamp;
	int m_pausing;
	int m_nServerBW;
	int m_nClientBW;
	uint8_t m_nClientBW2;
	uint8_t m_bPlaying;
	uint8_t m_bSendEncoding;
	uint8_t m_bSendCounter;
	uint8_t m_bConnected;

	int m_numInvokes;
	int m_numCalls;
	RTMP_METHOD *m_methodCalls; /* remote method calls queue */

	int m_channelsAllocatedIn;
	int m_channelsAllocatedOut;
	RTMPPacket **m_vecChannelsIn;
	RTMPPacket **m_vecChannelsOut;
	int *m_channelTimestamp; /* abs timestamp of last packet */

	double m_fAudioCodecs; /* audioCodecs for the connect packet */
	double m_fVideoCodecs; /* videoCodecs for the connect packet */
	double m_fEncoding; /* AMF0 or AMF3 */

	double m_fDuration; /* duration of stream in seconds */

	int m_isPublish;

	int m_msgCounter; /* RTMPT stuff */
	int m_polling;
	int m_resplen;
	int m_unackd;
	AVal m_clientID;

	RTMP_READ m_read;
	RTMPPacket m_write;
	RTMPSockBuf m_sb;
	RTMP_LNK Link;
	YangNetStatus netStatus;
};

class YangRtmpBase {
public:
	YangRtmpBase();
	virtual ~YangRtmpBase();
	//YangLog *m_log;
	RTMP *r;
	int m_errState;
	//virtual void netStatusHandler(char *code);
	void setPublishState(int p_pubstate);
	int connectServer();
	int HandleStream(char *psName,YangStreamOptType isPublish,int puid);
	int connectRtmpServer(YangStreamOptType pisPublish,char *serverIp,char *app,int port);
	void initRTMP(char *serverIp,char *app,int port);
	void initRTMP(char *serverIp,char *app,int meetingId,int port);
	int initConnect(RTMPPacket *cp);
	void setBufferMS(int size);
	int readPacket(RTMPPacket *packet);
	int yangReadPacket(RTMPPacket *packet,char* pszbody);
	int sendPacket(RTMPPacket *packet, int queue);
	int yangSendPacket(RTMPPacket *packet, int queue);
	int clientPacket(RTMPPacket *packet);
	int clientPacket_conn(RTMPPacket *packet);
	void freePacket(RTMPPacket *packet);
	int RTMP_SetupURL( char *url);
	int RTMP_SetOpt(const AVal *opt, AVal *arg);
	int RTMP_Connect0(struct sockaddr *svc);
	int RTMP_Connect1(RTMPPacket *cp);
	int RTMP_Serve();
	int RTMP_SendChunk(RTMPChunk *chunk);
	int RTMP_Socket();
	int RTMP_ToggleStream();
	void RTMP_DeleteStream();
	void RTMP_Init();
	void RTMP_Close();
	void deleteR();
	void RTMP_Free();
	int RTMP_LibVersion(void);
	void RTMP_UserInterrupt(void);
	int RTMP_SendCtrl(short nType, unsigned int nObject, unsigned int nTime);
	int RTMP_SendCtrlPong(short nType, char *p,int len);
	int RTMP_SendPause(int DoPause, int dTime);
	int RTMP_Pause(int DoPause);
	int RTMP_SendCreateStream();
	int RTMP_SendSeek(int dTime);
	int RTMP_SendServerBW();
	int RTMP_SendClientBW();
	void RTMP_DropRequest(int i, int freeit);
	int RTMP_Read(char *buf, int size);
	int RTMP_Write(const char *buf, int size);
	int RTMP_HashSWF(const char *url, unsigned int *size, unsigned char *hash,int age);
	int SendFCUnpublish();
	int SendDeleteStream(double dStreamId);
	int SendCloseStream(double dStreamId);
	int RTMP_ctrlC;
	char pathChar[30];
	//int m_streamState;
	//int m_isHandleStream;
protected:
	void yang_handleconn(AMFObject *pob);
	uint32_t RTMP_GetTime();
	void RTMP_OptUsage();
	//void RTMP_UserInterrupt();
	//void RTMPPacket_Reset(RTMPPacket *p);
	int RTMPSockBuf_Close(RTMPSockBuf *sb);
	int RTMPSockBuf_Send(RTMPSockBuf *sb, const char *buf, int len) ;
	void HandleClientBW(const RTMPPacket *packet);
	void HandleServerBW(const RTMPPacket *packet);
	void HandleChangeChunkSize(RTMP *r, const RTMPPacket *packet);
	int HandleMetadata(char *body, unsigned int len);
	int DumpMetaData(AMFObject *obj);
	void RTMPPacket_Dump(RTMPPacket *p);
	void RTMPPacket_Reset(RTMPPacket *p);
	void SocksSetup(AVal *sockshost);
	int RTMPPacket_Alloc(RTMPPacket *p, uint32_t nSize);
	void RTMPPacket_Free(RTMPPacket *p);
	int RTMP_IsConnected();
	void CloseInternal(int reconnect);
	int add_addr_info(struct sockaddr_in *service, AVal *host, int port);
	void handleError(int perrCode);
	int SocksNegotiate();
	int ReadN(char *buffer, int n);
	int WriteN(const char *buffer, int n);
	int SendConnectPacket(RTMPPacket *cp);
	int SendFCSubscribe(AVal *subscribepath);
	int SendUsherToken(AVal *usherToken);
	// void CloseInternal( int reconnect);
	int yang_ParseURL( char *url, int *protocol, AVal *host,
			unsigned int *port, AVal *app);
	void RTMP_SetupStream(int protocol, AVal *host, unsigned int port,
			AVal *sockshost, AVal *playpath, AVal *tcUrl, AVal *swfUrl,
			AVal *pageUrl, AVal *app, AVal *auth, AVal *swfSHA256Hash,
			uint32_t swfSize, AVal *flashVer, AVal *subscribepath, AVal *usherToken,
			int dStart, int dStop, int bLiveStream, long int timeout);
	int RTMP_ParseURL(char *url, int *protocol, AVal *host, unsigned int *port,
		AVal *playpath, AVal *app);
	int RTMP_ParseURL1(char *url, int *protocol, AVal *host, unsigned int *port,AVal *app);
	int parseAMF(AMFObject *obj, AVal *av, int *depth);
	int yang_SetupURL(char *url,int pport);
	int SendReleaseStream();
	int SendPublish();
	int SendFCPublish();
	int SendPong(double txn);
	int SendCheckBWResult(double txn);
	int SendCheckBW();
	int SendBytesReceived();
	int HandleInvoke(const char *body, unsigned int nBodySize);
	int SendSecureTokenResponse(AVal *resp);
	int SendPlay();
	int SendPlaylist();
	void HandleCtrl(const RTMPPacket *packet);
	int HandShake(int FP9HandShake);
	int SHandShake();
	AVal* getPlayPath();
	//AVal getPlayPath1();
private:
	//void initAVStreams();
	int RTMPSockBuf_Fill( RTMPSockBuf *sb);
private:

};

#endif /* YANGRTMP_IMPL_YANGRTMPBASE_H_ */
