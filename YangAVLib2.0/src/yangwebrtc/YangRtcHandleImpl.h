
#ifndef YANGRTP_YangRtcHandleImpl_H_
#define YANGRTP_YangRtcHandleImpl_H_

#include <string>
#include <string.h>
#include <yangwebrtc/YangRtcSession.h>
#include <yangwebrtc/YangSrsHttp.h>
#include <yangwebrtc/YangUdpHandle.h>
#include <yangwebrtc/YangRtcHandle.h>
#include <yangstream/YangStreamType.h>
using namespace std;
class YangRtcSourceDescription;
class YangRtcHandleImpl:public YangRtcHandle
{
public:
	YangRtcHandleImpl();
	~YangRtcHandleImpl();
public:
	void init(YangStreamConfig* pconf);
    void setMediaParam(YangAudioInfo* audioConf,YangVideoInfo* videoConf,YangRtcInfo* rtcInfo);
	int32_t connectRtcServer();
	int32_t disConnectServer();

	int32_t publishVideo(YangVideoStreamFrame* videoFrame);
	int32_t publishAudio(YangAudioStreamFrame* audioFrame);
	int32_t getState();
	void setRemoteMediaConfigCallback(YangMediaConfigCallback* cbk);
	void setSendRequestCallback(YangSendRequestCallback *cb);

private:
	YangSrsHttp* m_http;//url_publish;
	YangUdpHandle* m_udp;
	YangRtcSession *m_session;
	YangSdpHandle m_sdpHandle;

	YangRtcUserConfig m_conf;
	YangMediaConfigCallback* m_remoteMediaConf;
	YangSendRequestCallback* m_reqCb;
	int32_t m_isInit;


private:
	int32_t handlePlaySdp();
	int32_t createStunPacket(SrsSdpResponseType* prt,YangBuffer *stream,YangStreamOptType role);
	int32_t doHandleSignal(int32_t localport,YangSrsHttp* url,YangStreamOptType role);
};
#endif
