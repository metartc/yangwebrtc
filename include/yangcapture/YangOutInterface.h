
#ifndef INCLUDE_YANGCAPTURE_YANGOUTINTERFACE_H_
#define INCLUDE_YANGCAPTURE_YANGOUTINTERFACE_H_

#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/sys/YangThread.h"
class YangOutInterface:public YangThread {
public:
	YangOutInterface();
	virtual ~YangOutInterface();
	int32_t m_isStart;
public:
	virtual int32_t init()=0;
	virtual void setVideoCaptureStart()=0;
	virtual void setVideoCaptureStop()=0;
	virtual YangVideoBuffer* getOutVideoBuffer()=0;
	virtual YangVideoBuffer* getPreVideoBuffer()=0;
	virtual int32_t getVideoCaptureState()=0;
	void stop();
protected:
    void run();
    virtual void startLoop()=0;
     virtual  void stopLoop()=0;
};

#endif /* INCLUDE_YANGCAPTURE_YANGOUTINTERFACE_H_ */
