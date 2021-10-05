#ifndef YANGPLAYER_INCLUDE_YANGAVSYN_H_
#define YANGPLAYER_INCLUDE_YANGAVSYN_H_
#include <yangutil/sys/YangTime.h>
#include <yangutil/YangSynBufferI.h>
#include <yangutil/yangavtype.h>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <yangavutil/audio/YangAudioMix.h>
#include <yangutil/buffer/YangVideoBuffer.h>

enum YangSynType{
    AV_SYNC_AUDIO_MASTER, /* default choice */
    AV_SYNC_VIDEO_MASTER,
    AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};

class YangSynBuffer :public YangSynBufferI{
public:
    YangSynBuffer();
    virtual ~YangSynBuffer();
    int32_t m_uid;
    int32_t m_width;
    int32_t m_height;


public:
    uint8_t* getVideoRef(YangFrame* pframe);
    uint8_t* getAudioRef(YangFrame* audioFrame);
    int32_t getAudioSize();
    int32_t getVideoSize();

    //void setAudioBuffers(vector<YangAudioPlayBuffer*> *paudioList);
    void setInAudioBuffer(YangAudioPlayBuffer *paudioList);
    void setInVideoBuffer(YangVideoBuffer *pbuf);


    void initClock();
    void setClock();

    int32_t playAudioFrame(int64_t pts);
    int32_t playVideoFrame(int64_t pts);
    double compute_target_delay(double delay);
    void video_refresh(int32_t pisaudio, double *remaining_time);
    int32_t m_maxMinus;
private:
    void updateBaseTimestamp(int64_t pts);

private:
    //YangTime m_time;
    // YangSynType m_synType;
    int32_t m_paused;
    int64_t m_baseClock,m_startClock;


private:
    YangVideoBuffer *m_videoBuffer;
    YangAudioPlayBuffer* m_audioBuffer;






};

#endif /* YANGPLAYER_INCLUDE_YANGAVSYN_H_ */
