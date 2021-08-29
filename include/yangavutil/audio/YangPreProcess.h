/*
 * YangAudioPreProcess.h
 *
 *  Created on: 2020年9月29日
 *      Author: yang
 */

#ifndef YANGUTIL_AUDIO_YANGPREPROCESS_H_
#define YANGUTIL_AUDIO_YANGPREPROCESS_H_

class YangPreProcess {
public:
	YangPreProcess();
	virtual ~YangPreProcess();
	virtual void init(int pFrameSize,int sampling_rate,int pchannel)=0;
	virtual void state_reset()=0;
	virtual void preprocess_run(short* pcm)=0;
	int m_channel;
	int m_frameSize;
	int m_sampleRate;
};

#endif /* YANGUTIL_AUDIO_YANGPREPROCESS_H_ */
