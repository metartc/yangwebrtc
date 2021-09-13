/*
 * YangPlayBase.cpp
 *
 *  Created on: 2019年10月11日
 *      Author: yang
 */

#include <yangplayer/YangPlayerPlay.h>
#include "pthread.h"

#include "yangplayer/YangPlayFactory.h"
#include "unistd.h"



YangPlayerPlay::YangPlayerPlay() {
	vm_audioPlay=NULL;
	//vm_videoPlay=NULL;
	vm_audio_player_start=0;
}

YangPlayerPlay::~YangPlayerPlay() {
	//m_ini=NULL;
	if(vm_audioPlay&&vm_audioPlay->m_isStart){
		vm_audioPlay->stop();
		while(vm_audioPlay->m_isStart){
								usleep(1000);
							}
	}
	//if(vm_videoPlay&&vm_videoPlay->m_isStart){
	//	vm_videoPlay->stop();
	//	while(vm_videoPlay->m_isStart){
	//							usleep(1000);
	//						}
	//}
}
void YangPlayerPlay::stopAll(){
	if(vm_audioPlay)	vm_audioPlay->stop();
	//if(vm_videoPlay)	vm_videoPlay->stop();
}
void YangPlayerPlay::initAudioPlay(YangAudioInfo* paudio){
	//YangConfig *p_config = m_ini;
	if (vm_audioPlay == NULL) {
		//YangPlayFactory pf;
		//YangAudioParam audio={0};
		vm_audioPlay = new YangAudioPlayerSdl(paudio);
		vm_audioPlay->init();

	}

}
/**
void YangPlayerPlay::initVideoPlay(YangVideoDecoderHandle *handle){
	if (vm_videoPlay == NULL)	{
		vm_videoPlay = new YangVideoPlay();
		vm_videoPlay->init();
		//handle->m_yvp=vm_videoPlay;
	}

}

void YangPlayerPlay::setInVideoList(vector<YangVideoBuffer*> *pvideoList){
	if(vm_videoPlay!=NULL) vm_videoPlay->setVideoList(pvideoList);//setAudioList(paudioList);
}
void YangPlayerPlay::startVideoPlay(){
	//if(vm_audio_player_start) return;
	vm_videoPlay->start();
	//vm_audio_player_start=1;
}
int32_t YangPlayerPlay::getIsAecInit(){
	if(vm_audioPlay!=NULL) return vm_audioPlay->m_aecInit;
 return 0;
}
//F_initSdlWin YangPlayApp::getInitSdlWin(){
//	return vm_initSdlWin;
//}

void YangPlayerPlay::setAec(YangAecBase *paec){
	if(vm_audioPlay!=NULL) vm_audioPlay->setAecBase(paec);
}
**/
void YangPlayerPlay::startAudioPlay(){
	if(vm_audio_player_start) return;
	vm_audioPlay->start();
	vm_audio_player_start=1;
}
void YangPlayerPlay::setInAudioList(YangAudioPlayBuffer *paudioList){
	if(vm_audioPlay!=NULL) vm_audioPlay->setAudioList(paudioList);
}

