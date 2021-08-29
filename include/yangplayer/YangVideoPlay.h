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
	int serial;
	double pts;      //presentation timestamp for the frame
	double duration; //estimated duration of the frame
	int width;
	int height;
	int format;
	AVRational sar;
	int uploaded;
	int flip_v;
};
**/

class YangVideoPlay :public YangThread{
public:
	YangVideoPlay();
	virtual ~YangVideoPlay();


	void startLoop();
	void stopLoop();
	void closeVideoPlay();
	void setWindowSize(int p_sw,int p_sh,int p_pw,int p_ph);
	//void yuv_show(unsigned char *buf, int p_width);//int wrap[], int xsize, int ysize);
	//void yuv_show_frame(AVFrame *frame);
	//void yuv_show_list();
	void splitScreen(int screenNum);
	//void initSdlWin(int pcolorType,int w,int h,int pfps);//,int x,int y);
	//void initWin(YangSdlRect *wrc,int w,int h,int x,int y);

	void setVideoList(vector<YangVideoBuffer*> *pvideoList);
	int screenMode=0;
	int screenNum=0;
	int m_loops;

	//YangSdlRect rects={0};
	void init();
	void stop();
    int m_isStart;
protected:
	  void run();

private:
	vector<YangVideoBuffer*> *m_videoList;
	const int bpp = 12;
	int isInit;

	 int screen_w = 1280, screen_h = 720;
	 int pixel_w = 1280, pixel_h = 720;
	//SDL_Texture* pTextTexture;
	SDL_Window *m_screen = NULL;
	SDL_Renderer *m_sdlRenderer = NULL;
	SDL_Texture *m_sdlTexture ;
	//TTF_Font *font;
	//void initText(YangSdlRect *wrc,TTF_Font *pfont,const char* label,SDL_Color color);
	//void handle_yuv_show(YangSdlRect *wrc);
	//void handle_yuv_texut(YangSdlRect *wrc,uint8_t* buf,int pwidth);
	void handle_init();
	int hasData();

	YangLoadLib m_lib,m_lib1;
	void loadLib();
	void unloadLib();
	int SDLCALL (*yang_SDL_Init)(Uint32 flags);
	Uint8 SDLCALL (*yang_SDL_EventState)(Uint32 type, int state);
	SDL_Thread *SDLCALL
	(*yang_SDL_CreateThread)(SDL_ThreadFunction fn, const char *name, void *data);
	int SDLCALL (*yang_SDL_GetDisplayUsableBounds)(int displayIndex, SDL_Rect * rect);
	SDL_Window * SDLCALL (*yang_SDL_CreateWindow)(const char *title,
	                                                      int x, int y, int w,
	                                                      int h, Uint32 flags);
	SDL_Window * SDLCALL (*yang_SDL_CreateWindowFrom)(const void *data);
	SDL_Renderer * SDLCALL (*yang_SDL_CreateRenderer)(SDL_Window * window,int index, Uint32 flags);
	SDL_Texture * SDLCALL (*yang_SDL_CreateTexture)(SDL_Renderer * renderer, Uint32 format, int access, int w,int h);
	int SDLCALL (*yang_SDL_UpdateTexture)(SDL_Texture * texture,const SDL_Rect * rect,const void *pixels, int pitch);
	int SDLCALL (*yang_SDL_RenderClear)(SDL_Renderer * renderer);
	int SDLCALL (*yang_SDL_RenderCopy)(SDL_Renderer * renderer,SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect);
	void SDLCALL (*yang_SDL_RenderPresent)(SDL_Renderer * renderer);
	int SDLCALL (*yang_SDL_SetRenderDrawColor)(SDL_Renderer * renderer, Uint8 r, Uint8 g, Uint8 b,Uint8 a);
	int SDLCALL (*yang_SDL_RenderFillRect)(SDL_Renderer * renderer,const SDL_Rect * rect);
	void SDLCALL (*yang_SDL_DestroyTexture)(SDL_Texture * texture);
	void SDLCALL (*yang_SDL_DestroyRenderer)(SDL_Renderer * renderer);
	void SDLCALL (*yang_SDL_DestroyWindow)(SDL_Window * window);
	void SDLCALL (*yang_SDL_FreeSurface)(SDL_Surface * surface);
	SDL_Texture * SDLCALL (*yang_SDL_CreateTextureFromSurface)(SDL_Renderer * renderer, SDL_Surface * surface);
	const char *SDLCALL (*yang_SDL_GetError)(void);

	TTF_Font * SDLCALL (*yang_TTF_OpenFont)(const char *file, int ptsize);
	SDL_Surface * SDLCALL (*yang_TTF_RenderUTF8_Blended)(TTF_Font *font, const char *text, SDL_Color fg);
	void SDLCALL (*yang_TTF_CloseFont)(TTF_Font *font);
	int SDLCALL (*yang_TTF_Init)(void);
};

#endif /* YANGPLAYER_INCLUDE_YANGVIDEOPLAY_H_ */
