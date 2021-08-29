/*
 * YangAecBase.h
 *
 *  Created on: 2019年11月16日
 *      Author: yang
 */

#ifndef YANGUTIL_AUDIO_YANGAECBASE_H_
#define YANGUTIL_AUDIO_YANGAECBASE_H_
#include "yangutil/buffer/YangAudioBuffer.h"
class YangAecBase {
public:
	YangAecBase();
	virtual ~YangAecBase();
	virtual void init(int pFrameSize,int sampling_rate,int pchannel)=0;
	virtual void echo_state_reset()=0;
	virtual void echo_playback(short *play)=0;
	virtual void put_echo_playback(unsigned char *play,int plen)=0;
	virtual void echo_capture(short *rec, short *out)=0;
	virtual void preprocess_run(short* pcm)=0;
	virtual void echo_cancellation(const short *rec, const short *play, short *out)=0;
	virtual void setPlayBuffer(YangAudioBuffer *pab)=0;
	int m_channel;
	int m_frameSize;
	int m_sampleRate;
	int m_echoPath;
};

#endif /* YANGUTIL_AUDIO_YANGAECBASE_H_ */
