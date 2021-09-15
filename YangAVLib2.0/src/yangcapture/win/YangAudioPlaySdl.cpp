/*
 * YangAudioPlaySdlSdl.cpp
 *
 *  Created on: 2020年9月4日
 *      Author: yang
 */

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
//#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include "YangAudioPlaySdl.h"
#include <vector>
using namespace std;
struct Yang_Audio_Buf {
	Uint8 *audio_chunk;
	Uint32 audio_len;
	Uint8 *audio_pos;
	int32_t volume;
};
Yang_Audio_Buf g_yangAB2;

void YangAudioPlaySdl::loadLib() {
	//yang_speex_lib_get_mode=(const SpeexMode * (*) (int32_t mode))m_lib.loadFunction("");
	yang_SDL_Init = (int32_t SDLCALL (*)(Uint32 flags)) m_lib.loadFunction(
			"SDL_Init");
	yang_SDL_Delay = (void SDLCALL (*)(Uint32 ms)) m_lib.loadFunction(
			"SDL_Delay");
	yang_SDL_memset = (void* SDLCALL (*)(SDL_OUT_BYTECAP(len) void *dst, int32_t c, size_t len))m_lib.loadFunction("SDL_memset");

	yang_SDL_OpenAudio = (int32_t SDLCALL (*)(SDL_AudioSpec *desired,
			SDL_AudioSpec *obtained)) m_lib.loadFunction("SDL_OpenAudio");
	yang_SDL_MixAudio = (void SDLCALL (*)(Uint8 *dst, const Uint8 *src,
			Uint32 len, int32_t volume)) m_lib.loadFunction("SDL_MixAudio");
	yang_SDL_PauseAudio =
			(void SDLCALL (*)(int32_t pause_on)) m_lib.loadFunction(
					"SDL_PauseAudio");
	yang_SDL_CloseAudio = (void SDLCALL (*)(void)) m_lib.loadFunction(
			"SDL_CloseAudio");
	yang_SDL_GetError = (const char* SDLCALL (*)(void))m_lib.loadFunction("SDL_GetError");

}
void YangAudioPlaySdl::unloadLib() {
	yang_SDL_Init = NULL;
	yang_SDL_Delay = NULL;
	yang_SDL_memset = NULL;
	yang_SDL_OpenAudio = NULL;
	yang_SDL_MixAudio = NULL;
	yang_SDL_PauseAudio = NULL;
	yang_SDL_CloseAudio = NULL;
	yang_SDL_GetError = NULL;
}
YangAudioPlaySdl *g_yang_audioPlaysdl = NULL;
YangAudioPlaySdl::YangAudioPlaySdl(YangAudioInfo *pini) {
	m_ini = pini;
	m_size = 0;
	m_loops = 0;
	m_audioPlayCacheNum = pini->audioPlayCacheNum;
	//aIndex = 0;
	ret = 0;
	hasPlay = 0;
	memset(&g_yangAB2, 0, sizeof(Yang_Audio_Buf));
	g_yang_audioPlaysdl = this;
	m_preProcess = NULL;
	m_in_audioBuffer = NULL;
	m_timestamp = 0;
	m_audioDataLen = 0;
	unloadLib();
}

YangAudioPlaySdl::~YangAudioPlaySdl() {
	m_ini = NULL;
	m_preProcess = NULL;
	m_in_audioBuffer = NULL;
	closeAudio();
	unloadLib();
	m_lib.unloadObject();
	g_yang_audioPlaysdl = NULL;
}

void fill_audio(void *udata, Uint8 *stream, int32_t len) {
	if (g_yang_audioPlaysdl)
		g_yang_audioPlaysdl->yang_SDL_memset(stream, 0, len);
	if (g_yangAB2.audio_len == 0)
		return;
	len = (len > g_yangAB2.audio_len ? g_yangAB2.audio_len : len); //  Mix  as  much  data  as  possible
	//memcpy(stream,g_yangAb.audio_pos,len);
	if (g_yang_audioPlaysdl)
		g_yang_audioPlaysdl->yang_SDL_MixAudio(stream, g_yangAB2.audio_pos, len,
		SDL_MIX_MAXVOLUME);
	g_yangAB2.audio_pos += len;
	g_yangAB2.audio_len -= len;
}

int32_t YangAudioPlaySdl::getData(uint8_t *pcm_write) {
	//YangFrame* frame=NULL;
	uint8_t *tmp = NULL;
	for (size_t i = 0; i < m_in_audioBuffer->size(); i++) {
		if (m_in_audioBuffer->at(i) && m_in_audioBuffer->at(i)->m_size > 0) {
			//YangFrame* frame=m_in_audioBuffer->at(i)->getAudios();

			tmp = m_in_audioBuffer->at(i)->getAudios(&m_audioFrame);
			if (tmp) {
				//if (m_preProcess)	m_preProcess->preprocess_run((short*) tmp);
				if (i == 0) {
					memcpy(pcm_write, tmp, m_size);
				} else {
					m_mix.yangMix1(pcm_write, tmp, m_size, 128);
				}
			}
			tmp = NULL;

			if (m_in_audioBuffer->at(i)->m_size > m_audioPlayCacheNum)
				m_in_audioBuffer->at(i)->resetIndex();
		}
	}
	return Yang_Ok;
}

