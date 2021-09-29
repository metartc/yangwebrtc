/*
 * YangPlaySdl.h
 *
 *  Created on: 2020年8月19日
 *      Author: yang
 */

#ifndef YANGPLAYER_SRC_YANGPLAYSDL_H_
#define YANGPLAYER_SRC_YANGPLAYSDL_H_
#include "YangPlayCommon.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/sys/YangLoadLib.h"

class YangPlaySdl {
public:
	YangPlaySdl();
	virtual ~YangPlaySdl();
private:
	YangSdlRect rect;
	SDL_Texture* pTextTexture;
	SDL_Window *m_screen = NULL;
	SDL_Renderer *m_sdlRenderer = NULL;
	int32_t isInit;
	int32_t m_loops;
	YangVideoBuffer *m_videoList;
	int32_t hasData();
public:
	void handle_init();
	void startLoop();
private:
	YangLoadLib m_lib;
	void loadLib();
	    	void unloadLib();
};

#endif /* YANGPLAYER_SRC_YANGPLAYSDL_H_ */
