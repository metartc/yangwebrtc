#ifndef __YangVideoBuffer__
#define __YangVideoBuffer__
#include <string.h>
#include <yangutil/buffer/YangMediaBuffer.h>
#include "yangutil/yangtype.h"
#include "../YangAvsynI.h"
struct YangVideoBufferHeader{
	int length;
	long long timestamp;
};
#define yang_get_videoBuffer(x) new YangVideoBuffer(x->width,x->height,x->videoCaptureFormat==YangYuy2?16:12,x->bitDepth==8?1:2)
class YangVideoBuffer:public YangMediaBuffer
{
public:
	YangVideoBuffer(int pBitDepthLen);

	YangVideoBuffer(int pwid,int phei,int ptype,int pBitDepthLen);
	~YangVideoBuffer(void);
	void initTemp();
	void init(int pwid,int phei,int ptype);

	void reset(YangVideoInfo *conf);
	void reset();
	YangAvsynI* m_syn;
	//short type;
	int isPreview=0;
	int m_width;
	int m_height;
	int m_length;
	int m_frames;
	int m_bitDepthLen;
	void putVideo(unsigned char *p,int bufLen,long long timestamp);
	void getVideo(unsigned char *p,int bufLen, long long *timestamp);
	unsigned char * getVideoIn(long long *timestamp);
	int getTimestamp(long long *timestamp);
	int playFrame();

private:
	int m_headerLen;
	YangVideoBufferHeader *m_in,*m_out;
	void initHeader();
	unsigned char * retChar;

};
#endif
