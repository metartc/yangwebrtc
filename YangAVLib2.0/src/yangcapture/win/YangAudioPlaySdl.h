/*
 * YangAudioPlaySdl.h
 *
 *  Created on: 2020年9月4日
 *      Author: yang
 */

#ifndef RC_YangAudioPlaySdl_H_
#define RC_YangAudioPlaySdl_H_
#include <yangavutil/audio/YangAudioMix.h>
#include <yangavutil/audio/YangAecBase.h>
#include <yangutil/yangavinfotype.h>

#include <yangutil/sys/YangLoadLib.h>
#include <yangutil/sys/YangThread.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_audio.h"
#include <vector>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
//#include "../include/YangAudioPlay.h"
using namespace std;

class YangAudioPlaySdl :public YangThread//:public YangAudioPlay
{
public:
	YangAudioPlaySdl(YangAudioInfo *pini);
	~YangAudioPlaySdl();

	void init();
	void setAec(YangAecBase *paec);
	void playSDL(uint8_t *p_data);
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal);
    vector<YangAudioPlayBuffer*> *m_in_audioBuffer;
    int32_t getRenderData(uint8_t *pdata);
    int32_t hasData();

	void closeAudio();

	int32_t m_audioDataLen;
	int m_hasRenderEcho;
	int m_isStart;
	void* SDLCALL (*yang_SDL_memset)(SDL_OUT_BYTECAP(len) void *dst, int32_t c, size_t len);
	void SDLCALL (*yang_SDL_MixAudio)(Uint8 * dst, const Uint8 * src, Uint32 len, int32_t volume);
    void stop();
protected:
    void run();
	void startLoop();
	void stopLoop();

	  YangAudioMix m_mix;
	  YangFrame m_audioFrame;
	  YangAecBase *m_aec;
private:
	YangAudioInfo *m_ini;
	int32_t m_size;
	int32_t isInit;
	int32_t hasPlay;
	int64_t m_timestamp;
	int32_t m_frames;
	int32_t m_channel;
	int32_t m_sample;
	int32_t m_audioPlayCacheNum;
	int32_t ret;

	int32_t m_loops;
	SDL_AudioSpec wanted;

	void initSDL();
    YangLoadLib m_lib;
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
