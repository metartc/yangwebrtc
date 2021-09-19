/*
 * YangCaptureImpl.cpp
 *
 *  Created on: 2019年8月30日
 *      Author: yang
 */

#include "yangcapture/YangCaptureFactory.h"
#ifndef _WIN32
#include "YangAlsaDeviceHandle.h"
#include "YangAlsaHandle.h"
#include "YangAudioCaptureImpl.h"
#include "YangVideoCaptureImpl.h"

#else
#include "win/YangWinVideoCapture.h"
#include "win/YangWinAudioCapture.h"
#include "win/YangWinAudioApiDevice.h"
#include "win/YangWinRecordAudioCapture.h"
#endif
#include "YangScreenShare.h"
YangCaptureFactory::YangCaptureFactory() {


}

YangCaptureFactory::~YangCaptureFactory() {

}

YangAudioCapture *YangCaptureFactory::createAudioCapture(YangAudioInfo *pini){
#ifndef _WIN32
	return new YangAlsaDeviceHandle(pini);//new YangAlsaHandle(pini);//YangAudioCaptureImpl(pini);
#else
    return new YangWinAudioCapture(pini);
   //return new YangWinAudioApiDevice(pini);
#endif

}
YangAudioCapture *YangCaptureFactory::createRecordAudioCapture(YangAudioInfo *pini){
#ifndef _WIN32
	return new YangAudioCaptureImpl(pini);//new YangAlsaHandle(pini);//YangAudioCaptureImpl(pini);
#else
    return new YangWinRecordAudioCapture(pini);
#endif
}


YangMultiVideoCapture *YangCaptureFactory::createVideoCapture(YangVideoInfo *pini){
#ifndef _WIN32
	return new YangVideoCaptureImpl(pini);
#else
    return new YangWinVideoCapture(pini);
#endif
}
YangMultiVideoCapture *YangCaptureFactory::createRecordVideoCapture(YangVideoInfo *pini){
#ifndef _WIN32
	return new YangVideoCaptureImpl(pini);
#else
	return new YangWinVideoCapture(pini);
#endif
}

YangScreenCapture *YangCaptureFactory::createScreenCapture(int32_t capType){
	return new YangScreenShare();
}
