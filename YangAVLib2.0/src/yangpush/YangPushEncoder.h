#ifndef YANGAPP_YangPushEncoder_H_
#define YANGAPP_YangPushEncoder_H_

#include "yangencoder/YangAudioEncoderHandle.h"
#include "yangencoder/YangVideoEncoderHandle.h"
#include "yangutil/buffer/YangAudioBuffer.h"
#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/buffer/YangVideoEncoderBuffer.h"
#include "yangutil/sys/YangIni.h"
class YangPushEncoder {
public:
	YangPushEncoder(YangContext *pini);
	virtual ~YangPushEncoder();
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

	YangContext *m_ini;
	YangVideoMeta *m_vmd;
};

#endif /* YANGAPP_YANGENCODERAPP_H_ */
