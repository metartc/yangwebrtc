/*
 * YangSdlWindow.h
 *
 *  Created on: 2020年8月24日
 *      Author: yang
 */

#ifndef YANGPLAYER_INCLUDE_YANGWINPLAYSDL_H_
#define YANGPLAYER_INCLUDE_YANGWINPLAYSDL_H_
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <yangplayer/YangWinPlayFactroy.h>

#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/sys/YangLoadLib.h"
//#include "yangutil/sys/YangLoadLib.h"
class YangWinPlaySdl :public YangWinPlay{
public:
	YangWinPlaySdl();
	virtual ~YangWinPlaySdl();
	void init(void* pid);
	void initBg(int pwid,int phei);
	void initVideo(int pwid,int phei,YangYuvType sdfe);
	void initText(char *pname,YangColor *pcolor);

	void initAll(char *pname,YangColor *pcolor,void* pid,int pwid,int phei,YangYuvType sdfe);

	//void reInitBg(int pwid,int phei);
	void reInitVideo(int pwid,int phei,YangYuvType sdfe);
	void reInitText(char *pname,YangColor *pcolor);

	void renderPreview(unsigned char* pdata);
	void render(unsigned char* pdata);
	void renderBg(YangColor *pcolor);

	void render(unsigned char* pdata,long long ptimestamp);



	//int isPreview;
	//YangVideoBuffer *m_vb;
private:
	SDL_Window *m_sdlWindow;
	SDL_Renderer *m_sdlRenderer;
	SDL_Texture* m_sqlTexture;
#ifndef _WIN32
	SDL_Texture* m_textTexture;
	SDL_Surface* m_textSurface;
	TTF_Font *m_font;
	SDL_Rect rcText;
#endif
	SDL_Rect spos;
	SDL_PixelFormatEnum m_colorFormat;
	int isInit1,isInit2,isInit3;
	long long m_timestamp;

	unsigned char *m_img;
	int m_hasText;
	void YUY2toNV12(int inWidth, int inHeight, unsigned char *pSrc, unsigned char *pDest);
	void YUY2toYv12(int inWidth, int inHeight, uint8_t *pSrc, uint8_t *pDest);
	YangLoadLib m_lib;
#ifndef _WIN32
	YangLoadLib m_lib1;
#endif
	void loadLib();
	void unloadLib();
	int SDLCALL (*yang_SDL_Init)(Uint32 flags);
	Uint8 SDLCALL (*yang_SDL_EventState)(Uint32 type, int state);
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
#ifndef _WIN32
	TTF_Font * SDLCALL (*yang_TTF_OpenFont)(const char *file, int ptsize);
	SDL_Surface * SDLCALL (*yang_TTF_RenderUTF8_Blended)(TTF_Font *font, const char *text, SDL_Color fg);
	void SDLCALL (*yang_TTF_CloseFont)(TTF_Font *font);
	int SDLCALL (*yang_TTF_Init)(void);
#else
//#define yang_TTF_OpenFont TTF_OpenFont
//#define yang_TTF_RenderUTF8_Blended TTF_RenderUTF8_Blended
//#define yang_TTF_CloseFont TTF_CloseFont
//#define yang_TTF_Init TTF_Init


#endif
};

#endif /* YANGPLAYER_INCLUDE_YANGWINPLAYSDL_H_ */
