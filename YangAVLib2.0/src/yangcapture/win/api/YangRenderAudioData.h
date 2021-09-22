/*
 * YangAudioDataManager.h
 *
 *  Created on: 2021年9月21日
 *      Author: yang
 */

#ifndef SRC_YANGCAPTURE_WIN_API_YANGRENDERAUDIODATA_H_
#define SRC_YANGCAPTURE_WIN_API_YANGRENDERAUDIODATA_H_
#include <stdint.h>
#include <vector>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <yangavutil/audio/YangPreProcess.h>
#include <yangavutil/audio/YangResample.h>
#include <yangavutil/audio/YangAudioMix.h>
class YangRenderAudioData {
public:
	YangRenderAudioData();
	virtual ~YangRenderAudioData();
	void initRender(int psample,int pchannel);
	void initPlay(int psample,int pchannel);
	void setAudioList(YangAudioPlayBuffer *pal);
	uint8_t* getRenderAudioData(int len);
    uint8_t* getAecAudioData();
    bool n20ms;
    YangPreProcess* m_preProcess;
    void setInAudioBuffer(std::vector<YangAudioPlayBuffer*> *pal);

private:
     YangAudioPlayBuffer* m_in_audioBuffer;
     YangAudioMix m_mix;

    std::vector<YangAudioPlayBuffer*> *m_in_audioBuffers;
	uint8_t* m_cache;
	int m_cacheLen;
	int m_size;
	int m_pos;
	YangFrame m_audioFrame;
private:
	int m_renderSample;
	int m_playSample;
	int m_renderChannel;
	int m_playChannel;
	int m_bufLen;


	bool m_sampleTrans;
	bool m_channelTrans;
	uint8_t* m_resBuf;
	uint8_t* m_resTmp;
    uint8_t* m_mixBuf;

	YangResample m_res;
	bool m_isInitTrans;
	void initTrans();
	uint8_t* getAudioData(YangFrame* frame);
	void setAudioData(YangFrame* frame);
	int32_t hasData();
};

#endif /* SRC_YANGCAPTURE_WIN_API_YANGRENDERAUDIODATA_H_ */
