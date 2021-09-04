#ifndef ___YangAudioPlayBuffer__
#define ___YangAudioPlayBuffer__
#include <string.h>
#include <yangutil/buffer/YangMediaBuffer.h>
#include "../YangAvsynI.h"
struct YangAudioBufferHeader{
	int32_t length;
	int64_t timestamp;
};
class YangAudioPlayBuffer:public YangMediaBuffer
{
public:
	YangAudioPlayBuffer();
	~YangAudioPlayBuffer(void);
	YangAvsynI* m_syn;
	void putAudio(YangAudioFrame* pframe);
	void getAudio(YangAudioFrame* pframe);
	uint8_t *getAudios(int64_t *timestamp,int32_t *plen);
	int32_t getTimestamp(int64_t *timestamp);
	int32_t playFrame();
	void reset();
private:
	int32_t m_bufLen;
	int32_t m_headerLen;
	YangAudioBufferHeader *m_in,*m_out;
	uint8_t * retChar;
};
#endif
