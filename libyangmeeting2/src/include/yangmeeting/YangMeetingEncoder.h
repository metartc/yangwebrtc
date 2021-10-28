/*
 * YangEncoderBase.h
 *
 *  Created on: 2019年10月11日
 *      Author: yang
 */

#ifndef YANGAPP_YangMeetingEncoder_H_
#define YANGAPP_YangMeetingEncoder_H_
#include <yangmeeting/yangmeetingtype.h>
#include "yangencoder/YangAudioEncoderHandle.h"
#include "yangencoder/YangVideoEncoderHandle.h"
#include "yangutil/buffer/YangAudioBuffer.h"
#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/buffer/YangVideoEncoderBuffer.h"
#include "yangutil/sys/YangIni.h"

class YangMeetingEncoder {
public:
	YangMeetingEncoder(YangMeetingContext *pcontext);
	virtual ~YangMeetingEncoder();
public:
	void initVideoEncoder();
	void initAudioEncoder();
	void startAudioEncoder();
	void startVideoEncoder();
	void setInAudioBuffer(YangAudioBuffer *pal);
	void setInVideoBuffer(YangVideoBuffer *pvl);
	YangAudioEncoderBuffer * getOutAudioBuffer();
	YangVideoEncoderBuffer * getOutVideoBuffer();
	YangVideoMeta * getOutVideoMetaData();
	void stopAll();
	void sendKeyframe();
private:
	YangVideoEncoderHandle *m_ve;
	YangAudioEncoderHandle *m_ae;

	YangAudioEncoderBuffer *m_out_auidoBuffer;
	YangVideoEncoderBuffer *m_out_videoBuffer;

	YangMeetingContext *m_context;
	YangVideoMeta *m_vmd;
};

#endif /* YANGAPP_YANGENCODERAPP_H_ */
