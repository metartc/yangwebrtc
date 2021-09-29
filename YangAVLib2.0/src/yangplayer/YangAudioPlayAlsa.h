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

class YangAudioPlayAlsa:public YangThread{
public:
	YangAudioPlayAlsa(YangAudioInfo *pini);
	~YangAudioPlayAlsa();
	//void init();
    void setAudioList(YangAudioPlayBuffer *pal);
    int init();
    void stop();
    int32_t m_loops;
    int32_t m_isStart;
protected:
	void startLoop();
	void stopLoop();
	 void run();
private:
    //std::vector<YangAudioPlayBuffer*> *m_in_audioBuffer;
    YangAudioPlayBuffer* m_in_audioBuffer;

private:
    YangAudioInfo* m_ini;
    int32_t m_init;
	snd_pcm_t *m_handle;

	int32_t m_sample;
	int32_t m_channel;
	snd_pcm_uframes_t m_frames;

	int32_t hasData();
	void yangMix1(uint8_t *pin, uint8_t *pout);

	void mixAudio();
	void mixAudio1();
	void closeAudio();
	int32_t ret;
	int32_t m_size;
	void play(uint8_t *p_data);
};
#endif
#endif /* YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_ */
