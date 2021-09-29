/*
 * YangVideoPlay.h
 *
 *  Created on: 2019年7月27日
 *      Author: yang
 */

#ifndef YANGPLAYER_INCLUDE_YANGVIDEOPLAY_H_
#define YANGPLAYER_INCLUDE_YANGVIDEOPLAY_H_

#include "SDL2/SDL_ttf.h"
#include <vector>
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/sys/YangThread.h"
#include "yangutil/sys/YangLoadLib.h"
//#include "YangPlayCommon.h"
//#include "yangutil/VideoPlayList.h"
//#define YangSdlRectNum 4



using namespace std;
/**
struct Frame {
	AVFrame *frame;
	int32_t serial;
	double pts;      //presentation timestamp for the frame
	double duration; //estimated duration of the frame
	int32_t width;
	int32_t height;
	int32_t format;
	AVRational sar;
	int32_t uploaded;
	int32_t flip_v;
};
**/

class YangVideoPlay :public YangThread{
public:
	YangVideoPlay();
	virtual ~YangVideoPlay();


	void startLoop();
	void stopLoop();
	void closeVideoPlay();
	void setWindowSize(int32_t p_sw,int32_t p_sh,int32_t p_pw,int32_t p_ph);
	//void yuv_show(uint8_t *buf, int32_t p_width);//int32_t wrap[], int32_t xsize, int32_t ysize);
	//void yuv_show_frame(AVFrame *frame);
	//void yuv_show_list();
	void splitScreen(int32_t screenNum);
	//void initSdlWin(int32_t pcolorType,int32_t w,int32_t h,int32_t pfps);//,int32_t x,int32_t y);
	//void initWin(YangSdlRect *wrc,int32_t w,int32_t h,int32_t x,int32_t y);

	void setVideoList(vector<YangVideoBuffer*> *pvideoList);
	int32_t screenMode=0;
	int32_t screenNum=0;
	int32_t m_loops;

	//YangSdlRect rects={0};
	void init();
	void stop();
    int32_t m_isStart;
protected:
	  void run();

private:
	vector<YangVideoBuffer*> *m_videoList;
	const int32_t bpp = 12;
	int32_t isInit;

	 int32_t screen_w = 1280, screen_h = 720;
	 int32_t pixel_w = 1280, pixel_h = 720;
	//SDL_Texture* pTextTexture;
	SDL_Window *m_screen = NULL;
	SDL_Renderer *m_sdlRenderer = NULL;
	SDL_Texture *m_sdlTexture ;
	//TTF_Font *font;
	//void initText(YangSdlRect *wrc,TTF_Font *pfont,const char* label,SDL_Color color);
	//void handle_yuv_show(YangSdlRect *wrc);
	//void handle_yuv_texut(YangSdlRect *wrc,uint8_t* buf,int32_t pwidth);
	void handle_init();
	int32_t hasData();

	YangLoadLib m_lib,m_lib1;
	void loadLib();
	void unloadLib();
        int32_t  (*yang_SDL_Init)(Uint32 flags);
        Uint8  (*yang_SDL_EventState)(Uint32 type, int32_t state);
        SDL_Thread *
	(*yang_SDL_CreateThread)(SDL_ThreadFunction fn, const char *name, void *data);
        int32_t  (*yang_SDL_GetDisplayUsableBounds)(int32_t displayIndex, SDL_Rect * rect);
        SDL_Window *  (*yang_SDL_CreateWindow)(const char *title,
	                                                      int32_t x, int32_t y, int32_t w,
	                                                      int32_t h, Uint32 flags);
        SDL_Window *  (*yang_SDL_CreateWindowFrom)(const void *data);
        SDL_Renderer *  (*yang_SDL_CreateRenderer)(SDL_Window * window,int32_t index, Uint32 flags);
        SDL_Texture *  (*yang_SDL_CreateTexture)(SDL_Renderer * renderer, Uint32 format, int32_t access, int32_t w,int32_t h);
        int32_t  (*yang_SDL_UpdateTexture)(SDL_Texture * texture,const SDL_Rect * rect,const void *pixels, int32_t pitch);
        int32_t  (*yang_SDL_RenderClear)(SDL_Renderer * renderer);
        int32_t  (*yang_SDL_RenderCopy)(SDL_Renderer * renderer,SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect);
        void  (*yang_SDL_RenderPresent)(SDL_Renderer * renderer);
        int32_t  (*yang_SDL_SetRenderDrawColor)(SDL_Renderer * renderer, Uint8 r, Uint8 g, Uint8 b,Uint8 a);
        int32_t  (*yang_SDL_RenderFillRect)(SDL_Renderer * renderer,const SDL_Rect * rect);
        void  (*yang_SDL_DestroyTexture)(SDL_Texture * texture);
        void  (*yang_SDL_DestroyRenderer)(SDL_Renderer * renderer);
        void  (*yang_SDL_DestroyWindow)(SDL_Window * window);
        void  (*yang_SDL_FreeSurface)(SDL_Surface * surface);
        SDL_Texture *  (*yang_SDL_CreateTextureFromSurface)(SDL_Renderer * renderer, SDL_Surface * surface);
        const char * (*yang_SDL_GetError)(void);

        TTF_Font *  (*yang_TTF_OpenFont)(const char *file, int32_t ptsize);
        SDL_Surface *  (*yang_TTF_RenderUTF8_Blended)(TTF_Font *font, const char *text, SDL_Color fg);
        void  (*yang_TTF_CloseFont)(TTF_Font *font);
        int32_t  (*yang_TTF_Init)(void);
};

#endif /* YANGPLAYER_INCLUDE_YANGVIDEOPLAY_H_ */
