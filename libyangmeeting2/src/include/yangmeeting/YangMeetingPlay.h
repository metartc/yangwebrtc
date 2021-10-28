/*
 * YangMeetingPlay.h
 *
 *  Created on: 2019年9月11日
 *      Author: yang
 */

#ifndef MeetingPlayoBase_H
#define MeetingPlayoBase_H
#include <yangmeeting/YangMeetingDecoder.h>
#include <yangmeeting/YangMeetingNet.h>
#include <yangmeeting/yangmeetingtype.h>
#include <vector>

#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/buffer/YangVideoDecoderBuffer.h"

using namespace std;

class YangMeetingPlay {
public:
	YangMeetingPlay(YangMeetingContext *pcontext);
	virtual ~YangMeetingPlay();
	YangMeetingDecoder *m_ydb;

	void notifyMediaSource(int32_t puid,YangAudioParam* audio,YangVideoParam* video);
	void startAudioDecoder(YangMeetingNet *prr);
	void startVideoDecoder(YangMeetingNet *prr);
	void init(YangMeetingNet *prr);
	void unPlayAudio(int32_t puid);
	void unPlayVideo(int32_t puid);
	void setNetBuffer(YangMeetingNet *prr);
	void stopAll();

	void removeAllStream();
protected:

private:
	YangMeetingContext *m_context;
};

#endif //
