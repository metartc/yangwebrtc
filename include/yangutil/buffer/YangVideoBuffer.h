#ifndef __YangVideoBuffer__
#define __YangVideoBuffer__
#include <string.h>
#include <yangutil/buffer/YangMediaBuffer.h>
#include <yangutil/yangavinfotype.h>
#include "../YangAvsynI.h"
struct YangVideoBufferHeader{
	int32_t length;
	int64_t timestamp;
};
#define yang_get_videoBuffer(x) new YangVideoBuffer(x->width,x->height,x->videoCaptureFormat==YangYuy2?16:12,x->bitDepth==8?1:2)
class YangVideoBuffer:public YangMediaBuffer
{
public:
	YangVideoBuffer(int32_t pBitDepthLen);

	YangVideoBuffer(int32_t pwid,int32_t phei,int32_t ptype,int32_t pBitDepthLen);
	~YangVideoBuffer(void);
	void initTemp();
	void init(int32_t pwid,int32_t phei,int32_t ptype);

	void reset(YangVideoInfo *conf);
	void reset();
	YangAvsynI* m_syn;
	//short type;
	int32_t isPreview=0;
	int32_t m_width;
	int32_t m_height;
	int32_t m_length;
	int32_t m_frames;
	int32_t m_bitDepthLen;
	void putVideo(YangVideoFrame* pframe);
	void getVideo(YangVideoFrame* pframe);
	uint8_t * getVideoIn(int64_t *timestamp);
	int32_t getTimestamp(int64_t *timestamp);
	int32_t playFrame();

private:
	int32_t m_headerLen;
	YangVideoBufferHeader *m_in,*m_out;
	void initHeader();
	uint8_t * retChar;

};
#endif
