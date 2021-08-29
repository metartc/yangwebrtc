#ifndef __YangVideoEncoderBuffer__
#define __YangVideoEncoderBuffer__
#include <string.h>

struct YangVideoEBufferHeader{
	int length;
	long long timestamp;
	int frametype;
};
#include <yangutil/buffer/YangMediaBuffer.h>
class YangVideoEncoderBuffer:public YangMediaBuffer
{
public:
	YangVideoEncoderBuffer(YangVideoInfo *config);
	YangVideoEncoderBuffer(YangVideoInfo *config,int ttype);
	~YangVideoEncoderBuffer(void);


	void getEVideo(unsigned char *p, int *bufLen, long long *timestamp,int *frametype);
	unsigned char * getEVideoRef(int *bufLen, long long *timestamp,int *frametype);
	void putEVideo(unsigned char *p,int bufLen,long long timestamp,int frametype);
private:
	int m_headerLen;
	YangVideoEBufferHeader *m_in,*m_out;
	void initHeader();
};
#endif
