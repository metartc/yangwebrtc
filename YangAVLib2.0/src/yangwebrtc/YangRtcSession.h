
#ifndef YANGWEBRTC_YANGRTCSESSION_H_
#define YANGWEBRTC_YANGRTCSESSION_H_
#include <yangrtp/YangRtp.h>
#include <yangwebrtc/YangRtcUserConfig.h>
#include <yangutil/buffer/YangBuffer.h>
#include <yangutil/sys/YangSsl.h>
#include <yangutil/YangMediaConfigCallback.h>
#include <yangutil/sys/YangTimer.h>
#include <yangwebrtc/YangRtcDtls.h>

#include "YangRtcSdp.h"
#include "YangRtcPlayStream.h"

#include "YangRtcPublishStream.h"
class YangRtcSession :public YangTimerTask,public YangRtcSessionI{
public:
	YangRtcSession();
	virtual ~YangRtcSession();

	int32_t init(YangRtcUserConfig* pconf,YangSendUdpData *pudp,YangReceiveCallback *cbk,YangSendRequestCallback* reqcbk,YangStreamOptType role);
	void receive(char *data, int32_t nb_data);
	int32_t on_rtcp(char *data, int32_t nb_data);
	void startStunTimer();
	void setStunBuffer(char* p,int32_t plen);

	int32_t send_packet(YangRtpPacket *pkt);
	int32_t check_send_nacks(YangRtpNackForReceiver* nack, uint32_t ssrc, uint32_t& sent_nacks, uint32_t& timeout_nacks);
	int32_t send_rtcp_rr(uint32_t ssrc, YangRtpRingBuffer* rtp_queue, const uint64_t& last_send_systime, const YangNtp& last_send_ntp);
	int32_t send_rtcp_xr_rrtr(uint32_t ssrc);
	int32_t send_rtcp(char *data, int32_t nb_data);
	int32_t send_rtcp_fb_pli(uint32_t ssrc);
	void do_request_keyframe(uint32_t ssrc);
	YangSRtp* getSrtp();
	void doTask(int32_t taskId);

	int32_t publishVideo(YangVideoStreamFrame* videoFrame);
	int32_t publishAudio(YangAudioStreamFrame* audioFrame);

	void disconnect();
	int32_t send_video_meta(YangVideoStreamFrame* videoFrame);
	void setSendRequestCallback(YangSendRequestCallback *cb);
	void setSsrc(uint32_t audioSsrc,uint32_t videoSsrc);
public:

	YangRtcPlayStream* m_play;
	YangRtcPublishStream* m_publish;


private:
	YangRtcDtls *m_dtls;
	YangSRtp* m_srtp;
	YangSendUdpData *m_udp;
	YangRtcUserConfig *m_conf;

	YangRtpBuffer* m_rtpBuffer;
	//YangBuffer* cache_buffer_;
	//char* m_cache;
	//int32_t m_cacheLen;

	YangTimer* m_20ms;
	YangTimer* m_1s;
	YangTimer* m_100ms;
	YangRtpPacketUtil m_packet;
	YangSendRequestCallback* m_sendReq;

	char *m_stunBuffer;
	int32_t m_stunBufferLen;

	YangRtpExtensionTypes extension_types_;
	//int32_t nack_enabled_;
	int32_t isSendDtls;

	int32_t m_startRecv;
	int32_t twcc_id_;

	int32_t m_isSendStun;

	//char* m_audioPublishBuf;
	char* m_videoPublishBuf;

private:
	void startTimers();
	int32_t dispatch_rtcp(YangRtcpCommon* rtcp);
	int32_t on_rtcp_feedback_twcc(char* data, int32_t nb_data);
	int32_t on_rtcp_feedback_remb(YangRtcpPsfbCommon *rtcp);

	//std::vector<YangRtcTrack*> get_track_desc(std::string type, std::string media_name);
	int32_t send_avpacket(YangRtpPacket* pkt);
};

#endif /* YANGWEBRTC_YANGRTCSESSION_H_ */
