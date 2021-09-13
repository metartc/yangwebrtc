/*
 * YangPlayFactory.cpp
 *
 *  Created on: 2020年9月4日
 *      Author: yang
 */

#include "yangcapture/win/YangAudioPlaySdl.h"
#include <yangplayer/YangPlayFactory.h>

#include "YangAudioPlayAlsa.h"
YangPlayFactory::YangPlayFactory() {
	// TODO Auto-generated constructor stub

}

YangPlayFactory::~YangPlayFactory() {
	// TODO Auto-generated destructor stub
}

YangAudioPlay* YangPlayFactory::createAudioPlay(YangAudioInfo *pini){
#ifndef _WIN32
	if(pini->audioPlayType==1) return new YangAudioPlayAlsa(pini);
#endif
	return NULL;// new YangAudioPlaySdl(pini);
}

YangAudioPlay *YangPlayFactory::createAudioPlay(YangAudioPlayType paet,YangAudioInfo *pini){
#ifndef _WIN32
	if(paet==Yang_AP_ALSA) return new YangAudioPlayAlsa(pini);
#endif
	return NULL;//new YangAudioPlaySdl(pini);
}
