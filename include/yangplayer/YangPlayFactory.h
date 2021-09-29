/*
 * YangPlayFactory.h
 *
 *  Created on: 2020年9月4日
 *      Author: yang
 */

#ifndef YANGPLAYER_INCLUDE_YANGPLAYFACTORY_H_
#define YANGPLAYER_INCLUDE_YANGPLAYFACTORY_H_
#include "yangplayer/YangAudioPlay.h"
enum YangAudioPlayType{
	Yang_AP_SDL,
	Yang_AP_ALSA,
};

class YangPlayFactory {
public:
	YangPlayFactory();
	virtual ~YangPlayFactory();
	YangAudioPlay* createAudioPlay(YangAudioInfo *pini);
	YangAudioPlay *createAudioPlay(YangAudioPlayType paet,YangAudioInfo *pini);
};

#endif /* YANGPLAYER_INCLUDE_YANGPLAYFACTORY_H_ */
