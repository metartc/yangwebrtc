/*
 * YangVideoPlay.cpp
 *
 *  Created on: 2019年9月27日
 *      Author: yang
 */

#include <yangplayer/YangVideoPlay.h>
#include "unistd.h"
//#include <stdint.h>
#define yang_av_builtin_constant_p(x) 0
#define yang_AV_CEIL_RSHIFT(a,b) (!yang_av_builtin_constant_p(b) ? -((-(a)) >> (b)) \
                                                       : ((a) + (1<<(b)) - 1) >> (b))

#define MAX_PACK_SIZE 6220800
//int32_t twtime = 20;
//int32_t thread_exit = 0;
int32_t SDLCALL (*yang_SDL_WaitEvent)(SDL_Event *event);
int32_t SDLCALL (*yang_SDL_PushEvent)(SDL_Event *event);
void SDLCALL (*yang_SDL_Delay)(Uint32 ms);

void YangVideoPlay::loadLib() {
	//yang_speex_lib_get_mode=(const SpeexMode * (*) (int32_t mode))m_lib.loadFunction("");
	yang_SDL_Init = (int32_t SDLCALL (*)(Uint32 flags)) m_lib.loadFunction(
			"SDL_Init");
	yang_SDL_EventState =
			(Uint8 SDLCALL (*)(Uint32 type, int32_t state)) m_lib.loadFunction(
					"SDL_EventState");
	yang_SDL_CreateThread = (SDL_Thread* SDLCALL
	(*)(SDL_ThreadFunction fn, const char *name, void *data))m_lib.loadFunction("SDL_CreateThread");;
	yang_SDL_WaitEvent = (int32_t SDLCALL (*)(SDL_Event *event)) m_lib.loadFunction(
			"SDL_WaitEvent");
	;
	yang_SDL_PushEvent = (int32_t SDLCALL (*)(SDL_Event *event)) m_lib.loadFunction(
			"SDL_PushEvent");
	;
	yang_SDL_Delay = (void SDLCALL (*)(Uint32 ms)) m_lib.loadFunction(
			"SDL_Delay");
	;
	yang_SDL_GetDisplayUsableBounds = (int32_t SDLCALL (*)(int32_t displayIndex,
			SDL_Rect *rect)) m_lib.loadFunction("SDL_GetDisplayUsableBounds");
	;

	yang_SDL_CreateWindow = (SDL_Window* SDLCALL (*)(const char *title,
			int32_t x, int32_t y, int32_t w,
			int32_t h, Uint32 flags))m_lib.loadFunction("SDL_CreateWindow");
	yang_SDL_CreateWindowFrom = (SDL_Window* SDLCALL (*)(const void *data))m_lib.loadFunction("SDL_CreateWindowFrom");
	yang_SDL_CreateRenderer = (SDL_Renderer* SDLCALL (*)(SDL_Window * window,int32_t index, Uint32 flags))m_lib.loadFunction("SDL_CreateRenderer");
	yang_SDL_CreateTexture = (SDL_Texture* SDLCALL (*)(SDL_Renderer * renderer, Uint32 format, int32_t access, int32_t w,int32_t h))m_lib.loadFunction("SDL_CreateTexture");
	yang_SDL_UpdateTexture =
			(int32_t SDLCALL (*)(SDL_Texture *texture, const SDL_Rect *rect,
					const void *pixels, int32_t pitch)) m_lib.loadFunction(
					"SDL_UpdateTexture");
	yang_SDL_RenderClear =
			(int32_t SDLCALL (*)(SDL_Renderer *renderer)) m_lib.loadFunction(
					"SDL_RenderClear");
	yang_SDL_RenderCopy = (int32_t SDLCALL (*)(SDL_Renderer *renderer,
			SDL_Texture *texture, const SDL_Rect *srcrect,
			const SDL_Rect *dstrect)) m_lib.loadFunction("SDL_RenderCopy");
	yang_SDL_RenderPresent =
			(void SDLCALL (*)(SDL_Renderer *renderer)) m_lib.loadFunction(
					"SDL_RenderPresent");
	yang_SDL_SetRenderDrawColor = (int32_t SDLCALL (*)(SDL_Renderer *renderer,
			Uint8 r, Uint8 g, Uint8 b, Uint8 a)) m_lib.loadFunction(
			"SDL_SetRenderDrawColor");
	yang_SDL_RenderFillRect = (int32_t SDLCALL (*)(SDL_Renderer *renderer,
			const SDL_Rect *rect)) m_lib.loadFunction("SDL_RenderFillRect");
	yang_SDL_DestroyTexture =
			(void SDLCALL (*)(SDL_Texture *texture)) m_lib.loadFunction(
					"SDL_DestroyTexture");
	yang_SDL_DestroyRenderer =
			(void SDLCALL (*)(SDL_Renderer *renderer)) m_lib.loadFunction(
					"SDL_DestroyRenderer");
	yang_SDL_DestroyWindow =
			(void SDLCALL (*)(SDL_Window *window)) m_lib.loadFunction(
					"SDL_DestroyWindow");
	yang_SDL_FreeSurface =
			(void SDLCALL (*)(SDL_Surface *surface)) m_lib.loadFunction(
					"SDL_FreeSurface");
	yang_SDL_CreateTextureFromSurface = (SDL_Texture* SDLCALL (*)(SDL_Renderer * renderer, SDL_Surface * surface))m_lib.loadFunction("SDL_CreateTextureFromSurface");
	yang_SDL_GetError = (const char* SDLCALL (*)(void))m_lib.loadFunction("SDL_GetError");

	yang_TTF_Init = (int32_t SDLCALL (*)(void)) m_lib1.loadFunction("TTF_Init");
	yang_TTF_OpenFont = (TTF_Font* SDLCALL (*)(const char *file, int32_t ptsize))m_lib1.loadFunction("TTF_OpenFont");
	yang_TTF_RenderUTF8_Blended = (SDL_Surface* SDLCALL (*)(TTF_Font *font, const char *text, SDL_Color fg))m_lib1.loadFunction("TTF_RenderUTF8_Blended");
	yang_TTF_CloseFont = (void SDLCALL (*)(TTF_Font *font)) m_lib1.loadFunction(
			"TTF_CloseFont");

}
void YangVideoPlay::unloadLib() {
	yang_SDL_Init = NULL;
	yang_SDL_EventState = NULL;
	yang_SDL_CreateThread = NULL;
	yang_SDL_WaitEvent = NULL;
	yang_SDL_PushEvent = NULL;
	yang_SDL_Delay = NULL;
	yang_SDL_GetDisplayUsableBounds = NULL;
	yang_SDL_CreateWindow = NULL;
	yang_SDL_CreateWindowFrom = NULL;
	yang_SDL_CreateRenderer = NULL;
	yang_SDL_CreateTexture = NULL;
	yang_SDL_UpdateTexture = NULL;
	yang_SDL_RenderClear = NULL;
	yang_SDL_RenderCopy = NULL;
	yang_SDL_RenderPresent = NULL;
	yang_SDL_SetRenderDrawColor = NULL;
	yang_SDL_RenderFillRect = NULL;
	yang_SDL_DestroyTexture = NULL;
	yang_SDL_DestroyRenderer = NULL;
	yang_SDL_DestroyWindow = NULL;
	yang_SDL_FreeSurface = NULL;
	yang_SDL_CreateTextureFromSurface = NULL;
	yang_SDL_GetError = NULL;
	yang_SDL_Init = NULL;
}
int32_t YangVideoPlay::hasData() {
	//printf("\nhz%d,",m_videoList->size());
	if (m_videoList->size() == 0)
		return 0;
    for (uint32_t  i = 0; i < m_videoList->size(); i++) {

		if (m_videoList->at(i)->m_size > 0) {
			//printf("\nsz%d,",m_videoList->at(i)->m_size);
			return 1;
		}
	}
	return 0;
}
void YangVideoPlay::setVideoList(vector<YangVideoBuffer*> *pvideoList) {
	m_videoList = pvideoList;
}
void YangVideoPlay::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangVideoPlay::stop() {
	stopLoop();
}
void YangVideoPlay::stopLoop() {
	m_loops = 0;
}
void YangVideoPlay::startLoop() {

	handle_init();
	printf("\n*********************************init startLoop\n");
	//int32_t i = 0;
	m_loops = 1;
	//int32_t bufLen = 0;
	//int32_t pretime = 100;
	//int64_t timestamp = 0;
	int32_t tsize = 0;
	//SDL_Thread *refresh_thread = yang_SDL_CreateThread(refresh_video, NULL,	NULL);
	//SDL_Event event;
	YangVideoBuffer *tvpl = NULL;
	YangFrame frame;
    //int32_t vsize = 0;
	while (m_loops == 1) {
		//yang_SDL_WaitEvent(&event);
		//printf("\n%d-%d....",event.type,REFRESH_EVENT);
		//if (event.type == REFRESH_EVENT) {
			if (m_videoList == NULL) {
				yang_SDL_Delay(10);
				printf("\nvideoList is NULL");
				continue;
			}
			if (hasData() == 0) {
				yang_SDL_Delay(10);

				continue;
			}
			tvpl = m_videoList->front();
			tsize = tvpl->m_size;
			if (m_sdlTexture == NULL) {
				m_sdlTexture = yang_SDL_CreateTexture(m_sdlRenderer,
						//pcolorType == 1 ? yang_SDL_PIXELFORMAT_IYUV : yang_SDL_PIXELFORMAT_NV12,
								 SDL_PIXELFORMAT_IYUV, //SDL_PIXELFORMAT_NV12,
								SDL_TEXTUREACCESS_STREAMING, tvpl->m_width, tvpl->m_height);
				//initSdlWin(2, tvpl->m_width, tvpl->m_height, tvpl->m_frames);
			}

			//if (!rects.sdlTexture)				continue;
			/**	if (tsize > 0 && rects.sdlTexture) {
				yang_SDL_UpdateTexture(rects.sdlTexture, NULL, //&rects[i].rect,
						tvpl->getVideoIn(&timestamp), tvpl->m_width);
				twtime=timestamp-rects.preTimestamp;
				 twtime=twtime>2*rects.timestamp?rects.timestamp:twtime;
				 if(twtime>pretime&&i>0) twtime=pretime;
				 pretime=twtime;
				 rects.preTimestamp=timestamp;
				//printf("\n...............vsize0===%d,fps==%d,", tsize,tvpl->m_frames);
				//printf("t%d,",twtime);
			}**/

			//}
			uint8_t* tmp=tvpl->getVideoRef(&frame);
			if(tmp) yang_SDL_UpdateTexture(m_sdlTexture, NULL, //&rects[i].rect,
									tmp, tvpl->m_width);
			yang_SDL_RenderClear(m_sdlRenderer);
			printf("\n...............vsize1===%d,", tsize);
			yang_SDL_RenderCopy(m_sdlRenderer, m_sdlTexture, NULL,NULL);
			//yang_SDL_RenderCopy(m_sdlRenderer, rects.textTexture, NULL,	&rects.rcText);
			yang_SDL_RenderPresent(m_sdlRenderer);
			tvpl = NULL;
			//yang_SDL_Delay(20);
		/**} else if (event.type == SDL_WINDOWEVENT) {
			//SDL_GetWindowSize(m_screen,&screen_w,&screen_h);
		} else if (event.type == SDL_QUIT) {
			thread_exit = 1;
		} else if (event.type == BREAK_EVENT) {
			break;
		}**/

	}

	//SDL_Quit();
}
YangVideoPlay::YangVideoPlay() {
	//memset(&rects, 0, sizeof(YangSdlRect)) ;
	/**for(int32_t i=0;i<YangSdlRectNum;i++){
	 rects[i].textSurface=NULL;
	 rects[i].textTexture=NULL;
	 rects[i].sdlTexture=NULL;
	 }**/
	m_isStart = 0;
	isInit = 0;
	//font = NULL;
	//pTextTexture = NULL;
	m_sdlTexture=NULL;
	m_videoList = NULL;
	m_loops = 0;
	unloadLib();
}

