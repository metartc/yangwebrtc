
#ifndef YANGSTREAM_INCLUDE_YANGSTREAMBASE_H_
#define YANGSTREAM_INCLUDE_YANGSTREAMBASE_H_
#include <stdint.h>
#include <string>
#include <yangstream/YangAudioFrame.h>
#include <yangstream/YangVideoFrame.h>
#include "YangStreamType.h"
#include <yangutil/YangMediaConfigCallback.h>

class YangStreamHandle {
public:
	YangStreamHandle(int puid);
	virtual ~YangStreamHandle();
	void init(YangStreamConfig* pconf);
	int connectServer();
	int disConnectServer();
	virtual int reconnect()=0;
	virtual int receiveData(int *plen)=0;
	virtual int publishVideoData(YangVideoFrame* videoFrame)=0;
	virtual int publishAudioData(YangAudioFrame* audioFrame)=0;
	virtual int getConnectState()=0;
	virtual int isconnected()=0;
	virtual void setLocalMediaParam(YangAudioInfo* localAudioConf,YangVideoInfo* localVideoConf)=0;
	virtual void setRemoteMediaParamCallback(YangMediaConfigCallback* remoteCb)=0;
	virtual void setSendRequestCallback(YangSendRequestCallback *cb)=0;

	void setReceiveCallback(YangReceiveCallback *pdata);
	int m_streamInit;
	int m_uid;
	int m_audioStream;
	int m_videoStream;

protected:
	virtual int connectMediaServer()=0;
	virtual int disConnectMediaServer()=0;
	YangStreamConfig m_conf;
	YangReceiveCallback *m_data;
	int m_netState;
};

#endif /* YANGSTREAM_INCLUDE_YANGSTREAMBASE_H_ */
