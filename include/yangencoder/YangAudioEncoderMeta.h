/*
 * YangAudioEncoderMeta.h
 *
 *  Created on: 2020年10月8日
 *      Author: yang
 */

#ifndef YANGENCODER_INCLUDE_YANGAUDIOENCODERMETA_H_
#define YANGENCODER_INCLUDE_YANGAUDIOENCODERMETA_H_
#include "faac.h"
#include "yangutil/sys/YangLoadLib.h"
class YangAudioEncoderMeta{
public:
	YangAudioEncoderMeta();
	~YangAudioEncoderMeta();
	void createMeta(unsigned char *pasc,int *asclen);
private:
	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
	faacEncHandle  (*yang_faacEncOpen)(unsigned long sampleRate, unsigned int numChannels,
			unsigned long *inputSamples,	unsigned long *maxOutputBytes);
	int  (*yang_faacEncSetConfiguration)(faacEncHandle hEncoder,faacEncConfigurationPtr config);
	int  (*yang_faacEncEncode)(faacEncHandle hEncoder, int32_t * inputBuffer, unsigned int samplesInput,
				 unsigned char *outputBuffer, unsigned int bufferSize);
	int  (*yang_faacEncClose)(faacEncHandle hEncoder);
	faacEncConfigurationPtr
	  (*yang_faacEncGetCurrentConfiguration)(faacEncHandle hEncoder);


	int  (*yang_faacEncGetDecoderSpecificInfo)(faacEncHandle hEncoder, unsigned char **ppBuffer,
						  unsigned long *pSizeOfDecoderSpecificInfo);
};



#endif /* YANGENCODER_INCLUDE_YANGAUDIOENCODERMETA_H_ */
