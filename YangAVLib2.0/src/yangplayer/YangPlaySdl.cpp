/*
 * YangPlaySdl.cpp
 *
 *  Created on: 2020年8月19日
 *      Author: yang
 */

#include "YangPlaySdl.h"
#include "yangutil/yang_unistd.h"
/**
void YangPlaySdl::loadLib(){
//yang_speex_lib_get_mode=(const SpeexMode * (*) (int32_t mode))m_lib.loadFunction("");

}

void YangPlaySdl::unloadLib(){

}
YangPlaySdl::YangPlaySdl() {

	isInit = 0;

	pTextTexture = NULL;
	m_videoList = NULL;
	m_loops = 0;
}

YangPlaySdl::~YangPlaySdl() {
	yang_delete(m_videoList);

}

void YangPlaySdl::handle_init() {
	if (!isInit) {
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
			printf("Could not initialize SDL - %s\n", SDL_GetError());
			return;
		}
		printf("\ncur video driver is %s\n", SDL_GetCurrentVideoDriver());

		//m_screen = SDL_CreateWindow("Education", SDL_WINDOWPOS_UNDEFINED,		SDL_WINDOWPOS_UNDEFINED, screen_w, screen_h, //SDL_WINDOW_MAXIMIZED| SDL_WINDOW_BORDERLESS);
			//	SDL_WINDOW_OPENGL);
		if (!m_screen) {
			printf("SDL: could not create window - exiting:%s\n",
					SDL_GetError());
			return;
		}
		//if(1) return;
		m_sdlRenderer = SDL_CreateRenderer(m_screen, -1,	SDL_RENDERER_ACCELERATED);
		isInit = 1;
		//printf("\n******************init finished!***********\n");
	}
}

void YangPlaySdl::startLoop() {

	handle_init();
	//printf("\n*********************************init startLoop\n");
	//int32_t i = 0;
	m_loops = 1;
	int32_t bufLen = 0;
	//int32_t pretime = 100;
	int64_t timestamp = 0;

	//int32_t vsize = 0;
	while (m_loops == 1) {
		//SDL_WaitEvent(&event);
		if (m_videoList->m_size == 0) {
			SDL_Delay(10);
			continue;
		}
		SDL_UpdateTexture(rect.sdlTexture,
				NULL, //&rects[i].rect,
				m_videoList->getVideoIn(&timestamp),
				m_videoList->m_width);
		SDL_RenderClear(m_sdlRenderer);
		SDL_RenderCopy(m_sdlRenderer, rect.sdlTexture, NULL, &rect.sdlRect);
		SDL_RenderPresent(m_sdlRenderer);
	}

	SDL_Quit();
}
/**
 void YangPlaySdl::startLoop() {

 handle_init();
 //printf("\n*********************************init startLoop\n");
 int32_t i = 0;
 m_loops = 1;
 int32_t bufLen = 0;
 int32_t pretime=100;
 int64_t timestamp=0;
 int32_t tsize=0;
 //SDL_Thread *refresh_thread =
 SDL_CreateThread(refresh_video1,NULL,NULL);
 SDL_Event event;
 //YangVideoPlayBuffer *tvpl=NULL;
 int32_t vsize=0;
 while (m_loops == 1) {
 SDL_WaitEvent(&event);
 if(event.type==REFRESH_EVENT){
 if(m_videoList==NULL){
 SDL_Delay(10);
 continue;
 }
 if (hasData() == 0) {
 SDL_Delay(10);
 continue;
 }
 vsize=m_videoList->size();
 //printf("s%d",videoList->at(0)->size);

 //printf("s%d,",tsize);
 if(tsize>0&&rect.sdlTexture){
 SDL_UpdateTexture(rect.sdlTexture, NULL,//&rects[i].rect,
 m_videoList->getVideoData(&bufLen,&timestamp),	m_videoList->width);
 twtime1=timestamp-rect.preTimestamp;
 twtime1=twtime1>2*rect.timestamp?rect.timestamp:twtime1;
 if(twtime1>pretime&&i>0) twtime1=pretime;
 pretime=twtime1;
 rect.preTimestamp=timestamp;
 //	printf("t%d,",twtime);
 }

 SDL_RenderClear(m_sdlRenderer);
 for (i = 0; i < vsize; i++) {
 if (!m_videoList->at(i)->length||!rects[i].sdlTexture)		continue;
 SDL_RenderCopy(m_sdlRenderer, rect.sdlTexture, NULL, &rect.sdlRect);
 //	SDL_RenderCopy(m_sdlRenderer, rects[i].textTexture, NULL, &rects[i].sdlRect);
 //SDL_RenderCopy(m_sdlRenderer, rects[i].textTexture, NULL, &rects[i].rcText);
 }
 SDL_RenderPresent(m_sdlRenderer);
 //SDL_Delay(twtime);
 }else if(event.type==SDL_WINDOWEVENT){
 SDL_GetWindowSize(m_screen,&screen_w,&screen_h);
 }else if (event.type == SDL_QUIT){
 thread_exit=1;
 }else if(event.type==BREAK_EVENT){
 break;
 }

 }

 SDL_Quit();
 }

 **/

