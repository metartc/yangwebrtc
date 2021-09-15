/*
 * YangAudioDecoderOpus.cpp
 *
 *  Created on: 2019年9月29日
 *      Author: yang
 */
#include "YangAudioDecoderOpus.h"

#include "unistd.h"
#include "string.h"
#include "memory.h"
#include <stdio.h>

void YangAudioDecoderOpus::loadLib(){
	yang_opus_decoder_create=(OpusDecoder *(*)( opus_int32 Fs,  int32_t channels,  int32_t *error))m_lib.loadFunction("opus_decoder_create");
	yang_opus_decoder_init=(int32_t (*)(OpusDecoder *st,opus_int32 Fs, int32_t channels ))m_lib.loadFunction("opus_decoder_init") ;
	yang_opus_decode=(int32_t (*)(OpusDecoder *st,const uint8_t *data, opus_int32 len,opus_int16 *pcm,
			int32_t frame_size, int32_t decode_fec))m_lib.loadFunction("opus_decode");
	yang_opus_decoder_ctl=( int32_t (*)(OpusDecoder *st, int32_t request, ...))m_lib.loadFunction("opus_decoder_ctl");
	yang_opus_decoder_destroy=( void (*)(OpusDecoder *st))m_lib.loadFunction("opus_decoder_destroy");
	yang_opus_strerror=(const char *(*)(int32_t error))m_lib.loadFunction("opus_strerror");
}
void YangAudioDecoderOpus::unloadLib(){
	yang_opus_decoder_create=NULL;
	yang_opus_decoder_init=NULL;
	yang_opus_decode=NULL;
	yang_opus_decoder_ctl=NULL;
	yang_opus_decoder_destroy=NULL;
	yang_opus_strerror=NULL;
}

YangAudioDecoderOpus::YangAudioDecoderOpus(YangAudioParam *pini) {
	m_ini=pini;

	isConvert = 0;
	m_frameSize=1024;
	ret=0;
	m_out=NULL;
	m_output=NULL;
	m_decoder=NULL;
	m_sample=pini->sample;
	m_channel = pini->channel;
	m_isMono= (m_channel==1);
	m_frameSize=m_sample/50;
	unloadLib();

}

YangAudioDecoderOpus::~YangAudioDecoderOpus() {
	yang_deleteA(m_out);
	yang_delete(m_output);
	closedec();
	unloadLib();
	m_lib.unloadObject();
}

#define MAX_FRAME_SIZE 8192
void YangAudioDecoderOpus::init() {
	if(m_isInit) return;
	m_lib.loadObject("libopus");
	loadLib();
	//if(m_isMono){
	//		m_frameSize=320;
	//		m_sample=16000;
	//		m_channel=1;
	//	}
	int32_t err=0;
	m_decoder=yang_opus_decoder_create(m_sample, m_channel, &err);
	   if (err<0)
	   {
	      fprintf(stderr, "failed to create an decoder: %s\n", yang_opus_strerror(err));
	      _exit(0);
	    //  return EXIT_FAILURE;
	   }
	m_out=new short[MAX_FRAME_SIZE*m_channel];
	m_output=new uint8_t[m_frameSize*m_channel*2];
	m_alen=m_frameSize*m_channel*2;
	m_isInit=1;

}

int32_t YangAudioDecoderOpus::decode(YangFrame* pframe,YangDecoderCallback* pcallback){
if(!m_decoder) return 1;
     ret=yang_opus_decode(m_decoder, pframe->payload, pframe->nb, m_out, MAX_FRAME_SIZE, 0);

     if(ret==m_frameSize&&pcallback){

         for(int32_t i=0;i<ret*m_channel;i++){
             m_output[2*i]=m_out[i]&0xFF;
             m_output[2*i+1]=(m_out[i]>>8)&0xFF;
         }
         pframe->payload=m_output;
         pframe->nb=ret*2*m_channel;
    	 pcallback->onAudioData(pframe);
    	 return Yang_Ok;
     }
     return 1;
}

void YangAudioDecoderOpus::closedec() {
	if(m_decoder) yang_opus_decoder_destroy(m_decoder);
	m_decoder=NULL;

}
/**
int32_t YangAudioDecoderOpus::getIndex(int32_t puid){
	for(int32_t i=0;i<m_out_audioBuffer->size();i++){
		if(m_out_audioBuffer->at(i)->m_uid==puid){
			return i;
		}
	}

	return -1;
}
**/
