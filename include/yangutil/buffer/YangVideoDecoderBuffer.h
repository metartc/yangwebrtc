#ifndef __YangVideoDecoderBuffer__
#define __YangVideoDecoderBuffer__
#include <string.h>
struct YangVideoDBufferHeader{
	int uid;
	int length;
	long long timestamp;
	int frametype;
};

#include <yangutil/buffer/YangMediaBuffer.h>
class YangVideoDecoderBuffer:public YangMediaBuffer
{
public:
	YangVideoDecoderBuffer();
	~YangVideoDecoderBuffer(void);

	void putEVideo(int pind,unsigned char *p,int bufLen, long long timestamp,int frametype);
	void getEVideo(int *pind,unsigned char *p, int *bufLen, long long *timestamp,int *frametype);
private:
	int m_headerLen;
	YangVideoDBufferHeader *m_in,*m_out;
	void initHeader();

};
#endif
