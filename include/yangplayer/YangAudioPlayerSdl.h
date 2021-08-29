/*
 * YangAudioPlayerSdl.h
 *
 *  Created on: 2020年9月4日
 *      Author: yang
 */

#ifndef YANGPLAYER_INCLUDE_YANGAUDIOPLAYSDL1_H_
#define YANGPLAYER_INCLUDE_YANGAUDIOPLAYSDL1_H_
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/sys/YangThread.h"
#include "yangutil/sys/YangLoadLib.h"
#include <SDL2/SDL.h>
class YangAudioPlayerSdl:public YangThread{
public:
	YangAudioPlayerSdl(YangAudioInfo *pini);
	~YangAudioPlayerSdl();
	void init();
	void* SDLCALL (*yang_SDL_memset)(SDL_OUT_BYTECAP(len) void *dst, int c, size_t len);
	void SDLCALL (*yang_SDL_MixAudio)(Uint8 * dst, const Uint8 * src, Uint32 len, int volume);
	void setAudioList(YangAudioPlayBuffer *pal);
	int m_isStart;
	void stop();

	int m_frames;
	int m_channel;
	int m_sample;
	static void fill_audio(void *udata, Uint8 *stream, int len);
protected:
	void startLoop();
	void stopLoop();
	void run();
	// unsigned char *m_buffer[YangAudioBufferMaxNum];
private:
	YangAudioInfo *m_ini;
	YangAudioPlayBuffer *m_in_audioBuffer;
	static YangAudioPlayerSdl* m_instance;
	int isInit;

	void closeAudio();
	int ret;
	int m_size;
	int m_loops;

	SDL_AudioSpec wanted;
	void playSDL(unsigned char *p_data);
	void initSDL();
	void startLoopSDL();
	YangLoadLib m_lib,m_lib1;
	void loadLib();
	void unloadLib();
	int SDLCALL (*yang_SDL_Init)(Uint32 flags);
	void SDLCALL (*yang_SDL_Delay)(Uint32 ms);

	const char *SDLCALL (*yang_SDL_GetError)(void);

	int SDLCALL (*yang_SDL_OpenAudio)(SDL_AudioSpec * desired, SDL_AudioSpec * obtained);

	void SDLCALL (*yang_SDL_PauseAudio)(int pause_on);
	void SDLCALL (*yang_SDL_CloseAudio)(void);

};

#endif /* YANGPLAYER_INCLUDE_YANGAUDIOPLAYSDL_H_ */