YangVideoPlay::~YangVideoPlay() {
	m_videoList = NULL;
	closeVideoPlay();
	unloadLib();
	m_lib.unloadObject();
}
void YangVideoPlay::setWindowSize(int32_t p_sw, int32_t p_sh, int32_t p_pw, int32_t p_ph) {
	screen_w = p_sw, screen_h = p_sh;
	pixel_w = p_pw, pixel_h = p_ph;
}

void YangVideoPlay::handle_init() {
	if (!isInit) {
		m_lib.loadObject("libSDL2");
		m_lib1.loadObject("libSDL2_ttf");
		loadLib();
		if (yang_SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
			printf("Could not initialize SDL - %s\n", yang_SDL_GetError());
			return;
		}
		yang_SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
		yang_SDL_EventState(SDL_USEREVENT, SDL_IGNORE);
		//	printf("\ncur video driver is %s\n",SDL_GetCurrentVideoDriver());
		//yang_SDL_SetVideoModel();
		m_screen = yang_SDL_CreateWindow("Education", SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED, screen_w, screen_h, //yang_SDL_WINDOW_MAXIMIZED| yang_SDL_WINDOW_BORDERLESS);
				SDL_WINDOW_OPENGL);
		if (!m_screen) {
			printf("SDL: could not create window - exiting:%s\n",
					yang_SDL_GetError());
			return;
		}
		//if(1) return;
		m_sdlRenderer = yang_SDL_CreateRenderer(m_screen, -1,
				SDL_RENDERER_ACCELERATED);
		SDL_Rect trect;
		yang_SDL_GetDisplayUsableBounds(0, &trect);
		screen_w = trect.w;
		screen_h = trect.h;
		printf("\n****************SDL init w=%d,h=%d\n", screen_w, screen_h);
		//splitScreen(1);
		//for (int32_t i = 0; i < YangSdlRectNum; i++) {
		//rects.sdlRect.w = screen_w;
		//rects.sdlRect.h = screen_h;
		//rects.sdlRect.x = 0;
		//rects.sdlRect.y = 0;
		//}
		//if (yang_TTF_Init() == -1) {
		//	printf("Could not initialize SDL TTF - %s\n", yang_SDL_GetError());
		//	return;
		//}
		//char file1[180];
		//char file_path_getcwd[180];
		//memset(file1, 0, 180);
		//memset(file_path_getcwd, 0, 180);
		//getcwd(file_path_getcwd, 180);
		//sprintf(file1, "%s/lib/sim1.ttc", file_path_getcwd);
		//font = yang_TTF_OpenFont(file1, 26);
		//font = yang_TTF_OpenFont("/usr/share/fonts/truetype/freefont/simsun.ttc",26);
		//SDL_Color color2 = { 255, 255, 255 };

		//initText(&rects, font, "视频1", color2);
		//initSdlWin
		isInit = 1;
		printf("\n******************init finished!***********\n");
	}
}
void YangVideoPlay::init() {
	//initSdlWin(1,this->rects[0],);
	handle_init();

}

