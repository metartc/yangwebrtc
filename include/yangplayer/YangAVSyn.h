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

	int playAudioFrame(long long pts);
	int playVideoFrame(long long pts);
	 double compute_target_delay(double delay);
	 void video_refresh(int pisaudio, double *remaining_time);
	 int m_maxMinus;
private:
	 void updateBaseTimestamp(long long pts);

private:
	//YangTime m_time;
	// YangSynType m_synType;
	 int m_paused;
	long long m_baseClock,m_startClock;

private:


};

#endif /* YANGPLAYER_INCLUDE_YANGAVSYN_H_ */
