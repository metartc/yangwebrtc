
#ifndef YANGSTREAM_INCLUDE_YANGAUDIOSTREAMDATA_H_
#define YANGSTREAM_INCLUDE_YANGAUDIOSTREAMDATA_H_
#include "yangutil/yangtype.h"
#include "stdint.h"
class YangAudioFrame {
public:
	YangAudioFrame();
	virtual ~YangAudioFrame();
	void setAudioData(unsigned char* p,int plen);
	void setAudioMetaData(unsigned char* p,int plen);
	void setFrametype(int frametype);
	void init(int transType,int sample,int channel,YangAudioEncDecType audioType);
	unsigned char* getAudioData();
	long long getRtmpTimestamp();
	long long getTsTimestamp();
public:
	long long getTimestamp();
	int getAudioLen();
	int getFrametype();
	YangAudioEncDecType getAudioType();

private:
	unsigned char* m_audioBufs;
	unsigned char* m_audioBuffer;
	int m_audioHeaderLen;
	uint8_t* m_src;
	int m_srcLen;
	int m_audioLen;

	long long atime ;
	double atime1;
	double perSt ;

	int m_transType;
	int m_frametype;
	YangAudioEncDecType m_audioType;
};

#endif /* YANGSTREAM_INCLUDE_YANGAUDIOSTREAMDATA_H_ */
