/*
 * YangCaptureImpl.h
 *
 *  Created on: 2020年8月30日
 *      Author: yang
 */

#ifndef INCLUDE_YANGCAPTURE_YANGCAPTUREFACTORY_H_
#define INCLUDE_YANGCAPTURE_YANGCAPTUREFACTORY_H_
#include <yangcapture/YangMultiVideoCapture.h>
#include "YangAudioCapture.h"
#include "YangScreenCapture.h"
//#include "../include/YangLivingVideoCapture.h"

class YangCaptureFactory {
public:
	YangCaptureFactory();
	virtual ~YangCaptureFactory();
	YangAudioCapture *createAudioCapture(YangAudioInfo *pini);
	YangAudioCapture *createRecordAudioCapture(YangAudioInfo *pini);
	YangMultiVideoCapture *createVideoCapture(YangVideoInfo *pini);
	YangMultiVideoCapture *createRecordVideoCapture(YangVideoInfo *pini);
	YangScreenCapture *createScreenCapture(int32_t capType);
};

#endif /* INCLUDE_YANGCAPTURE_YANGCAPTUREFACTORY_H_ */
