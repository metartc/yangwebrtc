#ifndef VIDEOMEETING_H
#define VIDEOMEETING_H
//#include "yangutil/yangtype.h"
#include <yangmeeting/yangmeetingtype.h>
#include <vector>

#include "yangutil/buffer/YangVideoBuffer.h"
using namespace std;


class YangMeetingHandle {
public:
        YangMeetingHandle();
        virtual ~YangMeetingHandle();

public:
    virtual int init()=0;
    virtual void stopAll()=0;
    virtual vector<YangUser*>* getUserList()=0;
    virtual YangVideoBuffer* getPreviewVideo()=0;
    virtual YangVideoBuffer* getVideoBuffer(int uid)=0;
    virtual vector<YangVideoBuffer*>* getDecoderVideoBuffers()=0;
    virtual void change(int st)=0;
    virtual int sendHeartbeat()=0;
	int m_state;
	YangMeetingContext *m_ini;
};

#endif // VIDEOMEETING_H
