#ifndef INCLUDE_YANGPUSH_YANGPUSHHANDLE_H_
#define INCLUDE_YANGPUSH_YANGPUSHHANDLE_H_
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangSysMessageI.h>
#include <string>
class YangPushHandle {
public:
	YangPushHandle();
	virtual ~YangPushHandle();
    virtual int publish(std::string url,std::string localIp,int32_t localport)=0;
    virtual void disconnect()=0;
	virtual void init()=0;
	//virtual void startCapture()=0;
	virtual YangVideoBuffer* getPreVideoBuffer()=0;
	static YangPushHandle* createPushHandle(YangContext* pcontext,YangSysMessageI* pmessage);



};



#endif /* INCLUDE_YANGPUSH_YANGPUSHHANDLE_H_ */
