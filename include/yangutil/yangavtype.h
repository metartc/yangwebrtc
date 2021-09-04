#ifndef INCLUDE_YANGUTIL_YANGAVTYPE_H_
#define INCLUDE_YANGUTIL_YANGAVTYPE_H_
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
    uint8_t r;
    uint8_t g;
    uint8_t b;
};
struct YangH2645Conf {
	int32_t vpsLen;
	int32_t spsLen;
	int32_t ppsLen;
	uint8_t vps[128];
	uint8_t sps[128];
	uint8_t pps[64];
};
struct YangRtmpMeta {
	uint8_t buffer[128];
	int32_t bufLen;
};
struct YangVideoMeta {
	int32_t isInit;
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
struct YangAudioFrame{
	uint8_t* payload;
	int32_t nb;
	int32_t uid;
	int64_t timestamp;
	YangAudioFrame(){
		payload=NULL;
		nb=0;
		uid=0;
		timestamp=0;
	}
};

struct YangVideoFrame{
	uint8_t* payload;
	int32_t nb;
	int32_t uid;
	int32_t frametype;
	YangYuvType yuvType;
	int64_t timestamp;
	YangVideoFrame(){
		payload=NULL;
		nb=0;
		uid=0;
		frametype=0;
		timestamp=0;
		yuvType=YangI420;
	}
};

struct YangSharedPtrMessage{
	int32_t mediaType;
	int32_t nb;
	int64_t timestamp;
	uint8_t* payload;
	YangSharedPtrMessage(){
		mediaType=0;
		nb=0;
		timestamp=0;
		payload=NULL;
	}

};

#endif /* INCLUDE_YANGUTIL_YANGAVTYPE_H_ */