void YangVideoPlay::splitScreen(int32_t pScreeNum) {
	screenNum = pScreeNum;

}
/**
void YangVideoPlay::initText(YangSdlRect *wrc, TTF_Font *pfont,
		const char *label, SDL_Color color) {
	wrc->textSurface = yang_TTF_RenderUTF8_Blended(pfont, label, color);
	wrc->textTexture = yang_SDL_CreateTextureFromSurface(m_sdlRenderer,
			wrc->textSurface);
	wrc->rcText.w = 100;
	wrc->rcText.h = 25;
}
void YangVideoPlay::initSdlWin(int32_t pcolorType, int32_t w, int32_t h, int32_t pfps) {
	////if (!isInit)	return;
	YangSdlRect *wrc = NULL;//&rects;
	if (wrc->sdlTexture == NULL) {
		wrc->sdlTexture = yang_SDL_CreateTexture(m_sdlRenderer,
		//pcolorType == 1 ? yang_SDL_PIXELFORMAT_IYUV : yang_SDL_PIXELFORMAT_NV12,
				pcolorType == 1 ? SDL_PIXELFORMAT_YV12 : SDL_PIXELFORMAT_IYUV, //SDL_PIXELFORMAT_NV12,
				SDL_TEXTUREACCESS_STREAMING, w, h);
		//printf("\n*********************************init sdlTexture\n");
	}
	wrc->timestamp = 1000 / pfps;
	wrc->sdlRect.w = w;
	wrc->sdlRect.h = h;
	printf(
			"\n***********************initSdlWin********w=%d,h=%d*********************\n",
			wrc->sdlRect.w, wrc->sdlRect.h);
	//wrc->sdlRect.x = x;
	//wrc->sdlRect.y = y;
	//wrc->rect.w = w;
	//wrc->rect.h = h;
	//wrc->rect.x = x;
	//wrc->rect.y = y;
	wrc->rcText.w = 100;
	wrc->rcText.h = 25;
	wrc = NULL;
	//wrc->rcText.x = x;
	//wrc->rcText.y = y;
	//splitScreen(m_videoList->size());
}
void YangVideoPlay::initWin(YangSdlRect *wrc, int32_t w, int32_t h, int32_t x, int32_t y) {

	wrc->sdlRect.w = w;
	wrc->sdlRect.h = h;
	wrc->sdlRect.x = x;
	wrc->sdlRect.y = y;
	wrc->rcText.w = 100;
	wrc->rcText.h = 25;
	wrc->rcText.x = x;
	wrc->rcText.y = y;
}**/
//uint8_t g_src[1280*720*3/2];
//void YangVideoPlay::yuv_show(uint8_t *buf[], int32_t wrap[], int32_t xsize, int32_t ysize){
/**void YangVideoPlay::yuv_show(uint8_t *buf, int32_t p_width) {
 yang_SDL_UpdateTexture(rects[0].sdlTexture, &rects[0].rect, buf, p_width);
 //	printf("%d,",p_ylen);
 //yang_SDL_UpdateYUVTexture(gSdlTexture, &rect,buf,p_ylen,u,p_uLen,v,p_uLen);
 yang_SDL_RenderClear(m_sdlRenderer);
 handle_yuv_show(&rects[0]);
 if (screenNum > 1)
 handle_yuv_show(&rects[1]);
 if (screenNum > 2)
 handle_yuv_show(&rects[2]);
 if (screenNum > 3)
 handle_yuv_show(&rects[3]);

 yang_SDL_RenderPresent(m_sdlRenderer);
 //Delay 40ms
 yang_SDL_Delay(100);
 yang_SDL_Event event;
 yang_SDL_WaitEvent(&event);
 if (event.type == yang_SDL_QUIT)
 exit(0);
 }

 void YangVideoPlay::handle_yuv_show(YangSdlRect *wrc) {
 yang_SDL_RenderCopy(m_sdlRenderer, wrc->sdlTexture, &wrc->rect, &wrc->sdlRect);
 yang_SDL_RenderCopy(m_sdlRenderer, wrc->textTexture, NULL, &wrc->rcText);
 }

 void YangVideoPlay::handle_yuv_texut(YangSdlRect *wrc, uint8_t* buf, int32_t p_width) {
 yang_SDL_UpdateTexture(wrc->sdlTexture, &wrc->rect, buf, p_width);
 }**/

