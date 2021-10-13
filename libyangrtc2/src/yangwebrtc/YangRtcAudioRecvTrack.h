#ifndef SRC_YANGWEBRTC_YANGRTCAUDIORECVTRACK_H_
#define SRC_YANGWEBRTC_YANGRTCAUDIORECVTRACK_H_
#include <yangwebrtc/YangRecvTrack.h>

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
	int32_t on_mixrtp(YangRtpPacket *pkt);
	YangRtpPacket *get_audiortp(YangRtpPacket *pkt);
	int32_t check_send_nacks();

	size_t m_audioCacheSize;
private:
	map<int64_t,YangRtpPacket*> m_audioMap;
	YangRtpBuffer *m_aduioBuffer;
};

#endif /* SRC_YANGWEBRTC_YANGRTCAUDIORECVTRACK_H_ */
