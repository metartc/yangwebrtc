/*
 * YangAudioEncoderFactory.cpp
 *
 *  Created on: 2019年9月3日
 *      Author: yang
 */

#include "YangH264EncoderIntel.h"
#include "YangH264EncoderSoft.h"
#include "YangH265EncoderSoft.h"
#include "YangVideoEncoderFfmpeg.h"
#include "yangencoder/YangEncoderFactory.h"

#include "YangAudioEncoderAac.h"
#include "YangAudioEncoderMp3.h"
#include "YangAudioEncoderOpus.h"
#include "YangAudioEncoderSpeex.h"
#include "YangH264EncoderMeta.h"
#include "YangH265EncoderMeta.h"
#include "YangFfmpegEncoderMeta.h"
YangEncoderFactory::YangEncoderFactory() {


}

YangEncoderFactory::~YangEncoderFactory() {

}
YangVideoEncoderMeta* YangEncoderFactory::createVideoEncoderMeta(YangVideoInfo *pini){
	if(pini->videoEncHwType>0) return new YangFfmpegEncoderMeta();
	if(pini->videoEncoderType==0) return new YangH264EncoderMeta();
	if(pini->videoEncoderType==1) return new YangH265EncoderMeta();
	return new YangH264EncoderMeta();
}


YangAudioEncoder *YangEncoderFactory::createAudioEncoder(YangAudioEncDecType paet,YangAudioInfo *pini){
	if(paet==Yang_AED_MP3) return new YangAudioEncoderMp3();
	if(paet==Yang_AED_SPEEX) return new YangAudioEncoderSpeex();
	if(paet==Yang_AED_OPUS) return new YangAudioEncoderOpus();
	return new YangAudioEncoderAac();
}
YangAudioEncoder *YangEncoderFactory::createAudioEncoder(YangAudioInfo *pini){
	YangAudioEncDecType maet=Yang_AED_AAC;

	if(pini->audioEncoderType==1) maet=Yang_AED_MP3;
	if(pini->audioEncoderType==2) maet=Yang_AED_SPEEX;
	if(pini->audioEncoderType==3) maet=Yang_AED_OPUS;
	return createAudioEncoder(maet,pini);
}

YangVideoEncoder* YangEncoderFactory::createVideoEncoder(YangVideoEncDecType paet,YangVideoInfo *pini){
	if(pini->videoEncHwType==0){
		//printf("\n*********************pate===%d\n",paet);
		if (paet == Yang_VED_264)		return  new YangH264EncoderSoft();
		if (paet == Yang_VED_265)		return  new YangH265EncoderSoft();
	}else{
					return new YangVideoEncoderFfmpeg(paet,pini->videoEncHwType);
	}

		return new YangH264EncoderSoft();
}
	YangVideoEncoder* YangEncoderFactory::createVideoEncoder(YangVideoInfo *pini){
		YangVideoEncDecType maet=Yang_VED_264;
			if(pini->videoEncoderType==0) maet=Yang_VED_264;
			if(pini->videoEncoderType==1) maet=Yang_VED_265;
			//if(pini->audioEncoderType==2) maet=Yang_AE_SPEEX;
			return createVideoEncoder(maet,pini);
	}
