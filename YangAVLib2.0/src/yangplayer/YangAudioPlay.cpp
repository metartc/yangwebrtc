#include <yangplayer/YangAudioPlay.h>

#include <yangutil/yang_unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

#include <yangavutil/audio/YangMakeWave.h>
YangAudioPlay::YangAudioPlay() {        //m_rc=0;
	m_ini = NULL;
	m_in_audioBuffer = NULL;
	isInit = 0;
	m_ace = NULL;
	aIndex = 0;

	m_frames = 1024;
	m_channel = 2;
	m_sample = 44100;
	m_isStart=0;

}

YangAudioPlay::~YangAudioPlay() {
	m_ini = NULL;
	m_in_audioBuffer = NULL;
	isInit = 0;
	m_ace = NULL;
}
void YangAudioPlay::run() {
	m_isStart=1;
	startLoop();
	m_isStart=0;
}
void YangAudioPlay::stop() {
	stopLoop();
}
void YangAudioPlay::setAudioList(vector<YangAudioPlayBuffer*> *pal) {
	m_in_audioBuffer = pal;
}

void YangAudioPlay::setAecBase(YangAecBase *pace) {
	if (pace != NULL)
		m_aecInit = 1;
	m_ace = pace;
}

void YangAudioPlay::aecHandle(uint8_t *ppcm,int32_t plen) {
	if (m_ace != NULL) {
		//	m_ace->echo_state_reset();
		m_ace->put_echo_playback(ppcm,plen);

	}
}

