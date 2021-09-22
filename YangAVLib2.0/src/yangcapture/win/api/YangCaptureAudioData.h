
#ifndef YANGCAPTURE_WIN_API_YANGCAPTUREAUDIODATA_H_
#define YANGCAPTURE_WIN_API_YANGCAPTUREAUDIODATA_H_
#include <stdint.h>
#include <yangutil/yangavtype.h>
#include <yangavutil/audio/YangResample.h>
#include <yangavutil/audio/YangAecBase.h>
#include <yangutil/buffer/YangAudioBuffer.h>
#include "YangCaptureCallback.h"
class YangCaptureAudioData {
public:
	YangCaptureAudioData();
	virtual ~YangCaptureAudioData();
	void initIn(int psample,int pchannel);
	void initOut(int psample,int pchannel);
	void caputure(YangFrame* audioFrame);
	YangCaptureCallback* m_cb;
private:
	uint8_t* m_cache;
	int m_cacheLen;
	int m_size;
	int m_pos;

	int n20mslen_in;
	int n20mslen_out;
	int n20msframes_in;
	int n20msframes_out;
private:
	int m_inSample;
	int m_outSample;
	int m_inChannel;
	int m_outChannel;
	//int m_bufLen;

	bool m_sampleTrans;
	bool m_channelTrans;
	uint8_t* m_resBuf;
	uint8_t* m_resTmp;
	YangResample m_res;
	YangFrame m_audioFrame;
	bool m_isInitTrans;
	void initTrans();
	void captureData();
};

#endif /* YANGCAPTURE_WIN_API_YANGCAPTUREAUDIODATA_H_ */
