#ifndef INCLUDE_YANGUTIL_YANGAVTYPE_H_
#define INCLUDE_YANGUTIL_YANGAVTYPE_H_
#include <stdio.h>
#include <stdint.h>
#define YANG_Frametype_Spspps 9
#define YANG_Frametype_I 1
#define YANG_Frametype_P 0
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
struct YangAudioParam{
	YangAudioEncDecType encode;
	int32_t sample;
	int32_t channel;
    int32_t audioClock;
};

struct YangVideoParam{
	YangVideoEncDecType encode;
	int32_t videoClock;

};
struct YangFrame{
	int32_t mediaType;
	int32_t uid;
	int32_t frametype;
	int32_t nb;
	int64_t timestamp;
	uint8_t* payload;
	YangFrame(){
		mediaType=0;
		uid=0;
		frametype=0;
		nb=0;
		timestamp=0;
		payload=NULL;
	}
};

void yang_frame_copy( YangFrame* src,YangFrame* dst);
void yang_frame_copy_buffer( YangFrame* src,YangFrame* dst);
void yang_frame_copy_nobuffer(YangFrame* src,YangFrame* dst);
//void yang_frame_init(YangFrame* frame);

struct YangMessage{
	int32_t mediaType;
	int32_t nb;
	int64_t timestamp;

	char* payload;
	YangMessage(){
		mediaType=0;
		nb=0;
		timestamp=0;
		payload=NULL;
	}

};


class YangSample
{
public:
    // The size of unit.
    int32_t m_size;
    // The ptr of unit, user must free it.
    char* m_bytes;
public:
    YangSample();
    YangSample(char* b, int32_t s);
    ~YangSample();
public:

    // Copy sample, share the bytes pointer.
    YangSample* copy();
};

class YangBufferManager{
public:
	YangBufferManager();
	YangBufferManager(int32_t num,int32_t bufsize);
	virtual ~YangBufferManager();
	void init(int32_t num,int32_t bufsize);
	uint8_t* getBuffer();
private:
	uint8_t* m_cache;
	int32_t m_curindex;
	int32_t m_unitsize;
	int32_t m_size;
};

#endif /* INCLUDE_YANGUTIL_YANGAVTYPE_H_ */
