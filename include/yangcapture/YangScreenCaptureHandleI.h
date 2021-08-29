#ifndef YangScreenCaptureI_H_
#define YangScreenCaptureI_H_
class YangScreenCaptureHandleI {
public:
	YangScreenCaptureHandleI(){}
	virtual ~YangScreenCaptureHandleI(){}
	int m_width=0,m_height=0;
	virtual int init()=0;
	virtual int captureFrame(unsigned char* p)=0;
};

#endif
