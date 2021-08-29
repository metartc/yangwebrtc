#ifndef ___YangAudioBuffer__
#define ___YangAudioBuffer__
#include <string.h>

#include <yangutil/buffer/YangMediaBuffer.h>
class YangAudioBuffer:public YangMediaBuffer
{
public:
	YangAudioBuffer(int plen,int pcacheNum);
	~YangAudioBuffer(void);
	void putAudio(unsigned char *p_buf,int plen);
	int getAudio(unsigned char *p_buf,int* plen);
	unsigned char *getAudio(int* plen);
	void reset();

private:
	int m_headerLen,m_in,m_out;
	int m_bufLen;

};
#endif
