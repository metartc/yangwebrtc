#ifndef SRC_YANGWEBRTC_YANGRTCVIDEORECVTRACK_H_
#define SRC_YANGWEBRTC_YANGRTCVIDEORECVTRACK_H_
#include <yangwebrtc/YangRecvTrack.h>

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
	int32_t m_hasReceiveStap;
	int64_t m_key_frame_ts;
    bool m_hasRequestKeyframe;


private:
	int32_t put_frame_video(char *p, int64_t timestamp, int32_t nb);
	int32_t put_frame_mixvideo(char *p, int64_t timestamp, int32_t nb);
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
#endif /* SRC_YANGWEBRTC_YANGRTCVIDEORECVTRACK_H_ */
