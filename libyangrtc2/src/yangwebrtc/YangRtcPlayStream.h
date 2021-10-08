#ifndef YANGWEBRTC_YANGRTCPLAYSTREAM_H_
#define YANGWEBRTC_YANGRTCPLAYSTREAM_H_
#include <string>
#include <string.h>
#include <stdint.h>
#include <yangutil/yangavtype.h>
#include <yangrtp/YangRtpCodec.h>
#include <yangrtp/YangRtcp.h>
#include <yangrtp/YangRtp.h>

#include <yangutil/buffer/YangBuffer.h>
#include <yangutil/buffer/YangVideoDecoderBuffer.h>
#include <yangutil/buffer/YangAudioEncoderBuffer.h>
#include <yangstream/YangStreamType.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpQueue.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangwebrtc/YangRtcContext.h>
using namespace std;
class YangRtcTrack;
class YangRtcSourceDescription;
class YangRecvTrack {
protected:
	YangRtcTrack *m_track_desc;
public:

	YangRtcSessionI *m_session;
	YangRtpRingBuffer *m_rtp_queue;
	YangRtpNackForReceiver *m_nack_receiver;

protected:
	// send report ntp and received time.
	int32_t m_uid;
	// Latest sender report ntp and rtp time.
	YangNtp m_last_sender_report_ntp;
	int64_t m_last_sender_report_rtp_time;

	// Prev sender report ntp and rtp time.
	YangNtp m_last_sender_report_ntp1;
	int64_t m_last_sender_report_rtp_time1;
	//YangNtp m_last_sender_report_ntp;
	uint64_t m_last_sender_report_sys_time;

	YangRtpBuffer *m_rtpBuffer;
	YangRtcContext *m_conf;
	YangFrame m_audioFrame;
	YangFrame m_videoFrame;
	bool m_usingMixav;
public:
	YangRecvTrack(int32_t uid, YangRtcContext *pcontext,
			YangRtcSessionI *session, YangRtcTrack *track_desc,
			YangRtpBuffer *rtpBuffer, YangMixQueue *pmixque, bool is_audio);
	virtual ~YangRecvTrack();

	void setReceiveCallback(YangReceiveCallback *cbk);
public:

	bool has_ssrc(uint32_t ssrc);
	uint32_t get_ssrc();
	void update_rtt(int32_t rtt);
	void update_send_report_time(const YangNtp &ntp, uint32_t rtp_time);
	int64_t cal_avsync_time(uint32_t rtp_time);
	int32_t send_rtcp_rr();
	int32_t send_rtcp_xr_rrtr();
	std::string get_track_id();
public:
	// Note that we can set the pkt to NULL to avoid copy, for example, if the NACK cache the pkt and
	// set to NULL, nack nerver copy it but set the pkt to NULL.
	int32_t on_nack(YangRtpPacket *ppkt);
public:
	virtual int32_t on_rtp(YangRtpPacket *pkt) = 0;
	virtual int32_t check_send_nacks() = 0;
protected:
	virtual int32_t do_check_send_nacks(uint32_t &timeout_nacks);
	void on_audio_data(YangMessage *msg);
	void on_video_data(YangMessage *msg);
	YangReceiveCallback *m_recvcb;
	YangMixQueue *m_mixQueue;
};

class YangRtcAudioRecvTrack: public YangRecvTrack,
		public IYangRtspPacketDecodeHandler {
public:
	YangRtcAudioRecvTrack(int32_t uid, YangRtcContext *conf,
			YangRtcSessionI *session, YangRtcTrack *track_desc,
			YangRtpBuffer *rtpBuffer, YangMixQueue *pmixque);
	virtual ~YangRtcAudioRecvTrack();

public:
	virtual void on_before_decode_payload(YangRtpPacket *pkt, YangBuffer *buf,
			IYangRtpPayloader **ppayload, YangRtspPacketPayloadType *ppt);
public:
	int32_t on_rtp(YangRtpPacket *pkt);
	int32_t check_send_nacks();
private:

};

