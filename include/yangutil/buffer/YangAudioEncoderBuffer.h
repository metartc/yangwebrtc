#ifndef YangAudioEncoderBuffer_H
#define YangAudioEncoderBuffer_H

#include <yangutil/buffer/YangMediaBuffer.h>
struct YangAudioEBufferHeader {
	int length;
	int palyId;
};
class YangAudioEncoderBuffer: public YangMediaBuffer {
public:
	YangAudioEncoderBuffer(int paudioCacheNum);
	~YangAudioEncoderBuffer(void);
	void reset();

	void putAudio(unsigned char *p, int bufLen);
	void getAudio(unsigned char *p, int *bufLen);
	unsigned char* getAudioRef(int *bufLen);
	void putPlayAudio(unsigned char *p, int bufLen, int playId);
	void getPlayAudio(unsigned char *p, int *bufLen, int *playId);

private:
	int m_headerLen, m_tid;
	YangAudioEBufferHeader *m_in, *m_out;
	void initHeader();
};

#endif
