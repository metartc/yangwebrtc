/*
 * YangAudioPlayAlsa.h
 *
 *  Created on: 2020年9月4日
 *      Author: yang
 */

#ifndef YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_
#define YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_
#ifndef _WIN32
#include <alsa/asoundlib.h>

#include <yangplayer/YangAudioPlay.h>

class YangAudioPlayAlsa :public YangAudioPlay{
public:
	YangAudioPlayAlsa(YangAudioInfo *pini);
	~YangAudioPlayAlsa();
	void init();
protected:
	void startLoop();
	void stopLoop();

	// uint8_t *m_buffer[YangAudioBufferMaxNum];
private:
	 //uint8_t *m_buffer;
	snd_pcm_t *m_handle;
	snd_pcm_uframes_t m_frames;
	int32_t m_channel;
	int32_t hasData();
	void yangMix1(uint8_t *pin, uint8_t *pout);
	void mixAudio();
	void mixAudio1();
	void closeAudio();
	int32_t ret;
	int32_t m_size;
	int32_t m_loops;


	void play(uint8_t *p_data);


};
#endif
#endif /* YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_ */
