#ifndef YANGPLAYER_INCLUDE_YANGAVSYN_H_
#define YANGPLAYER_INCLUDE_YANGAVSYN_H_
#include <yangutil/sys/YangTime.h>
#include <yangutil/YangSynBufferI.h>
#include <yangutil/yangavtype.h>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <yangavutil/audio/YangAudioMix.h>
#include <yangutil/buffer/YangVideoBuffer.h>

enum YangSynType{
	YANG_SYNC_AUDIO_MASTER, /* default choice */
	YANG_SYNC_VIDEO_MASTER,
	YANG_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};
#define Yang_Max_Audio_Intervaltime 20
#define Yang_Max_Video_Intervaltime 40
#define Yang_Video_Cache_time 100
class YangSynBuffer //:public YangSynBufferI
{
public:
    YangSynBuffer();
    virtual ~YangSynBuffer();
    int32_t m_uid;
    int32_t m_width;
    int32_t m_height;
    void setTranstype(int transtype);

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

    //int32_t playAudioFrame(YangFrame* pframe);
    bool playAudioFrame(int64_t pts);
    //int32_t playVideoFrame(YangFrame* pframe);
    int playVideoFrame(int64_t pts);
    double compute_target_delay(double delay);
    void video_refresh(int32_t pisaudio, double *remaining_time);
    int32_t m_maxAudioMinus;
    int32_t m_maxVideoMinus;
private:
    bool m_isFirstVideo;
    bool m_isFirstAudio;
    int32_t m_video_time_state;
    void updateBaseTimestamp(int64_t pts);

private:
    //YangTime m_time;
     YangSynType m_synType;
    int32_t m_paused;
    int64_t m_baseClock,m_startClock;
    int32_t m_videoClock;
    int32_t m_audioClock;

    int64_t m_audioBase;
    int64_t m_videoBase;
    int64_t m_video_startClock;


    int64_t m_audioTime;
    int64_t m_videoTime;
    int32_t m_audioMinus;
    int32_t m_videoMinus;

    int64_t m_pre_audioTime;
    int64_t m_pre_videoTime;

    int m_transtype;


private:
    YangVideoBuffer *m_videoBuffer;
    YangAudioPlayBuffer* m_audioBuffer;






};

#endif /* YANGPLAYER_INCLUDE_YANGAVSYN_H_ */
