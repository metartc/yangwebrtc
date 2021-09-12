/*
 * YangPlayBase.h
 *
 *  Created on: 2019年10月11日
 *      Author: yang
 */

#ifndef YANGAPP_YANGPLAYAPP_H_
#define YANGAPP_YANGPLAYAPP_H_
#include <yangmeeting/yangmeetingtype.h>
#include <vector>

#include "yangavutil/audio/YangAecBase.h"
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/sys/YangIni.h"
using namespace std;
class YangMeetingAudioPlay {
public:
	YangMeetingAudioPlay(YangMeetingContext *pini);
	virtual ~YangMeetingAudioPlay();
	void initAudioPlay();
	void startAudioPlay();
	void setInAudioList(vector<YangAudioPlayBuffer*> *paudioList);
	void setAec(YangAecBase *paec);
	int32_t getIsAecInit();

	//void stopAll();
private:
	YangMeetingContext *m_ini;
};

#endif /* YANGAPP_YANGPLAYAPP_H_ */
