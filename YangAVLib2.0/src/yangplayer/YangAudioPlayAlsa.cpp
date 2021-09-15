/*
 * YangAudioPlayAlsa.cpp
 *
 *  Created on: 2020年9月4日
 *      Author: yang
 */

#include "YangAudioPlayAlsa.h"
#ifndef _WIN32
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>

YangAudioPlayAlsa::YangAudioPlayAlsa(YangAudioInfo *pini) {        //m_rc=0;
	m_ini=pini;
	m_handle = NULL;
	m_size = 0;
	m_loops = 0;
	ret = 0;
	m_frames = 0;
	m_channel=pini->channel;
}

YangAudioPlayAlsa::~YangAudioPlayAlsa() {

	closeAudio();
}
//CMyMakeWave mw1;
void YangAudioPlayAlsa::init() {
	if (isInit == 1)	return;

    if(m_ini->usingMono){
    	m_frames = 320;
    	m_channel=1;
    	m_sample=16000;
    }else{
    	m_frames = 1024;
    }
	uint32_t  val = 0;
	int32_t dir = 0;
	//m_frames = 1024;
	snd_pcm_hw_params_t *hw_params;
	int32_t err = 0;
	if ((err = snd_pcm_open(&m_handle, "default", SND_PCM_STREAM_PLAYBACK, 0))
			< 0) {
		fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(err));
		exit(1);
	}
	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
		fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_any(m_handle, hw_params)) < 0) {
		fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_access(m_handle, hw_params,
			SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf(stderr, "cannot set access type (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_format(m_handle, hw_params,
			SND_PCM_FORMAT_S16_LE)) < 0) {
		fprintf(stderr, "cannot set sample format (%s)\n", snd_strerror(err));
		exit(1);
	}

	val = m_sample;
	if ((err = snd_pcm_hw_params_set_rate_near(m_handle, hw_params, &val, 0))
			< 0) {
		fprintf(stderr, "cannot set sample rate (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_channels(m_handle, hw_params, m_channel)) < 0) {
		fprintf(stderr, "cannot set channel count (%s)\n", snd_strerror(err));
		exit(1);
	}

	//m_frames = m_frames;
	if ((err = snd_pcm_hw_params_set_period_size_near(m_handle, hw_params,
			&m_frames, &dir)) < 0) {
		fprintf(stderr, "cannot set period size (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params(m_handle, hw_params)) < 0) {
		fprintf(stderr, "cannot set parameters (%s)\n", snd_strerror(err));
		exit(1);
	}
	//int32_t dir=0;
	// snd_pcm_uframes_t t1=0,t2=0;
	//snd_pcm_hw_params_get_period_size(hw_params, &t1, &dir);
	//snd_pcm_hw_params_get_buffer_size(hw_params, &t2);

	snd_pcm_hw_params_free(hw_params);
	m_size = m_frames * 2*m_channel; // 2 bytes/sample, 2 channels
//mw1.start("/home/yang/bmp/out1.wav");
	isInit = 1;

}

void YangAudioPlayAlsa::closeAudio() {

	if (m_handle) {
		snd_pcm_drain(m_handle);
		snd_pcm_close(m_handle);
		// buffer=NULL;
		m_handle = NULL;
	}

	//m_params=NULL;
	//m_buffer[0] = NULL;
}

void YangAudioPlayAlsa::stopLoop() {

	m_loops = 0;
}

void YangAudioPlayAlsa::play(uint8_t *p_data) {
	ret = snd_pcm_writei(m_handle, p_data, m_frames);
	// printf("%d,",ret);
	if (ret) {
		return;
	} else if (ret == -EPIPE) {
		// EPIPE means overrun
		usleep(1000);
		fprintf(stderr, "underrun occurred\n");
		snd_pcm_prepare(m_handle);
	} else if (ret < 0) {
		fprintf(stderr, "error from read: %s\n", snd_strerror(ret));
	}
}

int32_t YangAudioPlayAlsa::hasData() {
	for (int32_t i = 0; i < (int) m_in_audioBuffer->size(); i++) {
		//al=m_in_audioBuffer->at(i);
		//if(m_in_audioBuffer->at(i)==NULL) printf("\n%d__%d*****************has null value\n",m_in_audioBuffer->size(),i);
		if (m_in_audioBuffer->at(i)->m_size > 0)
			return 1;
	}
	//al=NULL;
	return 0;
}

//long tt=0;
void YangAudioPlayAlsa::startLoop() {

	m_loops = 1;
	int32_t status = 0;
	//int32_t bufLen;
	//  mw1.start("/home/yang/bmp/ttt.wav");
	uint8_t *pcm = new uint8_t[SIZE_AUDIO_FRAME * 2];
	uint8_t *tmp = NULL;
	if ((status = snd_pcm_prepare(m_handle)) < 0) {
		fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
				snd_strerror(status));
		exit(1);
	}
	//int64_t timestamp=0;
	//int32_t audioLen=0;

	//printf("\n*************************PlayAlsaAudio is starting.....\n");
	YangFrame frame;
	while (m_loops == 1) {
		if (hasData() == 0) {
			usleep(20);
			continue;
		}
		//printf("as%d,", m_in_audioBuffer->size());
		memset(pcm, 0, SIZE_AUDIO_FRAME);
		for (size_t i = 0; i < m_in_audioBuffer->size(); i++) {
			//frame=m_in_audioBuffer->at(i)->getAudios();
			tmp = m_in_audioBuffer->at(i)->getAudios(&frame);
			if (tmp) {

				mix.yangMix1(pcm, tmp, frame.nb, 128);
			}
			//yangMix1(m_in_audioBuffer->at(i)->getAudios(),pcm);
			//SDL_MixAudio(pcm, m_in_audioBuffer->at(i)->getAudios(), SIZE_AUDIO_FRAME, SDL_MIX_MAXVOLUME);
		}
		//mw1.write(m_buffer,4096);
		status = snd_pcm_writei(m_handle, pcm, m_frames);
		aecHandle(pcm,m_size);
	//	 printf("%d,",status);
		//usleep(1000);
		// rc = snd_pcm_readi(m_handle, m_buffer, m_frames);
		if(status<0){
			if (status == -EPIPE) {
				// EPIPE means overrun
			//	fprintf(stderr, "underrun occurred\n");
				usleep(1000);
				snd_pcm_prepare(m_handle);
				continue;
				//snd_pcm_prepare(m_handle);
			}
			status = snd_pcm_recover(m_handle, status, 0);
			if (status < 0) {
				fprintf(stderr, "ALSA write failed (unrecoverable): %s\n", snd_strerror(status));
			}

		}
		usleep(1000);
		//**/
		//  rc = write(1, m_buffer, m_size);

		//m_acs->BufferCB(m_buffer);
		//mw1.write(m_buffer,4096);
		// printf("%d,",rc);
		// if (rc != m_size)
		//fprintf(stderr,"short write: wrote %d bytes\n", rc);
	}

	//mw1.stop();
//	printf("\n********************AudioCapture stop.......\n");
	snd_pcm_drain(m_handle);
	snd_pcm_close(m_handle);
	tmp = NULL;
	yang_deleteA(pcm);
	// buffer=NULL;
	m_handle = NULL;
	//m_params=NULL;

}
#endif
