/*
 * YangAudioDataManager.h
 *
 *  Created on: 2021年9月21日
 *      Author: yang
 */

#ifndef INCLUDE_YANGCAPTURE_YANGRENDERAUDIODATA_H_
#define INCLUDE_YANGCAPTURE_YANGRENDERAUDIODATA_H_
#include <stdint.h>
#include <vector>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <yangavutil/audio/YangPreProcess.h>
#include <yangavutil/audio/YangAudioUtil.h>
#include <yangavutil/audio/YangAudioMix.h>
using namespace std;

class YangRenderAudioData {
public:
    YangRenderAudioData();
    virtual ~YangRenderAudioData();

public:
    YangPreProcess* m_preProcess;
    YangAudioResample m_res;
    bool m_render20ms;


    void initRender(int psample,int pchannel);
    void initPlay(int psample,int pchannel);

    uint8_t* getRenderAudioData(int len);
    uint8_t* getAecAudioData();

    void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal);
    void setAudioList(YangAudioPlayBuffer *pal);

private:
    YangAudioPlayBuffer* m_in_audioBuffer;
    vector<YangAudioPlayBuffer*> *m_in_audioBuffers;
    YangAudioMix m_mix;


    int m_bufLen;
    uint8_t* m_mixBuf;

    uint8_t* m_cache;
    int m_cacheLen;
    int m_size;
    int m_pos;

    YangFrame m_audioFrame;
private:
    uint8_t* getAudioData(YangFrame* frame);
    void setAudioData(YangFrame* frame);
    bool hasData();
};

#endif /* INCLUDE_YANGCAPTURE_YANGRENDERAUDIODATA_H_ */
