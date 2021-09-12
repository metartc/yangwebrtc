
#ifndef INCLUDE_YANGWEBRTC_YANGRTCHANDLE_H_
#define INCLUDE_YANGWEBRTC_YANGRTCHANDLE_H_
#include <yangstream/YangAudioStreamFrame.h>
#include <yangstream/YangStreamType.h>
#include <yangstream/YangVideoStreamFrame.h>
#include <yangutil/YangMediaConfigCallback.h>
class YangRtcHandle
{
public:
	YangRtcHandle();
	virtual ~YangRtcHandle();
	virtual void init(YangStreamConfig* pconf)=0;
        virtual void setMediaParam(YangAudioInfo* audioConf,YangVideoInfo* videoConf,YangRtcInfo* rtcInfo)=0;
	virtual int32_t connectRtcServer()=0;
	virtual int32_t disConnectServer()=0;
	virtual int32_t getState()=0;
	virtual int32_t publishVideo(YangVideoStreamFrame* videoFrame)=0;
	virtual int32_t publishAudio(YangAudioStreamFrame* audioFrame)=0;
	virtual void setRemoteMediaConfigCallback(YangMediaConfigCallback* cbk)=0;
	virtual void setSendRequestCallback(YangSendRequestCallback *cb)=0;
	void setReceiveCallback(YangReceiveCallback* cbk);
	static YangRtcHandle* createRtcHandle();
protected:
	YangReceiveCallback* m_recvcb;
	YangStreamConfig *m_conf;
};


#endif /* INCLUDE_YANGWEBRTC_YANGRTCHANDLE_H_ */
