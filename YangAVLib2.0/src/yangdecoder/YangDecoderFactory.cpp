/*
 * YangDecoderFactory.cpp
 *
 *  Created on: 2019年9月3日
 *      Author: yang
 */

#include "yangdecoder/YangDecoderFactory.h"

#include "YangAudioDecoderAac.h"
//#include "YangAudioDecoderMp3.h"
#include "YangAudioDecoderOpus.h"
#include "YangAudioDecoderSpeex.h"
#include "YangVideoDecoderIntel.h"
#include "YangH2645VideoDecoderFfmpeg.h"
#include "YangH264DecoderSoft.h"
#include "YangH264DecoderSoftFactory.h"
YangDecoderFactory::YangDecoderFactory() {


}

YangDecoderFactory::~YangDecoderFactory() {
	// TODO Auto-generated destructor stub
}

YangAudioDecoder *YangDecoderFactory::createAudioDecoder(YangAudioEncDecType paet,YangAudioParam *pini){
	//if(paet==Yang_AED_MP3) return new YangAudioDecoderMp3(pini);
	if(paet==Yang_AED_SPEEX) return new YangAudioDecoderSpeex(pini);
	if(paet==Yang_AED_OPUS) return new YangAudioDecoderOpus(pini);
	return new YangAudioDecoderAac(pini);
}

YangAudioDecoder *YangDecoderFactory::createAudioDecoder(YangAudioParam *pini){

	//printf("\n***********createAudioDecoder=%d**\n",maet);
	return createAudioDecoder(pini->encode,pini);
}

YangVideoDecoder* YangDecoderFactory::createFfmpegVideoDecoder(YangVideoEncDecType paet,YangVideoInfo *pini){
	return new YangH2645VideoDecoderFfmpeg(pini,paet);
}
YangVideoDecoder* YangDecoderFactory::createVideoDecoder(YangVideoEncDecType paet,YangVideoInfo *pini){
	if (paet == Yang_VED_264)				{
		if(pini->videoDecHwType==0){
          //  return new YangH264DecoderSoft();
            return new YangH2645VideoDecoderFfmpeg(pini,paet);
		}else
		return new YangH2645VideoDecoderFfmpeg(pini,paet);
	}
	if (paet == Yang_VED_265)				return new YangH2645VideoDecoderFfmpeg(pini,paet);
	//de264=new YangVideoDecoderIntel();
	// if(p_decType==Type_Nvdia)   de264= new YangH264DecoderNv();
#if YangLibva
		return new YangVideoDecoderIntel();
#else
		return NULL;
#endif
}
	YangVideoDecoder* YangDecoderFactory::createVideoDecoder(YangVideoInfo *pini){
		YangVideoEncDecType maet=Yang_VED_264;
		if(pini->videoDecoderType==Yang_VED_265) maet=Yang_VED_265;
			//if(pini->videoDecoderType==1) maet=Type_Dec_Ffmpeg;
			//if(pini->audioDecoderType==2) maet=Yang_AE_SPEEX;
			return createVideoDecoder(maet,pini);
	}
