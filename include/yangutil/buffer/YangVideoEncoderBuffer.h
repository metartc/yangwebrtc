#ifndef __YangVideoEncoderBuffer__
#define __YangVideoEncoderBuffer__
#include <string.h>
#include <yangutil/buffer/YangMediaBuffer.h>

struct YangVideoEBufferHeader{
	int32_t length;
	int64_t timestamp;
	int32_t frametype;
};

class YangVideoEncoderBuffer:public YangMediaBuffer
{
public:
	YangVideoEncoderBuffer(YangVideoInfo *config);
	YangVideoEncoderBuffer(YangVideoInfo *config,int32_t ttype);
	~YangVideoEncoderBuffer(void);


	void getEVideo(YangVideoFrame* pframe);
	uint8_t * getEVideoRef(YangVideoFrame* pframe);
	void putEVideo(YangVideoFrame* pframe);
private:
	int32_t m_headerLen;
	YangVideoEBufferHeader *m_in,*m_out;
	void initHeader();
};
#endif
