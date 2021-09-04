#ifndef YANGPLAYER_INCLUDE_YANGAVSYN_H_
#define YANGPLAYER_INCLUDE_YANGAVSYN_H_
#include "yangutil/sys/YangTime.h"
#include "yangutil/YangAvsynI.h"
 enum YangSynType{
     AV_SYNC_AUDIO_MASTER, /* default choice */
     AV_SYNC_VIDEO_MASTER,
     AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
 };
class YangAVSyn :public YangAvsynI{
public:
	YangAVSyn();
	virtual ~YangAVSyn();
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


};

#endif /* YANGPLAYER_INCLUDE_YANGAVSYN_H_ */
