/*
 * YangSdlWindow.cpp
 *
 *  Created on: 2020年8月24日
 *      Author: yang
 */

#include "yangplayer/YangWinPlaySdl.h"
#include "yangutil/sys/YangLog.h"
#include "stdint.h"
#include <unistd.h>



void YangWinPlaySdl::loadLib(){
	yang_SDL_Init=(int32_t SDLCALL (*)(Uint32 flags))m_lib.loadFunction("SDL_Init");
	yang_SDL_EventState=(Uint8 SDLCALL (*)(Uint32 type, int32_t state)) m_lib.loadFunction("SDL_EventState");
	yang_SDL_CreateWindowFrom=(SDL_Window * SDLCALL (*)(const void *data))m_lib.loadFunction("SDL_CreateWindowFrom");
	yang_SDL_CreateRenderer=(SDL_Renderer * SDLCALL (*)(SDL_Window * window,int32_t index, Uint32 flags))m_lib.loadFunction("SDL_CreateRenderer");
	yang_SDL_CreateTexture=(SDL_Texture * SDLCALL (*)(SDL_Renderer * renderer, Uint32 format, int32_t access, int32_t w,int32_t h))m_lib.loadFunction("SDL_CreateTexture");
	yang_SDL_UpdateTexture=(int32_t SDLCALL (*)(SDL_Texture * texture,const SDL_Rect * rect,const void *pixels, int32_t pitch))m_lib.loadFunction("SDL_UpdateTexture");
	yang_SDL_RenderClear=(int32_t SDLCALL (*)(SDL_Renderer * renderer))m_lib.loadFunction("SDL_RenderClear");
	yang_SDL_RenderCopy=(int32_t SDLCALL (*)(SDL_Renderer * renderer,SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect))m_lib.loadFunction("SDL_RenderCopy");
	yang_SDL_RenderPresent=(void SDLCALL (*)(SDL_Renderer * renderer))m_lib.loadFunction("SDL_RenderPresent");
	yang_SDL_SetRenderDrawColor=(int32_t SDLCALL (*)(SDL_Renderer * renderer, Uint8 r, Uint8 g, Uint8 b,Uint8 a))m_lib.loadFunction("SDL_SetRenderDrawColor");
	yang_SDL_RenderFillRect=(int32_t SDLCALL (*)(SDL_Renderer * renderer,const SDL_Rect * rect))m_lib.loadFunction("SDL_RenderFillRect");
	yang_SDL_DestroyTexture=(void SDLCALL (*)(SDL_Texture * texture))m_lib.loadFunction("SDL_DestroyTexture");
	yang_SDL_DestroyRenderer=(void SDLCALL (*)(SDL_Renderer * renderer))m_lib.loadFunction("SDL_DestroyRenderer");
	yang_SDL_DestroyWindow=(void SDLCALL (*)(SDL_Window * window))m_lib.loadFunction("SDL_DestroyWindow");
	yang_SDL_FreeSurface=(void SDLCALL (*)(SDL_Surface * surface))m_lib.loadFunction("SDL_FreeSurface");
	yang_SDL_CreateTextureFromSurface=(SDL_Texture * SDLCALL (*)(SDL_Renderer * renderer, SDL_Surface * surface))m_lib.loadFunction("SDL_CreateTextureFromSurface");
	yang_SDL_GetError=(const char *SDLCALL (*)(void))m_lib.loadFunction("SDL_GetError");

#ifndef _WIN32
	yang_TTF_Init=(int32_t SDLCALL (*)(void))m_lib1.loadFunction("TTF_Init");
	yang_TTF_OpenFont=(TTF_Font * SDLCALL (*)(const char *file, int32_t ptsize))m_lib1.loadFunction("TTF_OpenFont");
	yang_TTF_RenderUTF8_Blended=(SDL_Surface * SDLCALL (*)(TTF_Font *font, const char *text, SDL_Color fg))m_lib1.loadFunction("TTF_RenderUTF8_Blended");
	yang_TTF_CloseFont=(void SDLCALL (*)(TTF_Font *font))m_lib1.loadFunction("TTF_CloseFont");
#endif

}
void YangWinPlaySdl::unloadLib(){
	yang_SDL_Init=NULL;
	yang_SDL_EventState=NULL;
	yang_SDL_CreateWindowFrom=NULL;
	yang_SDL_CreateRenderer=NULL;
	yang_SDL_CreateTexture=NULL;
	yang_SDL_UpdateTexture=NULL;
	yang_SDL_RenderClear=NULL;
	yang_SDL_RenderCopy=NULL;
	yang_SDL_RenderPresent=NULL;
	yang_SDL_SetRenderDrawColor=NULL;
	yang_SDL_RenderFillRect=NULL;
	yang_SDL_DestroyTexture=NULL;
	yang_SDL_DestroyRenderer=NULL;
	yang_SDL_DestroyWindow=NULL;
	yang_SDL_FreeSurface=NULL;
	yang_SDL_CreateTextureFromSurface=NULL;
	yang_SDL_GetError=NULL;
	yang_SDL_Init=NULL;
#ifndef _WIN32
	yang_TTF_Init=NULL;
	yang_TTF_OpenFont=NULL;
	yang_TTF_RenderUTF8_Blended=NULL;
	yang_TTF_CloseFont=NULL;
#endif
}
YangWinPlaySdl::YangWinPlaySdl() {

	m_sdlRenderer = NULL;
	m_sdlWindow = NULL;
	m_sqlTexture = NULL;
#ifndef _WIN32
	m_textSurface = NULL;
	m_textTexture = NULL;
	m_font = 0;
#endif
	m_width = 0;
	m_height = 0;
	m_timestamp = 0;
	m_img = NULL;
	m_hasText = 0;
	memset(&spos,0,sizeof(spos));
	isInit1=0,isInit2=0,isInit3=0;
	m_colorFormat=SDL_PIXELFORMAT_NV12;
	unloadLib();
}
YangWinPlaySdl::~YangWinPlaySdl() {


if (m_sdlWindow) {
	yang_SDL_DestroyWindow(m_sdlWindow);
	m_sdlWindow = NULL;
}

if (m_sdlRenderer) {
	yang_SDL_DestroyRenderer(m_sdlRenderer);
	m_sdlRenderer = NULL;
}

	if (m_sqlTexture) {
		yang_SDL_DestroyTexture(m_sqlTexture);
		m_sqlTexture = NULL;
	}

#ifndef _WIN32
	if (m_textTexture) {
			yang_SDL_DestroyTexture(m_textTexture);
			m_textTexture = NULL;
		}
	if (m_textSurface) {
		yang_SDL_FreeSurface(m_textSurface);
		m_textSurface = NULL;
	}
	if (m_font) {
		yang_TTF_CloseFont(m_font);
		m_font = NULL;
	}
#endif
	yang_deleteA(m_img);
	unloadLib();
		m_lib.unloadObject();

#ifndef _WIN32
		m_lib1.unloadObject();
#endif
	//m_vb=NULL;
}
void YangWinPlaySdl::init(void* pid){
	if(isInit1==1) return;
	m_lib.loadObject("libSDL2");
#ifndef _WIN32
	m_lib1.loadObject("libSDL2_ttf");
#endif
		loadLib();
	yang_SDL_Init(SDL_INIT_VIDEO);
	yang_SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
	yang_SDL_EventState(SDL_USEREVENT, SDL_IGNORE);
	m_sdlWindow = yang_SDL_CreateWindowFrom(pid);
	m_sdlRenderer = yang_SDL_CreateRenderer(m_sdlWindow, -1,	SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	isInit1=1;
}
void YangWinPlaySdl::initBg(int32_t pwid,int32_t phei){
	spos.x=0;
	spos.y=0;
	spos.w=pwid;
	spos.h=phei;
}
void YangWinPlaySdl::initVideo(int32_t pwid, int32_t phei,	YangYuvType psdf) {

	// printf("\n**************************init sdl***************************\n");
	if(isInit2==1) return;
	//SDL_PixelFormatEnum sdf;
	if(psdf==YangYuy2) m_colorFormat=SDL_PIXELFORMAT_YUY2;
	if(psdf==YangYv12) m_colorFormat=SDL_PIXELFORMAT_YV12;
	if(psdf==YangNv12) m_colorFormat=SDL_PIXELFORMAT_NV12;
	if(psdf==YangI420) m_colorFormat=SDL_PIXELFORMAT_IYUV;
	//  qDebug()<<m_ys->winId()<<"......................"<<(m_sdlWindow==nullptr?"is NULL":"Not NULL");
	//m_sdlWindow = SDL_CreateWindow("title",pos().x(),pos().y(),this->width(),this->height(),SDL_WINDOW_OPENGL|SDL_WINDOW_BORDERLESS);

	m_width = pwid;
	m_height = phei;
	if (m_img == NULL)
		m_img = new uint8_t[m_width * m_height * 3 / 2];
	m_sqlTexture = yang_SDL_CreateTexture(m_sdlRenderer,	m_colorFormat, SDL_TEXTUREACCESS_STREAMING, m_width, m_height);
	m_hasText=0;
	isInit2=1;
}
void YangWinPlaySdl::initText(char *pname, YangColor *pcolor){
	if(isInit3==1) return;
#ifndef _WIN32
	if (yang_TTF_Init() == -1) {
		printf("Could not initialize SDL TTF - %s\n", yang_SDL_GetError());
		return;
	}

	char file1[300];
	char file_path_getcwd[255];
	memset(file1, 0, 300);
	memset(file_path_getcwd, 0, 255);
	if(getcwd(file_path_getcwd, 255)){
		sprintf(file1, "%s/lib/sim1.ttc", file_path_getcwd);
		m_font = yang_TTF_OpenFont(file1, 26);
	}
		SDL_Color color2 = { pcolor->r, pcolor->g, pcolor->b };
		m_textSurface = yang_TTF_RenderUTF8_Blended(m_font, pname, color2);
		m_textTexture = yang_SDL_CreateTextureFromSurface(m_sdlRenderer, m_textSurface);
		rcText.w = 100;
		rcText.h = 25;
		rcText.x=0;
		rcText.y=0;
		m_hasText=1;
#endif
		isInit3=1;
}
//void YangWinPlaySdl::reInitBg(int32_t pwid,int32_t phei){
//	spos.w=pwid;
//	spos.h=phei;
//}
void YangWinPlaySdl::reInitVideo(int32_t pwid,int32_t phei,YangYuvType psdf){
	if(!isInit2) {initVideo(pwid,phei,psdf);return;}
	SDL_PixelFormatEnum colorFormat=SDL_PIXELFORMAT_IYUV;
	if(psdf==YangYuy2) colorFormat=SDL_PIXELFORMAT_YUY2;
		if(psdf==YangYv12) colorFormat=SDL_PIXELFORMAT_YV12;
		if(psdf==YangNv12) colorFormat=SDL_PIXELFORMAT_NV12;
		if(psdf==YangI420) colorFormat=SDL_PIXELFORMAT_IYUV;
	if(pwid==m_width&&phei==m_height&&colorFormat==m_colorFormat) return;
	m_colorFormat=colorFormat;
	m_width=pwid;
	m_height=phei;
	if (m_sqlTexture) {
		yang_SDL_DestroyTexture(m_sqlTexture);
			m_sqlTexture = yang_SDL_CreateTexture(m_sdlRenderer,	m_colorFormat, SDL_TEXTUREACCESS_STREAMING, m_width, m_height);;
		}
}
void YangWinPlaySdl::reInitText(char *pname, YangColor *pcolor){
#ifndef _WIN32
	if(!isInit3) {initText(pname,pcolor);return;}
	if(m_textSurface) yang_SDL_FreeSurface(m_textSurface);
	if (m_textTexture) yang_SDL_DestroyTexture(m_textTexture);
	SDL_Color color2 = { pcolor->r, pcolor->g, pcolor->b };
	m_textSurface = yang_TTF_RenderUTF8_Blended(m_font, pname, color2);
	m_textTexture = yang_SDL_CreateTextureFromSurface(m_sdlRenderer, m_textSurface);
#endif
}
/**
void YangWinPlaySdl::initAll(char *pname, YangColor *pcolor,void *pid, int32_t pwid, int32_t phei,YangYuvType sdfe) {
	//SDL_Init(SDL_INIT_VIDEO);
	// printf("\n**************************init sdl***************************\n");

	init(pid, phei, phei);
	//initVideo()


}**/
void YangWinPlaySdl::YUY2toYv12(int32_t inWidth, int32_t inHeight, uint8_t *pSrc, uint8_t *pDest){
	 int32_t i, j;
	 uint8_t *u = pDest + (inWidth * inHeight);
	 uint8_t *v = u + (inWidth * inHeight) / 4;

	 for (i = 0; i < inHeight/2; i++)
	 {

	        uint8_t *src_l1 = pSrc + inWidth*2*2*i;
	        uint8_t *src_l2 = src_l1 + inWidth*2;
	        uint8_t *y_l1 = pDest + inWidth*2*i;
	        uint8_t *y_l2 = y_l1 + inWidth;
	  for (j = 0; j < inWidth/2; j++)
	  {
	    *y_l1++ = src_l1[0];
	    *v++ = src_l1[1];
	    *y_l1++ = src_l1[2];
	    *u++ = src_l1[3];
	    *y_l2++ = src_l2[0];
	    *y_l2++ = src_l2[2];
	    src_l1 += 4;
	    src_l2 += 4;
	  }
	 }
}
void YangWinPlaySdl::YUY2toNV12(int32_t inWidth, int32_t inHeight, uint8_t *pSrc,uint8_t *pDest) {
	int32_t i, j;
	uint8_t *uv = pDest + (inWidth * inHeight);
// uint8_t *v = u + (inWidth * inHeight) / 4;

	for (i = 0; i < inHeight / 2; i++) {

		uint8_t *src_l1 = pSrc + inWidth * 2 * 2 * i;
		uint8_t *src_l2 = src_l1 + inWidth * 2;
		uint8_t *y_l1 = pDest + inWidth * 2 * i;
		uint8_t *y_l2 = y_l1 + inWidth;
		for (j = 0; j < inWidth / 2; j++) {
			// two pels in one go
			*y_l1++ = src_l1[0];
			*uv++ = src_l1[1];
			*y_l1++ = src_l1[2];
			*uv++ = src_l1[3];
			*y_l2++ = src_l2[0];
			*y_l2++ = src_l2[2];
			src_l1 += 4;
			src_l2 += 4;
		}
	}
}
void YangWinPlaySdl::render(uint8_t *pdata) {
	//printf("%d,",m_width);
	//YUY2toNV12(m_width,m_height,pdata,m_img);
	yang_SDL_UpdateTexture(m_sqlTexture, NULL, pdata, m_width);	//m_vb->width);
	yang_SDL_RenderClear(m_sdlRenderer);
	yang_SDL_RenderCopy(m_sdlRenderer, m_sqlTexture, NULL, NULL);//&m_rect);// &rect.sdlRect);
#ifndef _WIN32
	if (m_hasText && m_textTexture)
		yang_SDL_RenderCopy(m_sdlRenderer, m_textTexture, NULL, &rcText);//&rects[i].rcText);
#endif
	yang_SDL_RenderPresent(m_sdlRenderer);
}
void YangWinPlaySdl::render(uint8_t* pdata,int64_t ptimestamp){
	yang_SDL_UpdateTexture(m_sqlTexture, NULL, pdata, m_width);	//m_vb->width);
		yang_SDL_RenderClear(m_sdlRenderer);
		yang_SDL_RenderCopy(m_sdlRenderer, m_sqlTexture, NULL, NULL);//&m_rect);// &rect.sdlRect);
#ifndef _WIN32
		if (m_hasText && m_textTexture)
			yang_SDL_RenderCopy(m_sdlRenderer, m_textTexture, NULL, &rcText);//&rects[i].rcText);
#endif
		yang_SDL_RenderPresent(m_sdlRenderer);
}
void YangWinPlaySdl::renderPreview(uint8_t *pdata) {
	if(m_colorFormat==SDL_PIXELFORMAT_NV12)
		YUY2toNV12(m_width, m_height, pdata, m_img);
	else
		YUY2toYv12(m_width, m_height, pdata, m_img);
	render(m_img);
}
 void YangWinPlaySdl::renderBg(YangColor *pcolor){
	 yang_SDL_SetRenderDrawColor(m_sdlRenderer, pcolor->r, pcolor->g, pcolor->b, 255);
	 yang_SDL_RenderFillRect(m_sdlRenderer, nullptr);
	 yang_SDL_SetRenderDrawColor(m_sdlRenderer, pcolor->r, pcolor->g, pcolor->b, 0xFF);
	 yang_SDL_RenderFillRect(m_sdlRenderer, &spos);
	 yang_SDL_RenderPresent(m_sdlRenderer);
}
