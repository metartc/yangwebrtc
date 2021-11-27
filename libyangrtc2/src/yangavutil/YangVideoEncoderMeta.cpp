/*
 * YangVideoEncoderMeta.cpp
 *
 *  Created on: 2020年08月14日
 *      Author: yang
 */
#include <yangutil/sys/YangLog.h>
#include <yangutil/yang_unistd.h>
#include <string.h>
#include <malloc.h>
#include <yangavutil/video/YangVideoEncoderMeta.h>


YangVideoEncoderMeta::YangVideoEncoderMeta() {


}

YangVideoEncoderMeta::~YangVideoEncoderMeta() {

}

void yang_getConfig_Flv_H264(YangH2645Conf *p_264, uint8_t *configBuf,
		int32_t *p_configLen) {
	configBuf[0] = 0x17;
	configBuf[1] = 0x00;
	configBuf[2] = 0x00;
	configBuf[3] = 0x00;
	configBuf[4] = 0x00;
	configBuf[5] = 0x01;
	configBuf[6] = p_264->sps[1];//0x42;
	configBuf[7] = p_264->sps[2];//0xC0;
	configBuf[8] = p_264->sps[3];//0x29;		//0x29;  //AVCLevelIndication1f
	configBuf[9] = 0xff;		//03;//ff;//0x03; AVCLevelIndication
	configBuf[10] = 0xe1;		//01;//e1;//01;numOfSequenceParameterSets
	char * szTmp = (char*) configBuf + 11;


    *szTmp++=0x00;
    *szTmp++=p_264->spsLen;

	memcpy(szTmp, p_264->sps, p_264->spsLen);
	szTmp += p_264->spsLen;
	*szTmp = 0x01;
	szTmp += 1;
    *szTmp++=0x00;
    *szTmp++=p_264->ppsLen;
	memcpy(szTmp, p_264->pps, p_264->ppsLen);

	szTmp += p_264->ppsLen;
	*p_configLen = szTmp - (char*) configBuf;
	szTmp = NULL;
}

void yang_getConfig_Flv_H265(YangH2645Conf *p_264,
		uint8_t *configBuf, int32_t *p_configLen) {
	int32_t i = 0;
	configBuf[i++] = 0x1C;		//0x17;
	configBuf[i++] = 0x00;

	configBuf[i++] = 0x00;
	configBuf[i++] = 0x00;
	configBuf[i++] = 0x00;
//configurationVersion
	configBuf[i++] = 0x00;


	configBuf[i++] = p_264->sps[1];
//general_profile_compatibility_flags 32 bit
	configBuf[i++] = p_264->sps[2];
	configBuf[i++] = p_264->sps[3];
	configBuf[i++] = p_264->sps[4];
	configBuf[i++] = p_264->sps[5];

// 48 bit NUll nothing deal in rtmp
	configBuf[i++] = p_264->sps[6];
	configBuf[i++] = p_264->sps[7];
	configBuf[i++] = p_264->sps[8];
	configBuf[i++] = p_264->sps[9];
	configBuf[i++] = p_264->sps[10];
	configBuf[i++] = p_264->sps[11];

//general_level_idc
	configBuf[i++] = p_264->sps[12];

//configBuf[i++] = p_264->sps[13];
//configBuf[i++] = p_264->sps[14];

//configBuf[i++] = 0x00;
//configBuf[i++] = 0x00;
//configBuf[i++] = 0x00;
//configBuf[i++] = 0x00;
// 48 bit NUll nothing deal in rtmp
	configBuf[i++] = 0x00;
	configBuf[i++] = 0x00;
	configBuf[i++] = 0x00;
	configBuf[i++] = 0x00;
	configBuf[i++] = 0x00;
	configBuf[i++] = 0x00;

//bit(16) avgFrameRate;
	configBuf[i++] = 0x00;
	configBuf[i++] = 0x00;

//bit(2) constantFrameRate;
// bit(3) numTemporalLayers;
///bit(1) temporalIdNested;
	configBuf[i++] = 0x00;
	configBuf[i++] = 0x03;

//yang_trace("HEVCDecoderConfigurationRecord data = %s\n", configBuf);
	configBuf[i++] = 0x20;  //vps 32
	configBuf[i++] = 0x0;
	configBuf[i++] = 0x01;
	configBuf[i++] = (p_264->vpsLen >> 8) & 0xff;
	configBuf[i++] = (p_264->vpsLen) & 0xff;
	memcpy(&configBuf[i], p_264->vps, p_264->vpsLen);
	i += p_264->vpsLen;

	configBuf[i++] = 0x21; //sps 33
	configBuf[i++] = (1 >> 8) & 0xff;
	configBuf[i++] = 1 & 0xff;
	configBuf[i++] = (p_264->spsLen >> 8) & 0xff;
	configBuf[i++] = (p_264->spsLen) & 0xff;
	memcpy(&configBuf[i], p_264->sps, p_264->spsLen);
	i += p_264->spsLen;

	configBuf[i++] = 0x22; //sps 33
	configBuf[i++] = (1 >> 8) & 0xff;
	configBuf[i++] = 1 & 0xff;
	configBuf[i++] = (p_264->ppsLen >> 8) & 0xff;
	configBuf[i++] = (p_264->ppsLen) & 0xff;
	memcpy(&configBuf[i], p_264->pps, p_264->ppsLen);
	i += p_264->ppsLen;
	*p_configLen = i;

}
