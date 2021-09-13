#ifndef SRC_YANGCAPTURE_SRC_WIN_YANGWINAUDIOAPI_H_
#define SRC_YANGCAPTURE_SRC_WIN_YANGWINAUDIOAPI_H_

#ifdef _WIN32
#include <vector>
#include <yangavutil/audio/YangAudioMix.h>
#include <yangavutil/audio/YangPreProcess.h>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <yangcapture/YangAudioCapture.h>
#include <yangutil/sys/YangLoadLib.h>
#include "api/YangWinAecI.h"
#include "YangWinAudioCaptureHandle.h"

#define UsingApi 0
#if UsingApi
#include "api/YangWinAudioApiRender.h"
#else
#include "YangAudioPlaySdl.h"
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
	YangAecType *m_aec;
#if UsingApi
    YangWinAudioApiRender* m_player;
  #else
	YangAudioPlaySdl *m_player;
#endif
	void run();

	void startLoop();

	void stopLoop();
private:

	int m_isInit;
	int m_ret;

private:
	int m_size;
	int m_loops;

	YangLoadLib m_lib;
	void (*yang_get_winaec)(YangAecType *context);
	void (*yang_delete_winaec)(YangAecType *context);
	int (*yang_init_aec)(YangAecType *context);
	int (*yang_get_aec_buffer)(YangAecType *context);

};

#endif /* SRC_YANGCAPTURE_SRC_WIN_YANGWINAUDIOAPI_H_ */
#endif
