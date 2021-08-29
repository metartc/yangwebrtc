
#ifndef INCLUDE_YANGWEBRTC_YANGRTCHANDLE_H_
#define INCLUDE_YANGWEBRTC_YANGRTCHANDLE_H_
#include <yangstream/YangStreamType.h>
#include <yangstream/YangAudioFrame.h>
#include <yangstream/YangVideoFrame.h>
#include <yangutil/YangMediaConfigCallback.h>
class YangRtcHandle
{
public:
	YangRtcHandle();
	virtual ~YangRtcHandle();
	virtual void init(YangStreamConfig* pconf)=0;
	virtual void setMediaParam(YangAudioInfo* audioConf,YangVideoInfo* videoConf)=0;
	virtual int connectRtcServer()=0;
	virtual int disConnectServer()=0;
	virtual int getState()=0;
	virtual int publishVideo(YangVideoFrame* videoFrame)=0;
	virtual int publishAudio(YangAudioFrame* audioFrame)=0;
	virtual void setRemoteMediaConfigCallback(YangMediaConfigCallback* cbk)=0;
	virtual void setSendRequestCallback(YangSendRequestCallback *cb)=0;
	void setReceiveCallback(YangReceiveCallback* cbk);
	static YangRtcHandle* createRtcHandle();
protected:
	YangReceiveCallback* m_recvcb;
	YangStreamConfig *m_conf;
};


#endif /* INCLUDE_YANGWEBRTC_YANGRTCHANDLE_H_ */
