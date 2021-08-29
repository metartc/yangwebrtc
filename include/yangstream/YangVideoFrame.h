#ifndef YANGSTREAM_INCLUDE_YANGVIDEOSTREAMDATA_H_
#define YANGSTREAM_INCLUDE_YANGVIDEOSTREAMDATA_H_
#include "yangutil/yangtype.h"
#include "stdint.h"
struct YangSpsppsConf{
	uint8_t* sps;
	uint8_t* pps;
	int spsLen;
	int ppsLen;
};
class YangVideoFrame {
public:
	YangVideoFrame();
	virtual ~YangVideoFrame();

	void init(int transtype);
	void setVideoData(unsigned char* p,int plen,long long videoTimestamp,int frametype,YangVideoEncDecType videoType=Yang_VED_264);
	void setVideoMeta(unsigned char* p,int plen,YangVideoEncDecType videoType=Yang_VED_264);
	void setFrametype(int frametype);

	long long getRtmpTimestamp();
	long long getTsTimestamp();


public:
	//uint8_t* getVideoMeta();
	//int getVideoMetaLen();
	uint8_t* getVideoData();
	YangSpsppsConf* getSpsppsConf();

	int getFrametype();
	int getVideoLen();
	long long getTimestamp();
protected:

	void initEncType(YangVideoEncDecType videoType,int frametype);
	void initSpspps(uint8_t *p);
private:
	uint8_t* m_videoBufs;
	uint8_t* m_videoBuffer;
	//uint8_t* m_videoMeta;
	YangSpsppsConf *m_spsppsConf;
	int m_frametype;
	int m_videoLen;
	uint8_t* m_src;

	int m_transType;
	char* yang_put_be32(char *output, unsigned int nVal);

	long long videoTimestamp , baseTimestamp;
	long long curVideotimestamp = 0;
	int preTimestamp = 0;



};

#endif /* YANGSTREAM_INCLUDE_YANGVIDEOSTREAMDATA_H_ */
