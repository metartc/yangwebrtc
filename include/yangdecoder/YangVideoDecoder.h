#ifndef YANGH264DECODER_H
#define YANGH264DECODER_H
#include "YangDecoder.h"
#include "yangutil/yangtype.h"
class YangVideoDecoder {
public:
	YangVideoDecoder(){}
	virtual ~YangVideoDecoder(){}
	virtual void init()=0;
	virtual void parseRtmpHeader(unsigned char *p, int pLen, int *pwid,
			int *phei, int *pfps)=0;
	virtual int decode(int puid,unsigned char *pData,int nSize,long long ptimestamp,YangYuvType pyuvType,YangDecoderCallback* pcallback)=0;
	int m_isInit=0;
	int m_uid = -1;
	int m_state = 0;
protected:

private:

};

#endif // YANGH264DECODER_H