void YangVideoPlay::closeVideoPlay() {
	//for(int32_t i=0;i<YangSdlRectNum;i++){
	if (m_sdlTexture != NULL) {
		yang_SDL_DestroyTexture(m_sdlTexture);
		m_sdlTexture= NULL;
	}
	/**
	if (rects.textTexture != NULL) {
		yang_SDL_DestroyTexture(rects.textTexture);
		rects.textTexture = NULL;
	}**/
	//}
	yang_SDL_DestroyRenderer(m_sdlRenderer);
	m_sdlRenderer = NULL;
	//yang_TTF_CloseFont(font);
	//rects[0].sdlTexture = NULL;

}
/**
 double compute_target_delay(double delay, VideoState *is) {
 double sync_threshold, diff = 0;

 //update delay to follow master synchronisation source
 // if video is slave, we try to correct big delays by duplicating or
 // deleting a frame
 diff = get_clock(&is->vidclk) - get_clock(&is->audclk);

 // skip or repeat frame. We take into account the delay to compute the
 // threshold. I still don't know if it is the best guess
 sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
 if (!isnan(diff) && fabs(diff) < is->max_frame_duration) {
 if (diff <= -sync_threshold)
 delay = FFMAX(0, delay + diff);
 else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)
 delay = delay + diff;
 else if (diff >= sync_threshold)
 delay = 2 * delay;
 }

 printf("video: delay=%0.3f A-V=%f\n", delay, -diff);

 return delay;
 }
 void video_refresh(void *opaque, double *remaining_time) {
 VideoState *is = (VideoState*)opaque;
 double time;

 if (is->video_st) {
 retry:
 if (frame_queue_nb_remaining(&is->pictq) == 0) {
 // nothing to do, no picture to display in the queue
 }
 else {
 double last_duration, duration, delay;
 Frame *vp, *lastvp;

 //dequeue the picture
 lastvp = frame_queue_peek_last(&is->pictq); // f->rindex
 vp = frame_queue_peek(&is->pictq); // f->rindex + f->rindex_shown

 if (vp->serial != is->videoq.serial) {
 frame_queue_next(&is->pictq);
 goto retry;
 }

 if (lastvp->serial != vp->serial)
 is->frame_timer = av_gettime_relative() / 1000000.0;

 if (is->paused)
 goto display;

 // compute nominal last_duration
 last_duration = vp_duration(is, lastvp, vp);
 delay = compute_target_delay(last_duration, is);

 time = av_gettime_relative() / 1000000.0;
 if (time < is->frame_timer + delay) {
 *remaining_time = FFMIN(is->frame_timer + delay - time, *remaining_time);
 goto display;
 }

 is->frame_timer += delay;
 if (delay > 0 && time - is->frame_timer > AV_SYNC_THRESHOLD_MAX)
 is->frame_timer = time;

 SDL_LockMutex(is->pictq.mutex);
 if (!isnan(vp->pts))
 update_video_pts(is, vp->pts, vp->serial);
 SDL_UnlockMutex(is->pictq.mutex);

 // 丢帧？？
 if (frame_queue_nb_remaining(&is->pictq) > 1) {
 Frame *nextvp = frame_queue_peek_next(&is->pictq); // f->rindex + f->rindex_shown + 1
 duration = vp_duration(is, vp, nextvp);
 if (time > is->frame_timer + duration) {
 frame_queue_next(&is->pictq);
 goto retry;
 }
 }

 frame_queue_next(&is->pictq);
 is->force_refresh = 1;
 }
 display:
 // display picture
 if (is->force_refresh && is->pictq.rindex_shown)
 video_display(is);
 }
 is->force_refresh = 0;
 }
 **/

