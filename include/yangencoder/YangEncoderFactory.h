/*
 * YangAudioEncoderFactory.h
 *
 *  Created on: 2020年9月3日
 *      Author: yang
 */

#ifndef YANGENCODER_INCLUDE_YANGVIDEOENCODERFACTORY_H_
#define YANGENCODER_INCLUDE_YANGVIDEOENCODERFACTORY_H_
#include <yangutil/yangavinfotype.h>
#include "YangVideoEncoder.h"
#include "yangencoder/YangAudioEncoder.h"
#include "YangVideoEncoderMeta.h"

class YangEncoderFactory {
public:
	YangEncoderFactory();
	virtual ~YangEncoderFactory();
	YangAudioEncoder* createAudioEncoder(YangAudioEncDecType paet,YangAudioInfo *pini);
	YangAudioEncoder* createAudioEncoder(YangAudioInfo *pini);
	YangVideoEncoder* createVideoEncoder(YangVideoEncDecType paet,YangVideoInfo *pini);
	YangVideoEncoder* createVideoEncoder(YangVideoInfo *pini);
	YangVideoEncoderMeta* createVideoEncoderMeta(YangVideoInfo *pini);
};

#endif /* YANGENCODER_INCLUDE_YANGVIDEOENCODERFACTORY_H_ */
