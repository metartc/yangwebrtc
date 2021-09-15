#ifndef ___YangAudioPlayBuffer__
#define ___YangAudioPlayBuffer__
#include <string.h>
#include <yangutil/buffer/YangMediaBuffer.h>
#include "../YangAvsynI.h"

class YangAudioPlayBuffer:public YangMediaBuffer
{
public:
	YangAudioPlayBuffer();
	~YangAudioPlayBuffer(void);
	YangAvsynI* m_syn;
	void putAudio(YangFrame* pframe);
	void getAudio(YangFrame* pframe);
	uint8_t *getAudios(YangFrame* pframe);
	int32_t getFrameTimestamp(int64_t *timestamp);
	//int32_t playFrame();
	void reset();
private:
	int32_t m_bufLen;

};
#endif
