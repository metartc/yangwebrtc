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
#include "api/YangWinAudioApiRender.h"
#include "api/YangCaptureCallback.h"
#include "api/YangWinAudioApiCapture.h"



class YangWinAudioApiDevice: public YangAudioCapture,public YangCaptureCallback {
public:
	YangWinAudioApiDevice(YangAudioInfo *pini,bool usingBuiltinAec=false);
	virtual ~YangWinAudioApiDevice();
	int init();
	void setInAudioBuffer(std::vector<YangAudioPlayBuffer*> *pal);
	void setPreProcess(YangPreProcess *pp);
	void setCatureStart();
	void setCatureStop();
	void setOutAudioBuffer(YangAudioBuffer *pbuffer);
	void setPlayAudoBuffer(YangAudioBuffer *pbuffer);
	void setAec(YangAecBase *paec);
	void caputureAudioData(YangFrame* audioFrame);
	YangAudioCaptureHandle *m_ahandle;
	int m_isStart;
	void stop();
protected:

	void run();

	void startLoop();

	void stopLoop();

	pthread_mutex_t m_lock;
	pthread_cond_t m_cond_mess;
private:
    int m_loops;
	int m_isInit;


private:
	bool m_usingBuiltinAec;
	YangAudioApiCapture *m_capture;
    YangWinAudioApiRenders* m_player;
};

#endif /* SRC_YANGCAPTURE_SRC_WIN_YANGWINAUDIOAPI_H_ */
#endif
