#ifndef YangAudioEncoderBuffer_H
#define YangAudioEncoderBuffer_H

#include <yangutil/buffer/YangMediaBuffer.h>
struct YangAudioEBufferHeader {
	int32_t length;
	int32_t palyId;
};
class YangAudioEncoderBuffer: public YangMediaBuffer {
public:
	YangAudioEncoderBuffer(int32_t paudioCacheNum);
	~YangAudioEncoderBuffer(void);
	void reset();

	void putAudio(YangAudioFrame* audioFrame);
	void getAudio(YangAudioFrame* audioFrame);
	uint8_t* getAudioRef(int32_t *bufLen);
	void putPlayAudio(YangAudioFrame* pframe);
	void getPlayAudio(YangAudioFrame* audioFrame);

private:
	int32_t m_headerLen, m_tid;
	YangAudioEBufferHeader *m_in, *m_out;
	void initHeader();
};

#endif