//CMyMakeWave mw1;
void YangAudioPlaySdl::init() {
	if (isInit == 1)
		return;
	m_lib.loadObject("libSDL2");
	loadLib();
	if (yang_SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		fprintf(stderr, "Count not initialize SDL - %s\n", yang_SDL_GetError());
		exit(1);
	}
	if (m_ini->usingMono) {
		m_frames = 320;
		m_channel = 1;
		m_sample = 16000;
	} else {
		m_frames = 1024;
	}
	m_size = m_frames * m_channel * 2;
	wanted.freq = m_sample;
	wanted.channels = m_channel; // 1 = mono, 2 = stereo
	wanted.samples = m_frames; // Good low-latency value for callback
	wanted.format = AUDIO_S16;
	wanted.callback = fill_audio;
	wanted.userdata = NULL;
	if (yang_SDL_OpenAudio(&wanted, NULL) < 0) {
		fprintf(stderr, "\nFailed to open audio: %s\n", yang_SDL_GetError());
		return;
	}
	isInit = 1;
	//mw1.start("/home/yang/bmp/out1.wav");

}

void YangAudioPlaySdl::closeAudio() {

	yang_SDL_CloseAudio();
	//m_params=NULL;
	//m_buffer[0] = NULL;
}

void YangAudioPlaySdl::stopLoop() {

	m_loops = 0;
}

//long tt=0;

void YangAudioPlaySdl::playSDL(uint8_t *p_data) {
	//g_yangAb[0].audio_len=4096;
	//g_yangAb[0].audio_chunk=p_data;
	//g_yangAb[0].audio_pos=p_data;
	if (hasPlay == 0) {
		yang_SDL_PauseAudio(0);
		hasPlay = 1;
	}
	g_yangAB2.audio_len = m_size;
	g_yangAB2.audio_chunk = p_data;
	g_yangAB2.audio_pos = p_data;
	while (g_yangAB2.audio_len > 0)		//Wait until finish
		yang_SDL_Delay(1);
}
void YangAudioPlaySdl::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal) {
	m_in_audioBuffer = pal;
}
int32_t YangAudioPlaySdl::hasData() {
	if (!m_in_audioBuffer)
		return 0;
	for (int32_t i = 0; i < (int) m_in_audioBuffer->size(); i++) {
		//al=audioList->at(i);
		//if(audioList->at(i)==NULL) printf("\n%d__%d*****************has null value\n",audioList->size(),i);
		if (m_in_audioBuffer->at(i)->m_size > 0)
			return 1;
	}
	//al=NULL;
	return 0;
}

/**
 #define SIZE_AUDIO_FRAME 4096
 void YangAudioPlaySdl::startLoop() {

 m_loops = 1;
 //int32_t rc = 0;
 //int32_t bufLen;
 yang_SDL_PauseAudio(0);
 //int32_t i = 0;
 uint8_t *pcm = new uint8_t[SIZE_AUDIO_FRAME * 2];
 uint8_t *tmp = NULL;
 //printf("\n*************************PlaySDLAudio is starting.....\n");
 int32_t talen = 4096;
 if (m_ini->usingMono) {
 talen = 640;
 }
 while (m_loops == 1) {
 if (hasData() == 0) {
 usleep(20);
 continue;
 }
 //if (m_in_audioBuffer->size() >= 1)		printf("v%d-%d,", m_in_audioBuffer->size(),m_in_audioBuffer->at(0)->m_size);
 //mixAudio();
 //memset(m_buffer[i],0,4096);
 memset(pcm, 0, SIZE_AUDIO_FRAME);
 for (int32_t i = 0; (int) i < m_in_audioBuffer->size(); i++) {
 tmp = m_in_audioBuffer->at(i)->getAudios();

 //tmp = m_in_audioBuffer->at(i)->getAudios();
 //	if (tmp)	yangMixs(pcm, tmp, SIZE_AUDIO_FRAME, SDL_MIX_MAXVOLUME);
 //yangMix1(m_in_audioBuffer->at(i)->getAudios(),pcm);
 //SDL_MixAudio(pcm, m_in_audioBuffer->at(i)->getAudios(), SIZE_AUDIO_FRAME, SDL_MIX_MAXVOLUME);
 }
 //printf("t%d,",talen);
 //memcpy(pcm,tmp,talen);
 tmp = NULL;
 g_yangAB.audio_len = talen;
 g_yangAB.audio_chunk = pcm;
 g_yangAB.audio_pos = pcm;

 while (g_yangAB.audio_len > 0)		//Wait until finish
 yang_SDL_Delay(1);

 //aecHandle(pcm);
 /**yangAudioNum=m_in_audioBuffer->size();
 for(i=0;i<yangAudioNum;i++){
 if(m_buffer[i]==NULL) m_buffer[i]=(uint8_t*)malloc(4096);
 memset(m_buffer[i],0,4096);
 if(m_in_audioBuffer->at(i)->size>0) m_in_audioBuffer->at(i)->getAudio(m_buffer[i]);
 g_yangAb[i].audio_len=4096;
 g_yangAb[i].audio_chunk=m_buffer[i];
 g_yangAb[i].audio_pos=m_buffer[i];
 }
 while(g_yangAb[yangAudioNum-1].audio_len>0)//Wait until finish
 SDL_Delay(1);**/
//printf("a,");
/**}
 closeAudio();
 yang_deleteA(pcm);
 //mw1.stop();
 //	printf("\n********************AudioCapture stop.......\n");

 //free(m_buffer);
 //m_buffer = NULL;

 }
 **/

