/*
 * YangVideoEncoderMeta.h
 *
 *  Created on: 2020年11月14日
 *      Author: yang
 */

#ifndef YANGENCODER_INCLUDE_YANGVIDEOENCODERMETA_H_
#define YANGENCODER_INCLUDE_YANGVIDEOENCODERMETA_H_
#include "yangutil/yangtype.h"
#include <stdint.h>
class YangVideoEncoderMeta {
public:
	YangVideoEncoderMeta();
	virtual ~YangVideoEncoderMeta();
	virtual void yang_initVmd(YangVideoMeta *p_vmd, YangVideoInfo *p_config, YangVideoEncInfo *penc)=0;
protected:
	void yang_getConfig_Flv_H264(YangH2645Conf *p_264, unsigned char *configBuf,int *p_configLen);
	void yang_getConfig_Flv_H265(YangH2645Conf *p_264, unsigned char *configBuf,int *p_configLen);
};

#endif /* YANGENCODER_INCLUDE_YANGVIDEOENCODERMETA_H_ */
