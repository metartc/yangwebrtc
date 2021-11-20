#ifndef SRC_YANGCAPTURE_YANGOUTINTERFACEIMPL_H_
#define SRC_YANGCAPTURE_YANGOUTINTERFACEIMPL_H_



#include <yangcapture/YangOutInterface.h>
#include "yangutil/buffer/YangVideoBuffer.h"
#include <yangutil/yangavinfotype.h>

class YangOutInterfaceImpl:public YangOutInterface {
public:
	YangOutInterfaceImpl(YangContext* pcontext);
	virtual ~YangOutInterfaceImpl();
	int32_t m_width,m_height;
	int32_t init();
	void setInterval(int32_t pinterval);
	void setDrawmouse(bool isDraw);
	bool getisDrawmouse();
	YangVideoBuffer * getOutVideoBuffer();
	YangVideoBuffer * getPreVideoBuffer();
	void setVideoCaptureStart();
	void setVideoCaptureStop();
	int32_t getVideoCaptureState();
	YangVideoBuffer* getScreenBuffer();
    uint8_t *m_rgb;
	int32_t m_bufLen,isCapture;
	int32_t m_fd;
	void putBuffer(YangFrame* videoFrame);
	void startLoop();
	void stopLoop();
private:
	YangVideoBuffer *m_out_videoBuffer;
	YangVideoBuffer *m_pre_videoBuffer;
	YangContext* m_context;
	int m_loop;
	int m_state;
	int32_t m_interval;
	bool m_isDrawmouse;
};
#endif /* SRC_YANGCAPTURE_YANGOUTINTERFACEIMPL_H_ */
