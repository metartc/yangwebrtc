#ifndef YangMultiVideoCapture_H
#define YangMultiVideoCapture_H
#include "yangutil/sys/YangThread.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/sys/YangLog.h"
#include "YangVideoCapture.h"
class YangMultiVideoCapture: public YangVideoCapture {
public:
	YangMultiVideoCapture();
	virtual ~YangMultiVideoCapture();
public:
	virtual void setLivingOutVideoBuffer(YangVideoBuffer *pbuf)=0;
	virtual void setLivingVideoCaptureStart()=0;
	virtual void setLivingVideoCaptureStop()=0;
	virtual int getLivingVideoCaptureState()=0;

	virtual void setFilmOutVideoBuffer(YangVideoBuffer *pbuf)=0;
	virtual void setFilmVideoCaptureStart()=0;
	virtual void setFilmVideoCaptureStop()=0;
	virtual int getFilmVideoCaptureState()=0;
};

#endif // YANGVIDEOCAPTURE_H
