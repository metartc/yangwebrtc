/*
 * YangPlayBase.cpp
 *
 *  Created on: 2019年10月11日
 *      Author: yang
 */

#include <yangmeeting/YangMeetingAudioPlay.h>
#include <pthread.h>

#include <yangaudiodev/YangAudioPlay.h>
#include <yangplayer/YangPlayFactory.h>


YangAudioPlay *vm_audioPlay=NULL;
int32_t vm_audio_player_start=0;

YangMeetingAudioPlay::YangMeetingAudioPlay(YangMeetingContext *pcontext) {
	m_context=pcontext;
}

YangMeetingAudioPlay::~YangMeetingAudioPlay() {
	m_context=NULL;
}

void YangMeetingAudioPlay::initAudioPlay(){
	//YangConfig *p_config = m_context;
	if (vm_audioPlay == NULL) {
		YangPlayFactory pf;
		vm_audioPlay = pf.createAudioPlay(&m_context->audio);
		vm_audioPlay->init();

	}
	//if (m_context->isPlayVideo) {
	//	if (vm_videoPlay == NULL)	vm_videoPlay = new YangVideoPlay();
	//	vm_videoPlay->init();
	//}
	//p_config=NULL;
}

void YangMeetingAudioPlay::setInAudioList(vector<YangAudioPlayBuffer*> *paudioList){
	if(vm_audioPlay!=NULL) vm_audioPlay->setAudioBuffers(paudioList);
}

void YangMeetingAudioPlay::startAudioPlay(){
	if(vm_audio_player_start) return;
	vm_audioPlay->start();
	vm_audio_player_start=1;
}
int32_t YangMeetingAudioPlay::getIsAecInit(){
	if(vm_audioPlay!=NULL) return vm_audioPlay->m_aecInit;
 return 0;
}
//F_initSdlWin YangPlayApp::getInitSdlWin(){
//	return vm_contexttSdlWin;
//}

void YangMeetingAudioPlay::setAec(YangAecBase *paec){
	if(vm_audioPlay!=NULL) vm_audioPlay->setAecBase(paec);
}
