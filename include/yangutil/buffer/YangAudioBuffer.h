#ifndef ___YangAudioBuffer__
#define ___YangAudioBuffer__
#include <string.h>

#include <yangutil/buffer/YangMediaBuffer.h>
class YangAudioBuffer:public YangMediaBuffer
{
public:
	YangAudioBuffer(int32_t plen,int32_t pcacheNum);
	~YangAudioBuffer(void);
	void putAudio(uint8_t *p_buf,int32_t plen);
	int32_t getAudio(YangAudioFrame* pframe);
	uint8_t *getAudio(int* plen);
	void reset();

private:
	int32_t m_headerLen,m_in,m_out;
	int32_t m_bufLen;

};
#endif
