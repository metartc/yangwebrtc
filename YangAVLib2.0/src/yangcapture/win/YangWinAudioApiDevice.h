#ifndef SRC_YANGCAPTURE_SRC_WIN_YANGWINAUDIOAPI_H_
#define SRC_YANGCAPTURE_SRC_WIN_YANGWINAUDIOAPI_H_

#ifdef _WIN32
#include <vector>
#include <yangavutil/audio/YangAudioMix.h>
#include <yangavutil/audio/YangPreProcess.h>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <yangcapture/YangAudioCapture.h>
#include <yangutil/sys/YangLoadLib.h>

#include "YangWinAudioCaptureHandle.h"

#define UsingApi 1
#define UsingBuiltInAec 0

#if UsingApi
#include "api/YangWinAudioApiRender.h"
#else
#include "YangAudioPlaySdl.h"
#endif

#if UsingBuiltInAec
#include "api/YangWinAecI.h"
#else
#include "api/YangWinAudioApiCapture.h"
#endif


class YangWinAudioApiDevice: public YangAudioCapture {
public:
	YangWinAudioApiDevice(YangAudioInfo *pini);
	virtual ~YangWinAudioApiDevice();
	int init();
	void setInAudioBuffer(std::vector<YangAudioPlayBuffer*> *pal);
	void setPreProcess(YangPreProcess *pp);
	void setCatureStart();
	void setCatureStop();
	void setOutAudioBuffer(YangAudioBuffer *pbuffer);
	void setPlayAudoBuffer(YangAudioBuffer *pbuffer);
	void setAec(YangAecBase *paec);
	YangAudioCaptureHandle *m_ahandle;
	int m_isStart;
protected:

	void run();

	void startLoop();

	void stopLoop();
private:
     int m_loops;
	int m_isInit;
	int m_ret;
    YangFrame m_audioFrame;
    YangAudioMix m_mix;
    YangPreProcess *m_preProcess;
    int32_t m_size;
    vector<YangAudioPlayBuffer*> *m_in_audioBuffer;
    int32_t hasData();
    int32_t getRenderData(uint8_t *pdata);
private:

#if UsingBuiltInAec
     YangAecType *m_capture;
	YangLoadLib m_lib;
	void (*yang_get_winaec)(YangAecType *context);
	void (*yang_delete_winaec)(YangAecType *context);
	int (*yang_init_aec)(YangAecType *context);
	int (*yang_get_aec_buffer)(YangAecType *context);
#else
YangWinAudioApiCapture *m_capture;
#endif

#if UsingApi
    YangWinAudioApiRender* m_player;
  #else
    YangAudioPlaySdl *m_player;
#endif

};

#endif /* SRC_YANGCAPTURE_SRC_WIN_YANGWINAUDIOAPI_H_ */
#endif
