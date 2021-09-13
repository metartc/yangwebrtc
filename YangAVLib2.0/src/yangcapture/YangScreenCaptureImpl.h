/*
 * YangScreenCapture.h
 *
 *  Created on: 2020年8月30日
 *      Author: yang
 */

#ifndef YANGCAPTURE_SRC_YANGSCREENCAPTUREIMPL_H_
#define YANGCAPTURE_SRC_YANGSCREENCAPTUREIMPL_H_
#include <yangcapture/YangScreenCapture.h>
#include "yangutil/buffer/YangVideoBuffer.h"
#include <yangutil/yangavtype.h>
class YangScreenCaptureImpl:public YangScreenCapture {
public:
	YangScreenCaptureImpl();
	virtual ~YangScreenCaptureImpl();
	int32_t m_width,m_height;
	int32_t init();
	int32_t captureFrame(uint8_t* p);
	void  setVideoBuffer(YangVideoBuffer *pbuf);
	YangVideoBuffer* getScreenBuffer();
	//uint8_t *trgb;
	uint8_t *m_rgb;
	int32_t m_bufLen,isCapture;
	int32_t m_fd;
#ifdef _WIN32
#else

#endif

	void putBuffer(YangFrame* videoFrame);
private:
	YangVideoBuffer *m_out_videoBuffer;
};

#endif /* YANGCAPTURE_SRC_YANGSCREENCAPTUREIMPL_H_ */
