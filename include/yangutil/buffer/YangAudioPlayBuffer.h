#ifndef ___YangAudioPlayBuffer__
#define ___YangAudioPlayBuffer__
#include <string.h>
#include <yangutil/buffer/YangMediaBuffer.h>
#include "../YangAvsynI.h"
struct YangAudioBufferHeader{
	int length;
	long long timestamp;
};
class YangAudioPlayBuffer:public YangMediaBuffer
{
public:
	YangAudioPlayBuffer();
	~YangAudioPlayBuffer(void);
	YangAvsynI* m_syn;
	void putAudio(unsigned char *p_buf,int plen,long long ptimestamp);
	void getAudio(unsigned char *p_buf,int* plen,long long *timestamp);
	unsigned char *getAudios(long long *timestamp,int *plen);
	int getTimestamp(long long *timestamp);
	int playFrame();
	void reset();
private:
	int m_bufLen;
	int m_headerLen;
	YangAudioBufferHeader *m_in,*m_out;
	unsigned char * retChar;
};
#endif
