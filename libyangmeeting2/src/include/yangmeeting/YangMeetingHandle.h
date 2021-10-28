#ifndef VIDEOMEETING_H
#define VIDEOMEETING_H
//#include "yangutil/yangavtype.h"
#include <vector>
#include <yangmeeting/yangmeetingtype.h>
#include <yangutil/buffer/YangVideoBuffer.h>
using namespace std;


class YangMeetingHandle {
public:
        YangMeetingHandle();
        virtual ~YangMeetingHandle();

public:
    virtual int32_t init()=0;
    virtual void stopAll()=0;
    virtual vector<YangUser*>* getUserList()=0;
    virtual YangVideoBuffer* getPreviewVideo()=0;
    virtual YangVideoBuffer* getVideoBuffer(int32_t uid)=0;
    virtual vector<YangVideoBuffer*>* getDecoderVideoBuffers()=0;
    virtual void change(int32_t st)=0;
    virtual int32_t sendHeartbeat()=0;
    virtual void setMeetingMessage(YangMeetingMessage* pms)=0;
    virtual void setPlayvideoHandle(YangVideoPlayHandle* playvideo)=0;
    virtual void delMessage()=0;
	int32_t m_state;
	YangMeetingContext *m_context;
};

#endif // VIDEOMEETING_H
