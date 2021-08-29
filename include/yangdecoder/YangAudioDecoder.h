/*
 * YangAudioDecoder.h
 *
 *  Created on: 2020年9月3日
 *      Author: yang
 */

#ifndef YANGDECODER_INCLUDE_YANGAUDIODECODER_H_
#define YANGDECODER_INCLUDE_YANGAUDIODECODER_H_
#include "yangutil/yangtype.h"
#include "yangstream/YangStreamType.h"
#include "YangDecoder.h"
class YangAudioDecoder{
public:
	YangAudioDecoder();
	virtual ~YangAudioDecoder();
    virtual void init()=0;
    virtual int decode(int puid,unsigned char *pData,int nSize,long long ptimestamp,YangDecoderCallback* pcallback)=0;
    int m_uid;
protected:
	int m_isInit;
	int m_frameSize;
	int m_alen;
	YangAudioParam *m_ini;
	unsigned char* m_dstBuffer;
	int m_dstLen;
};

#endif /* YANGDECODER_INCLUDE_YANGAUDIODECODER_H_ */
