/*
 * YangH246Encoder.h
 *
 *  Created on: 2019年10月16日
 *      Author: yang
 */

#ifndef YANGENCODER_INCLUDE_YANGVideoENCODER_H_
#define YANGENCODER_INCLUDE_YANGVideoENCODER_H_
#include "stdint.h"
#include "yangutil/yangtype.h"
#include "YangEncoder.h"
class YangVideoEncoder{
public:
	YangVideoEncoder();
	virtual ~YangVideoEncoder();
	virtual void init(YangVideoInfo *pvp,YangVideoEncInfo *penc)=0;
	virtual int encode(int puid,unsigned char *pData,int nSize,long long ptimestamp, YangEncoderCallback* pcallback)=0;
	virtual void setVideoMetaData(YangVideoMeta *pvmd)=0;
	virtual void sendKeyFrame()=0;
protected:
    int m_isInit;
    YangVideoInfo m_yvp;
    YangVideoEncInfo m_enc;
    unsigned char* m_vbuffer;
   void setVideoPara(YangVideoInfo *pap,YangVideoEncInfo *penc);
};

#endif /* YANGENCODER_INCLUDE_YANGENCODER_H_ */
