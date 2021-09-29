/*
 * YangAudioPlayAlsa.cpp
 *
 *  Created on: 2020年9月4日
 *      Author: yang
 */

#include "YangAudioPlayAlsa.h"

#ifndef _WIN32
#include <yangutil/yang_unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <yangutil/sys/YangLog.h>
YangAudioPlayAlsa::YangAudioPlayAlsa(YangAudioInfo *pini) {        //m_rc=0;
	m_ini = pini;
	m_handle = NULL;
	m_size = 0;
	m_loops = 0;
	ret = 0;
	m_frames = 0;
	m_channel = pini->channel;
	m_isStart = 0;
	m_init = 0;
	m_in_audioBuffer = NULL;
	m_sample = pini->sample;

}

YangAudioPlayAlsa::~YangAudioPlayAlsa() {
	m_ini = NULL;
	closeAudio();
}

int YangAudioPlayAlsa::init() {
	if (m_init == 1)
		return Yang_Ok;

	if (m_ini->usingMono) {
		m_frames = 320;
		m_channel = 1;
		m_sample = 16000;
	} else if (m_ini->audioDecoderType == Yang_AED_AAC) {
		m_frames = 1024;
	} else if (m_ini->audioDecoderType == Yang_AED_OPUS) {
		m_frames = m_ini->sample / 50;
	}
	uint32_t val = 0;
	int32_t dir = 0;
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

	if ((err = snd_pcm_hw_params_set_channels(m_handle, hw_params, m_channel))
			< 0) {
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

	snd_pcm_hw_params_free(hw_params);
	m_size = m_frames * 2 * m_channel; // 2 bytes/sample, 2 channels

	m_init = 1;
	return Yang_Ok;

}

void YangAudioPlayAlsa::closeAudio() {

	if (m_handle) {
		snd_pcm_drain(m_handle);
		snd_pcm_close(m_handle);
		m_handle = NULL;
	}

}
void YangAudioPlayAlsa::setAudioList(YangAudioPlayBuffer *pal) {
	m_in_audioBuffer = pal;
}
void YangAudioPlayAlsa::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangAudioPlayAlsa::stop() {

	stopLoop();
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
		yang_usleep(1000);
		fprintf(stderr, "underrun occurred\n");
		snd_pcm_prepare(m_handle);
	} else if (ret < 0) {
		fprintf(stderr, "error from read: %s\n", snd_strerror(ret));
	}
}
/**
 int32_t YangAudioPlayAlsa::hasData() {
 for (int32_t i = 0; i < (int) m_in_audioBuffer->size(); i++) {
 if (m_in_audioBuffer->at(i)->m_size > 0)
 return 1;
 }
 return 0;
 }
 **/

void YangAudioPlayAlsa::startLoop() {

	m_loops = 1;
	int32_t status = 0;
	uint8_t *pcm = new uint8_t[SIZE_AUDIO_FRAME * 2];
	uint8_t *tmp = NULL;
	if ((status = snd_pcm_prepare(m_handle)) < 0) {
		fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
				snd_strerror(status));
		exit(1);
	}
	YangFrame frame;
	int err = 0;
	struct pollfd *ufds;
	int count = snd_pcm_poll_descriptors_count(m_handle);
	ufds = (struct pollfd *)malloc(sizeof(struct pollfd) * count);
	if ((err = snd_pcm_poll_descriptors(m_handle, ufds, count)) < 0) {
		yang_error("Unable to obtain poll descriptors for playback: %s\n",
				snd_strerror(err));
		return;
	}

	while (m_loops == 1) {
		unsigned short revents = 0;
		//int32_t err;
		err = snd_pcm_poll_descriptors_revents(m_handle, ufds, count, &revents);
		if (err < 0) {
			yang_error("error in alsa_device_playback_ready: %s",snd_strerror(err));
			return;
		}
		if (revents & POLLOUT) {
			if (m_in_audioBuffer->m_size == 0) 		continue;
			tmp = m_in_audioBuffer->getAudios(&frame);

			if (tmp)		status = snd_pcm_writei(m_handle, tmp, m_frames);

			if (status !=m_frames) {
				if (status == -EPIPE) {
					// EPIPE means overrun
					//	fprintf(stderr, "underrun occurred\n");
					yang_usleep(1000);
					snd_pcm_prepare(m_handle);
					continue;
					//snd_pcm_prepare(m_handle);
				}
				status = snd_pcm_recover(m_handle, status, 0);
				if (status < 0) {
					yang_error("ALSA write failed (unrecoverable): %s",	snd_strerror(status));
				}

			}
		}//endif

	}

	//snd_pcm_drain(m_handle);
	snd_pcm_close(m_handle);
	tmp = NULL;
	yang_deleteA(pcm);
	if(ufds) free(ufds);
	m_handle = NULL;

}
#endif
