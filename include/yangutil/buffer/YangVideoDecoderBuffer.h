#ifndef __YangVideoDecoderBuffer__
#define __YangVideoDecoderBuffer__
#include <string.h>
#include <stdint.h>
struct YangVideoDBufferHeader{
	int32_t uid;
	int32_t length;
	int64_t timestamp;
	int32_t frametype;
};

#include <yangutil/buffer/YangMediaBuffer.h>
class YangVideoDecoderBuffer:public YangMediaBuffer
{
public:
	YangVideoDecoderBuffer();
	~YangVideoDecoderBuffer(void);

	void putEVideo(YangVideoFrame* pframe);
	void getEVideo(YangVideoFrame* pframe);
private:
	int32_t m_headerLen;
	YangVideoDBufferHeader *m_in,*m_out;
	void initHeader();

};
#endif
