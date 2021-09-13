#include "YangAudioEncoderAac.h"

#include "unistd.h"
#include "stdio.h"

void YangAudioEncoderAac::loadLib(){
	yang_faacEncOpen=(faacEncHandle  (*)(unsigned long sampleRate, uint32_t  numChannels,
			unsigned long *inputSamples, unsigned long *maxOutputBytes))m_lib.loadFunction("faacEncOpen");

	yang_faacEncSetConfiguration=(int32_t  (*)(faacEncHandle hEncoder,faacEncConfigurationPtr config))m_lib.loadFunction("faacEncSetConfiguration");
	yang_faacEncEncode=(int32_t  (*)(faacEncHandle hEncoder, int32_t * inputBuffer, uint32_t  samplesInput,
				 uint8_t *outputBuffer,
				 uint32_t  bufferSize))m_lib.loadFunction("faacEncEncode");
	yang_faacEncGetCurrentConfiguration=(faacEncConfigurationPtr
	  (*)(faacEncHandle hEncoder))m_lib.loadFunction("faacEncGetCurrentConfiguration");
	yang_faacEncClose=(int32_t  (*)(faacEncHandle hEncoder))m_lib.loadFunction("faacEncClose");

}

void YangAudioEncoderAac::unloadLib(){
	yang_faacEncGetCurrentConfiguration=NULL;
	yang_faacEncOpen=NULL;
	yang_faacEncSetConfiguration=NULL;
	yang_faacEncEncode=NULL;
	yang_faacEncClose=NULL;

}
YangAudioEncoderAac::YangAudioEncoderAac() {
	//m_ini = conf;
	nSampleRate = 44100;
	nChannels = 2;
	//nPCMBitSize = 16;
	nInputSamples = 0;
	nMaxOutputBytes = 0;

	m_aacBuffer = NULL;
	//isPause=1;
	ret = 0;

	// nChannels=0;
	nPCMBitSize = 0;
	nInputSamples = 0;
	nMaxOutputBytes = 0;

	//isZb=0;
	isRec = 0;
	hEncoder = NULL;
	isamples = 0;
	maxsample = 0;
	mnInputSamples = 0;
	nRet = 0;

	nBytesRead = 0;
	nPCMBufferSize = 0;
	//isConvert = 0;
	frames = 0;

	//isConvert = 0;
	unloadLib();

}
YangAudioEncoderAac::~YangAudioEncoderAac(void) {

	// Delete WAV buffer
	//delete [] pWAVBuffer;
	closeAac();
	yang_delete(m_aacBuffer);
	unloadLib();
	m_lib.unloadObject();
}



void YangAudioEncoderAac::init(YangAudioInfo *pap) {
	if(m_isInit) return;
	m_lib.loadObject("libfaac");
	loadLib();
	setAudioPara(pap);
	hEncoder = yang_faacEncOpen(nSampleRate, nChannels, &nInputSamples,&nMaxOutputBytes);
	isamples = nInputSamples;
	maxsample = nMaxOutputBytes;
	mnInputSamples = nInputSamples * 2;
	//printf("\n*****is=%d,maxs=%d,mns=%d\n", isamples, maxsample,mnInputSamples);
	m_aacBuffer = new uint8_t[nMaxOutputBytes];
	faacEncConfigurationPtr aconfiguration = yang_faacEncGetCurrentConfiguration(hEncoder);

	aconfiguration->version = MPEG4;

	aconfiguration->aacObjectType = LOW;	//MAIN;//LOW;//MAIN;
	aconfiguration->allowMidside = 1;
	aconfiguration->useTns = 0;
	aconfiguration->shortctl = SHORTCTL_NORMAL;
	//aconfiguration->
	//aconfiguration->nputformat=FAAC_INPUT_16BIT;
	aconfiguration->outputFormat = pap->hasAudioHeader;

	aconfiguration->inputFormat = FAAC_INPUT_16BIT;
	aconfiguration->bitRate = 128000 / nChannels;//config.Audio_Bitrate;//0;//128;//config.Audio_Bitrate;
	nRet = yang_faacEncSetConfiguration(hEncoder, aconfiguration);
	m_isInit=1;


}
//void YangAudioEncoderAac::encoder(uint8_t *p_buf,int32_t bufLen){
int32_t YangAudioEncoderAac::encoder(YangFrame* pframe,YangEncoderCallback* pcallback){
	if(!hEncoder) return 1;
	ret = yang_faacEncEncode(hEncoder,(int32_t *) pframe->payload, isamples, m_aacBuffer, maxsample);

				if (ret > 0&&pcallback){
					pframe->payload=m_aacBuffer;
					pframe->nb=ret;
					pcallback->onAudioData(pframe);
					return Yang_Ok;
				}else
					return 1;
}
/**
void YangAudioEncoderAac::encoder(uint8_t *p_buf,int32_t bufLen,YangAudioEncoderBuffer* pbuf){
	if(!hEncoder) return;
	ret = yang_faacEncEncode(hEncoder,(int32_t *) p_buf, isamples, m_aacBuffer, maxsample);
			//printf("e%d",ret);
			if (ret > 0){
				pbuf->putAudio(m_aacBuffer,ret);
			}
}

void YangAudioEncoderAac::encoder(uint8_t *p_buf,int32_t bufLen,uint8_t *p_out,int32_t *outLen){
	if(!hEncoder) return;
	ret = yang_faacEncEncode(hEncoder, (int32_t *)p_buf, isamples, m_aacBuffer, maxsample);

		if (ret > 0){
			memcpy(p_out,m_aacBuffer,ret);
			*outLen=ret;
		}else{
			*outLen=0;
		}
}
**/

void YangAudioEncoderAac::closeAac() {
	if(hEncoder) yang_faacEncClose(hEncoder);
	hEncoder = NULL;
}

