/*
 * YangAudioPlaySdl.h
 *
 *  Created on: 2020年9月4日
 *      Author: yang
 */

#ifndef RC_YANGWinAUDIOPLAYSDL_H_
#define RC_YANGWinAUDIOPLAYSDL_H_
#include <yangavutil/audio/YangAudioMix.h>
#include <yangavutil/audio/YangPreProcess.h>
#include <yangutil/yangavinfotype.h>
#include "yangutil/sys/YangLoadLib.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_audio.h"
#include <vector>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
//#include "../include/YangAudioPlay.h"
using namespace std;

class YangAudioPlaySdl //:public YangAudioPlay
{
public:
	YangAudioPlaySdl(YangAudioInfo *pini);
	~YangAudioPlaySdl();
	YangPreProcess *m_preProcess;
	void init();
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal);
	void playSDL(uint8_t *p_data);
	int32_t hasData();

	void closeAudio();
	int32_t getData(uint8_t *pdata);
	int32_t m_audioDataLen;
	YangFrame m_audioFrame;
	void* SDLCALL (*yang_SDL_memset)(SDL_OUT_BYTECAP(len) void *dst, int32_t c, size_t len);
	void SDLCALL (*yang_SDL_MixAudio)(Uint8 * dst, const Uint8 * src, Uint32 len, int32_t volume);
protected:
	void startLoop();
	void stopLoop();

	// uint8_t *m_buffer[YangAudioBufferMaxNum];
private:
	YangAudioInfo *m_ini;
	vector<YangAudioPlayBuffer*> *m_in_audioBuffer;
	//  YangAudioParam *m_ini;
	int32_t isInit;
	int32_t hasPlay;
	int64_t m_timestamp;
	// YangAecBase *m_ace;
	YangAudioMix m_mix;
	int32_t m_frames;
	int32_t m_channel;
	int32_t m_sample;
	int32_t m_audioPlayCacheNum;
	int32_t ret;
	int32_t m_size;
	int32_t m_loops;
	//uint8_t *m_buffer;
	SDL_AudioSpec wanted;

	void initSDL();
	//void startLoopSDL();
	YangLoadLib m_lib;//,m_lib1;
	void loadLib();
	void unloadLib();
	int32_t SDLCALL (*yang_SDL_Init)(Uint32 flags);
	void SDLCALL (*yang_SDL_Delay)(Uint32 ms);

	const char *SDLCALL (*yang_SDL_GetError)(void);

	int32_t SDLCALL (*yang_SDL_OpenAudio)(SDL_AudioSpec * desired, SDL_AudioSpec * obtained);

	void SDLCALL (*yang_SDL_PauseAudio)(int32_t pause_on);
	void SDLCALL (*yang_SDL_CloseAudio)(void);

};

#endif /* YANGPLAYER_SRC_YANGAUDIOPLAYSDL_H_ */
