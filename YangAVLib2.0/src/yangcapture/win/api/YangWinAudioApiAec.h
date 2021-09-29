/*
 * YangWinAudioApiAec.h
 *
 *  Created on: 2021年9月20日
 *      Author: yang
 */
#ifndef YANGCAPTURE_WIN_API_YANGWINAUDIOAPIAEC_H_
#define YANGCAPTURE_WIN_API_YANGWINAUDIOAPIAEC_H_
#ifdef _MSC_VER
#include <mmdeviceapi.h>
#include <dmo.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLoadLib.h>
#include "YangAudioApiCapture.h"
#include "YangWinAudioApi.h"
#include <yangcapture/YangCaptureAudioData.h>

class YangWinAudioApiAec:public YangAudioApiCapture,public YangWinAudioApi {
public:
    YangWinAudioApiAec();
	virtual ~YangWinAudioApiAec();
	void setCaptureCallback(YangCaptureCallback* cb);
    int initCapture();
    int startCpature();
    int stopCapture();
    void captureThread();
    int getAudioOutLength(){return 640;}


     int initRecordingDMO();
     int setDMOProperties();
     int getRenderIndex();
    // int getBuffer(uint8_t* p,unsigned long* pbufLen);
     YangCaptureAudioData m_audioData;

	void stop();
protected:
	void run();
	void startLoop();
	void stopLoop();
	 int setBoolProperty(IPropertyStore* ptrPS, REFPROPERTYKEY key,VARIANT_BOOL value);
	 int setVtI4Property(IPropertyStore* ptrPS,REFPROPERTYKEY key,LONG value);
	int m_loops;
    //YangCaptureCallback* m_cb;
private:
	   IMediaObject* m_dmo;
	   IMediaBuffer* m_mediaBuffer;
	    DMO_OUTPUT_DATA_BUFFER OutputBufferStruct;
	    DWORD m_dwStatus;

	    int m_micIndex;
	    BYTE* m_dataBuf;
	   IPropertyStore* m_ps;
      // YangRecData m_recData;
	   YangFrame m_audioFrame;
};
#endif
#endif /* YANGCAPTURE_WIN_API_YANGWINAUDIOAPIAEC_H_ */
