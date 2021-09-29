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
	void initBg(int32_t pwid,int32_t phei);
	void initVideo(int32_t pwid,int32_t phei,YangYuvType sdfe);
	void initText(char *pname,YangColor *pcolor);

	void initAll(char *pname,YangColor *pcolor,void* pid,int32_t pwid,int32_t phei,YangYuvType sdfe);

	//void reInitBg(int32_t pwid,int32_t phei);
	void reInitVideo(int32_t pwid,int32_t phei,YangYuvType sdfe);
	void reInitText(char *pname,YangColor *pcolor);

	void renderPreview(uint8_t* pdata);
	void render(uint8_t* pdata);
	void renderBg(YangColor *pcolor);

	void render(uint8_t* pdata,int64_t ptimestamp);



	//int32_t isPreview;
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
	int32_t isInit1,isInit2,isInit3;
	int64_t m_timestamp;

	uint8_t *m_img;
	int32_t m_hasText;
	void YUY2toNV12(int32_t inWidth, int32_t inHeight, uint8_t *pSrc, uint8_t *pDest);
	void YUY2toYv12(int32_t inWidth, int32_t inHeight, uint8_t *pSrc, uint8_t *pDest);
	YangLoadLib m_lib;
#ifndef _WIN32
	YangLoadLib m_lib1;
#endif
	void loadLib();
	void unloadLib();
        int32_t  (*yang_SDL_Init)(Uint32 flags);
        Uint8  (*yang_SDL_EventState)(Uint32 type, int32_t state);
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
#ifndef _WIN32
        TTF_Font *  (*yang_TTF_OpenFont)(const char *file, int32_t ptsize);
        SDL_Surface *  (*yang_TTF_RenderUTF8_Blended)(TTF_Font *font, const char *text, SDL_Color fg);
        void  (*yang_TTF_CloseFont)(TTF_Font *font);
        int32_t  (*yang_TTF_Init)(void);
#else
//#define yang_TTF_OpenFont TTF_OpenFont
//#define yang_TTF_RenderUTF8_Blended TTF_RenderUTF8_Blended
//#define yang_TTF_CloseFont TTF_CloseFont
//#define yang_TTF_Init TTF_Init


#endif
};

#endif /* YANGPLAYER_INCLUDE_YANGWINPLAYSDL_H_ */
