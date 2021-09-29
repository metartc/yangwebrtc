#ifndef SRC_YANGCAPTURE_SRC_WIN_YANGWINAUDIOAPI_H_
#define SRC_YANGCAPTURE_SRC_WIN_YANGWINAUDIOAPI_H_

#ifdef _WIN32
#include <vector>
#include <yangavutil/audio/YangAudioMix.h>
#include <yangavutil/audio/YangPreProcess.h>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <yangcapture/YangAudioCapture.h>
#include <yangutil/sys/YangLoadLib.h>

#include "../YangWinAudioCaptureHandle.h"
#include "YangWinAudioApiRender.h"
#include <yangcapture/YangCaptureCallback.h>
#include "YangWinAudioApiCapture.h"



class YangWinAudioApiDevice: public YangAudioCapture,public YangCaptureCallback {
public:
	YangWinAudioApiDevice(YangAudioInfo *pini,bool isRecord,bool usingBuiltinAec=false);
	 ~YangWinAudioApiDevice();
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

protected:
	void startLoop();
	void stopLoop();

private:
    int m_loops;
	int m_isInit;
	int m_outLen;


private:
	bool m_usingBuiltinAec;
	YangAudioApiCapture *m_capture;
    YangWinAudioApiRenders* m_player;
};

#endif /* SRC_YANGCAPTURE_SRC_WIN_YANGWINAUDIOAPI_H_ */
#endif
