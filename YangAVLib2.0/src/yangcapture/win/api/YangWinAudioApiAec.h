/*
 * YangWinAudioApiAec.h
 *
 *  Created on: 2021年9月20日
 *      Author: yang
 */
#ifndef YANGCAPTURE_WIN_API_YANGWINAUDIOAPIAEC_H_
#define YANGCAPTURE_WIN_API_YANGWINAUDIOAPIAEC_H_
#ifdef _WIN32
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLoadLib.h>
#include "YangAudioApiCapture.h"
#include "YangWinAecI.h"
class YangWinAudioApiAec:public YangAudioApiCapture {
public:
	YangWinAudioApiAec(YangAudioInfo *pini);
	virtual ~YangWinAudioApiAec();
	void setCaptureCallback(YangCaptureCallback* cb);
    int initCapture();
    int startCpature();
    int stopCapture();
	int m_isStart;
	void stop();
protected:
	void run();
	void startLoop();
	void stopLoop();

	int m_loops;
	YangCaptureCallback* m_cb;
private:
    YangAecType *m_capture;
	YangLoadLib m_lib;
	void (*yang_get_winaec)(YangAecType *context);
	void (*yang_delete_winaec)(YangAecType *context);
	int (*yang_init_aec)(YangAecType *context);
	int (*yang_get_aec_buffer)(YangAecType *context);
};
#endif
#endif /* YANGCAPTURE_WIN_API_YANGWINAUDIOAPIAEC_H_ */
