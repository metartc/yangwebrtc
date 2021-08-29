/*
 * YangPlayBase.h
 *
 *  Created on: 2019年10月11日
 *      Author: yang
 */

#ifndef YANGAPP_YANGPLAYAPP_H_
#define YANGAPP_YANGPLAYAPP_H_
#include <yangdecoder/YangVideoDecoderHandles.h>
#include <yangplayer/YangAudioPlayerSdl.h>
#include <yangplayer/YangVideoPlay.h>
#include <vector>

#include "yangavutil/audio/YangAecBase.h"
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/sys/YangIni.h"
//#include "yangvideomeeting/yangmeetingtype.h"
using namespace std;
class YangPlayerPlay {
public:
	YangPlayerPlay();
	virtual ~YangPlayerPlay();
	void initAudioPlay(YangAudioInfo* paudio);
	//void initVideoPlay(YangVideoDecoderHandle *handle);
	void startAudioPlay();
	//void startVideoPlay();
	void setInAudioList(YangAudioPlayBuffer *paudioList);
	//void setInVideoList(vector<YangVideoBuffer*> *pvideoList);
	//void setAec(YangAecBase *paec);
	//int getIsAecInit();
	void stopAll();
private:
	YangAudioPlayerSdl *vm_audioPlay;
	//YangVideoPlay *vm_videoPlay;
	int vm_audio_player_start;
	//YangMeetingContext *m_ini;
};

#endif /* YANGAPP_YANGPLAYAPP_H_ */
