/*
 * yangtype.h
 *
 *  Created on: 2020年10月14日
 *      Author: yang
 */

#ifndef YANGUTIL_YANGTYPE_H_
#define YANGUTIL_YANGTYPE_H_
#include "YangErrorCode.h"
#include "../Yang_Config.h"

#define yang_delete(a) {if( (a) != NULL ) {delete (a); (a) = NULL;}}
#define yang_deleteA(a) {if( (a) != NULL ) {delete[] (a); (a) = NULL;}}
#define yang_free(a) {if( (a) != NULL ) {free((a)); (a) = NULL;}}
#define yang_min(a, b) (((a) < (b))? (a) : (b))
#define yang_max(a, b) (((a) < (b))? (b) : (a))
#define yang_freep(p) \
    if (p) { \
        delete p; \
        p = NULL; \
    } \
    (void)0
// Please use the freepa(T[]) to free an array, otherwise the behavior is undefined.
#define yang_freepa(pa) \
    if (pa) { \
        delete[] pa; \
        pa = NULL; \
    } \
    (void)0
#define Yang_Rtmp 0
#define Yang_Srt 1
#define Yang_Webrtc 2
enum YangYuvType{
    YangYuy2,
	YangI420,
	YangNv12,
    YangYv12,
	YangP010,
	YangP016
};
struct YangRect{
    short x;
    short y;
    short w;
    short h;
};
struct YangColor{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};
struct YangH2645Conf {
	int vpsLen;
	int spsLen;
	int ppsLen;
	unsigned char vps[128];
	unsigned char sps[128];
	unsigned char pps[64];
};
struct YangRtmpMeta {
	unsigned char buffer[128];
	int bufLen;
};
struct YangVideoMeta {
	int isInit;
	YangH2645Conf mp4Meta;
	//YangRtmpMeta flvMeta;
	YangRtmpMeta livingMeta;
};

enum YangAudioEncDecType{
	Yang_AED_AAC,
	Yang_AED_MP3,
	Yang_AED_SPEEX,
	Yang_AED_OPUS
};
enum YangVideoEncDecType{
	Yang_VED_264,
	Yang_VED_265,
	Yang_VED_vp9
};
enum YangVideoHwType{
	Yang_Hw_Soft,
	YangV_Hw_Intel,
	YangV_Hw_Nvdia,
	YangV_Hw_Android
};
/**
struct YangEncoderParam {
	int width;
	int height;
	int fps;
	int bitrate;
	int keyint_max;
	int level_idc;
	int profile;
};
**/
struct YangAudioInfo{
	int sample;
	int frameSize;
	int bitrate;
	int channel;

	int usingMono;
	int hasAec;
	int echoPath;

	int aecBufferFrames;
	int audioCacheSize;
	int audioCacheNum;
	int audioPlayCacheNum;

	int hasAudioHeader;
	int audioEncoderType;
	int	audioDecoderType;
	int audioPlayType;

	int aIndex;
	int aSubIndex;
};

struct YangVideoInfo{
		int width; //= 800
		int height; //= 600
		int outWidth;
		int outHeight;
		int rate; // 512
		int frame; //25
		//int bitcount; // 16
		int bitDepth;

		int videoCacheNum;
		int evideoCacheNum;
		int videoPlayCacheNum;

		int videoCaptureFormat;
		int videoEncoderFormat;
		
		int videoEncoderType;
		int videoDecoderType;
		int videoEncHwType;
		int videoDecHwType;
		int vIndex;
};
struct YangVideoEncInfo{
		int preset;
		int level_idc;
		int profile;
		int keyint_max;
		int enc_threads;
		//int fps_num;
};
struct YangSysInfo{
		int isMultCamera;
		int transType;
		int usingDataServer;
		int rtmpPort;
		int srtPort;
		int rtcPort;
		int rtcLocalPort;
		int httpPort;
		int dataPort;
		int hasLogFile;
		int logLevel;
		int cameraCount;

		char cameraIndexs[50];
		char rtmpServerIP[20];
		char srtServerIP[20];
		char rtcServerIP[20];
		char httpServerIP[20];
		char dataServerIP[20];
};



#endif /* YANGUTIL_YANGTYPE_H_ */