class YangRtcVideoRecvTrack: public YangRecvTrack,
		public IYangRtspPacketDecodeHandler {
public:
	YangRtcVideoRecvTrack(int32_t uid, YangRtcContext *conf,
			YangRtcSessionI *session, YangRtcTrack *stream_descs,
			YangRtpBuffer *rtpBuffer, YangMixQueue *pmixque);
	virtual ~YangRtcVideoRecvTrack();

public:
	virtual void on_before_decode_payload(YangRtpPacket *pkt, YangBuffer *buf,
			IYangRtpPayloader **ppayload, YangRtspPacketPayloadType *ppt);
public:
	virtual int32_t on_rtp(YangRtpPacket *pkt);
	virtual int32_t check_send_nacks();
private:
	uint16_t m_header_sn;
	uint16_t m_lost_sn;
	int64_t m_key_frame_ts;
    bool m_hasRequestKeyframe;

private:
	int32_t put_frame_video(char *p, int64_t timestamp, int32_t nb);
	struct RtcPacketCache {
		bool m_in_use;
		uint16_t m_sn;
		uint32_t m_ts;
		YangRtpPacket *m_pkt;
	};
	const static uint16_t s_cache_size = 512;
	RtcPacketCache m_cache_video_pkts[s_cache_size];
	void clear_cached_video();
	inline uint16_t cache_index(uint16_t current_sn) {
		return current_sn % s_cache_size;
	}

	bool check_frame_complete(const uint16_t start, const uint16_t end);
	int32_t find_next_lost_sn(uint16_t current_sn, uint16_t &end_sn);
	int32_t packet_video(const uint16_t start, const uint16_t end);
	int32_t packet_video_key_frame(YangRtpPacket *pkt);
};

class YangRtcPlayStream: public IYangRtspPacketDecodeHandler {
public:
	YangRtcPlayStream(YangRtcSessionI *psession);
	virtual ~YangRtcPlayStream();

	int32_t initialize(YangRtcContext *conf,
			YangRtcSourceDescription *stream_desc, YangReceiveCallback *cbk,
			YangRtpBuffer *rtpBuffer);
	int32_t send_rtcp_rr();
	int32_t send_rtcp_xr_rrtr();
	int32_t check_send_nacks();
	int32_t send_periodic_twcc();
	void request_keyframe(uint32_t ssrc);

	virtual void on_before_decode_payload(YangRtpPacket *pkt, YangBuffer *buf,
			IYangRtpPayloader **ppayload, YangRtspPacketPayloadType *ppt);
	int32_t on_rtp(char *data, int32_t nb_data);
	int32_t on_rtp_plaintext(char *plaintext, int32_t nb_plaintext);
	int32_t do_on_rtp_plaintext(YangRtpPacket *pkt, YangBuffer *buf);
	int32_t on_rtcp(YangRtcpCommon *rtcp);
private:
	int32_t on_rtcp_sr(YangRtcpSR *rtcp);
	int32_t on_rtcp_xr(YangRtcpXr *rtcp);
	int32_t on_twcc(uint16_t sn);
	void update_rtt(uint32_t ssrc, int32_t rtt);
	void update_send_report_time(uint32_t ssrc, const YangNtp &ntp,
			uint32_t rtp_time);
	void simulate_nack_drop(int32_t nn);
	void simulate_drop_packet(YangRtpHeader *h, int32_t nn_bytes);

private:
	YangRtcContext *m_conf;
	YangRtcSessionI *m_session;
	uint16_t m_pt_to_drop;
	YangRtpBuffer *m_rtpBuffer;
	YangMixQueue m_mixQueue;

	bool m_twcc_enabled;
	int32_t m_twcc_id;
	int64_t m_last_time_send_twcc;
	uint8_t m_twcc_fb_count;
	YangRtcpTWCC m_rtcp_twcc;

	bool m_nack_enabled;

	YangRtpExtensionTypes m_extension_types;

	int32_t m_nn_simulate_nack_drop;
	bool m_request_keyframe;
	std::vector<YangRtcAudioRecvTrack*> m_audio_tracks;
	std::vector<YangRtcVideoRecvTrack*> m_video_tracks;
	YangRtcAudioRecvTrack* get_audio_track(uint32_t ssrc);
	YangRtcVideoRecvTrack* get_video_track(uint32_t ssrc);

};

#endif /* YANGWEBRTC_YANGRTCPLAYSTREAM_H_ */
