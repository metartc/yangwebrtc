
#ifndef YANGSTREAM_INCLUDE_YANGSTREAMBASE_H_
#define YANGSTREAM_INCLUDE_YANGSTREAMBASE_H_
#include <stdint.h>
#include <yangstream/YangAudioStreamFrame.h>
#include <string>
#include <yangstream/YangVideoStreamFrame.h>
#include "YangStreamType.h"
#include <yangutil/YangMediaConfigCallback.h>

class YangStreamHandle {
public:
	YangStreamHandle(int32_t puid);
	virtual ~YangStreamHandle();
	void init(YangStreamConfig* pconf);
	int32_t connectServer();
	int32_t disConnectServer();
	virtual int32_t reconnect()=0;
	virtual int32_t receiveData(int32_t *plen)=0;
	virtual int32_t publishVideoData(YangVideoStreamFrame* videoFrame)=0;
	virtual int32_t publishAudioData(YangAudioStreamFrame* audioFrame)=0;
	virtual int32_t getConnectState()=0;
	virtual int32_t isconnected()=0;
	virtual void setLocalMediaParam(YangAudioInfo* localAudioConf,YangVideoInfo* localVideoConf,YangRtcInfo* rtcInfo)=0;
	virtual void setRemoteMediaParamCallback(YangMediaConfigCallback* remoteCb)=0;
	virtual void setSendRequestCallback(YangSendRequestCallback *cb)=0;

	void setReceiveCallback(YangReceiveCallback *pdata);
	int32_t m_streamInit;
	int32_t m_uid;
	int32_t m_audioStream;
	int32_t m_videoStream;

protected:
	virtual int32_t connectMediaServer()=0;
	virtual int32_t disConnectMediaServer()=0;
	YangStreamConfig m_conf;
	YangReceiveCallback *m_data;
	int32_t m_netState;
};

#endif /* YANGSTREAM_INCLUDE_YANGSTREAMBASE_H_ */
