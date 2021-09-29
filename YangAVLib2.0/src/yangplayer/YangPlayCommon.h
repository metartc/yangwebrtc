#ifndef YANGPLAYER_YANGPlayCommon_H_
#define YANGPLAYER_YANGPlayCommon_H_
#include "SDL2/SDL.h"
struct YangSdlRect{
	SDL_Surface *textSurface;
	SDL_Texture *textTexture;
	SDL_Texture *sdlTexture ;
	//SDL_Rect rect;
	SDL_Rect rcText;
	SDL_Rect sdlRect;
	int32_t timestamp;
	int32_t preTimestamp;
};
#define REFRESH_EVENT  (SDL_USEREVENT + 1)
#define BREAK_EVENT  (SDL_USEREVENT + 2)
#endif
