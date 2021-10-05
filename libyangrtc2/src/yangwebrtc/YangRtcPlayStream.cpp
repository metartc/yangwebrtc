#include <yangwebrtc/YangRtcPlayStream.h>

#include "YangRtcSdp.h"
#include <inttypes.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangLog.h>

YangRtcPlayStream::YangRtcPlayStream(YangRtcSessionI *psession) {
	m_session = psession;
	m_request_keyframe = false;
	m_nn_simulate_nack_drop = 0;
	m_nack_enabled = false;

	m_pt_to_drop = 0;
	m_twcc_enabled = false;
	m_twcc_id = 0;
	m_twcc_fb_count = 0;
	m_last_time_send_twcc = 0;
	m_conf = NULL;
	m_rtpBuffer = NULL;
}

YangRtcPlayStream::~YangRtcPlayStream() {
	m_session = NULL;
	m_conf = NULL;
	for (int32_t i = 0; i < (int) m_video_tracks.size(); ++i) {
		YangRtcVideoRecvTrack *track = m_video_tracks.at(i);
		yang_freep(track);
	}
	m_video_tracks.clear();

	for (int32_t i = 0; i < (int) m_audio_tracks.size(); ++i) {
		YangRtcAudioRecvTrack *track = m_audio_tracks.at(i);
		yang_freep(track);
	}
	m_audio_tracks.clear();

}

int32_t YangRtcPlayStream::initialize(YangRtcContext *conf,
		YangRtcSourceDescription *stream_desc, YangReceiveCallback *cbk,
		YangRtpBuffer *rtpBuffer) {
	int32_t err = Yang_Ok;
	m_conf = conf;
	m_rtpBuffer = rtpBuffer;
	m_mixQueue.setAudioSize(conf->context->rtc.audioQueueCount);
	m_mixQueue.setVideoSize(conf->context->rtc.videoQueueCount);
	if (stream_desc->audio_track_desc_) {
		m_audio_tracks.push_back(
				new YangRtcAudioRecvTrack(conf->streamConf->uid, conf,m_session,
						stream_desc->audio_track_desc_, rtpBuffer,&m_mixQueue));

	}

	for (int32_t i = 0; i < (int) stream_desc->video_track_descs_.size(); ++i) {
		YangRtcTrack *desc = stream_desc->video_track_descs_.at(i);
		m_video_tracks.push_back(
				new YangRtcVideoRecvTrack(conf->streamConf->uid, conf,m_session,
						desc, rtpBuffer,&m_mixQueue));
	}

	int32_t twcc_id = -1;
	uint32_t media_ssrc = 0;
	// because audio_track_desc have not twcc id, for example, h5demo
	// fetch twcc_id from video track description,
	for (int32_t i = 0; i < (int) stream_desc->video_track_descs_.size(); ++i) {
		YangRtcTrack *desc = stream_desc->video_track_descs_.at(i);
		twcc_id = desc->get_rtp_extension_id(kTWCCExt);

		media_ssrc = desc->ssrc_;
		break;
	}
	if (twcc_id > 0) {
		m_twcc_id = twcc_id;
		m_extension_types.register_by_uri(m_twcc_id, kTWCCExt);
		m_rtcp_twcc.set_media_ssrc(media_ssrc);
	}

	m_nack_enabled = true;
	m_pt_to_drop = 0;
	m_twcc_enabled = true;

	// No TWCC when negotiate, disable it.
	if (twcc_id <= 0) {
		m_twcc_enabled = false;
	}

	yang_trace("RTC publisher nack=%d,  pt-drop=%u, twcc=%u/%d", m_nack_enabled,
			m_pt_to_drop, m_twcc_enabled, twcc_id);

	// Setup tracks.
	for (int32_t i = 0; i < (int) m_audio_tracks.size(); i++) {
		YangRtcAudioRecvTrack *track = m_audio_tracks.at(i);
		track->setReceiveCallback(cbk);
	}

	for (int32_t i = 0; i < (int) m_video_tracks.size(); i++) {
		YangRtcVideoRecvTrack *track = m_video_tracks.at(i);
		track->setReceiveCallback(cbk);
	}
	return err;
}

int32_t YangRtcPlayStream::send_rtcp_rr() {
	int32_t err = Yang_Ok;

	for (int32_t i = 0; i < (int) m_video_tracks.size(); ++i) {
		YangRtcVideoRecvTrack *track = m_video_tracks.at(i);
		if ((err = track->send_rtcp_rr()) != Yang_Ok) {
			return yang_error_wrap(err, "track=%s",
					track->get_track_id().c_str());
		}
	}

	for (int32_t i = 0; i < (int) m_audio_tracks.size(); ++i) {
		YangRtcAudioRecvTrack *track = m_audio_tracks.at(i);
		if ((err = track->send_rtcp_rr()) != Yang_Ok) {
			return yang_error_wrap(err, "track=%s",
					track->get_track_id().c_str());
		}
	}

	return err;
}

int32_t YangRtcPlayStream::send_rtcp_xr_rrtr() {
	int32_t err = Yang_Ok;

	for (int32_t i = 0; i < (int) m_video_tracks.size(); ++i) {
		YangRtcVideoRecvTrack *track = m_video_tracks.at(i);
		if ((err = track->send_rtcp_xr_rrtr()) != Yang_Ok) {
			return yang_error_wrap(err, "track=%s",
					track->get_track_id().c_str());
		}
	}

	for (int32_t i = 0; i < (int) m_audio_tracks.size(); ++i) {
		YangRtcAudioRecvTrack *track = m_audio_tracks.at(i);
		if ((err = track->send_rtcp_xr_rrtr()) != Yang_Ok) {
			return yang_error_wrap(err, "track=%s",
					track->get_track_id().c_str());
		}
	}

	return err;
}

int32_t YangRtcPlayStream::on_twcc(uint16_t sn) {
	int32_t err = Yang_Ok;

	int64_t now = yang_get_system_time();
	err = m_rtcp_twcc.recv_packet(sn, now);

	return err;
}

int32_t YangRtcPlayStream::on_rtp(char *data, int32_t nb_data) {
	int32_t err = Yang_Ok;
/**
	if (m_twcc_id) {
	 // We must parse the TWCC from RTP header before SRTP unprotect, because:
	 //      1. Client may send some padding packets with invalid SequenceNumber, which causes the SRTP fail.
	 //      2. Server may send multiple duplicated NACK to client, and got more than one ARQ packet, which also fail SRTP.
	 // so, we must parse the header before SRTP unprotect(which may fail and drop packet).

		 uint16_t twcc_sn = 0;
		 if ((err = yang_rtp_fast_parse_twcc(data, nb_data, m_twcc_id, twcc_sn))== Yang_Ok) {
			 printf("twcc_%hu,",twcc_sn);
			 if((err = on_twcc(twcc_sn)) != Yang_Ok) {
			 return yang_error_wrap(err, "on twcc");
			 }
		 }
	}
**/

	char *plaintext = data;
	int32_t nb_plaintext = nb_data;
	if ((err = m_session->getSrtp()->dec_rtp(plaintext, &nb_plaintext))
			!= Yang_Ok) {
		// We try to decode the RTP header for more detail error informations.
		YangBuffer b(data, nb_data);
		YangRtpHeader h;
		h.ignore_padding(true);
		h.decode(&b);
		err = yang_error_wrap(err,
				"marker=%u, pt=%u, seq=%u, ts=%u, ssrc=%u, pad=%u, payload=%uB",
				h.get_marker(), h.get_payload_type(), h.get_sequence(),
				h.get_timestamp(), h.get_ssrc(), h.get_padding(),
				nb_data - b.pos());

		return err;
	}

	// Handle the plaintext RTP packet.
	if ((err = on_rtp_plaintext(plaintext, nb_plaintext)) != Yang_Ok) {
		// We try to decode the RTP header for more detail error informations.
		//YangBuffer b(data, nb_data);
		//YangRtpHeader h;
		//h.ignore_padding(true);
		//int32_t r0 =
		//	h.decode(&b);
		//int32_t nb_header = h.nb_bytes();
		return yang_error_wrap(err, "cipher=%u, plaintext=%u", nb_data,
				nb_plaintext);
	}

	return err;
}

int32_t YangRtcPlayStream::on_rtp_plaintext(char *plaintext,
		int32_t nb_plaintext) {
	int32_t err = Yang_Ok;
	// Allocate packet form cache.
	YangRtpPacket *pkt = new YangRtpPacket();

	YangBuffer buf(pkt->wrap(m_rtpBuffer, plaintext, nb_plaintext),
			nb_plaintext);
	// @remark Note that the pkt might be set to NULL.
	err = do_on_rtp_plaintext(pkt, &buf);
	// Free the packet.
	// @remark Note that the pkt might be set to NULL.
	yang_freep(pkt);
	return err;
}

int32_t YangRtcPlayStream::do_on_rtp_plaintext(YangRtpPacket *pkt,
		YangBuffer *buf) {
	int32_t err = Yang_Ok;

	pkt->set_decode_handler(this);
	pkt->set_extension_types(&m_extension_types);
	pkt->m_header.ignore_padding(false);

	if ((err = pkt->decode(buf)) != Yang_Ok) {
		return yang_error_wrap(err, "decode rtp packet");
	}


	uint32_t ssrc = pkt->m_header.get_ssrc();
	YangRtcAudioRecvTrack *audio_track = get_audio_track(ssrc);
	YangRtcVideoRecvTrack *video_track = get_video_track(ssrc);

	if (audio_track) {
		pkt->m_frame_type = SrsFrameTypeAudio;
		if ((err = audio_track->on_rtp(pkt)) != Yang_Ok) {
			return yang_error_wrap(err, "on audio");
		}
	} else if (video_track) {
		pkt->m_frame_type = SrsFrameTypeVideo;
		if ((err = video_track->on_rtp(pkt)) != Yang_Ok) {
			return yang_error_wrap(err, "on video");
		}
	} else {
		return yang_error_wrap(ERROR_RTC_RTP, "unknown ssrc=%u", ssrc);
	}

	// For NACK to handle packet.
	// @remark Note that the pkt might be set to NULL.
	if (m_nack_enabled) {
		if (audio_track) {
			if ((err = audio_track->on_nack(pkt)) != Yang_Ok) {
				return yang_error_wrap(err, "on nack");
			}
		} else if (video_track) {
			if ((err = video_track->on_nack(pkt)) != Yang_Ok) {
				return yang_error_wrap(err, "on nack");
			}
		}
	}

	return err;
}

int32_t YangRtcPlayStream::check_send_nacks() {
	int32_t err = Yang_Ok;

	if (!m_nack_enabled) {
		return err;
	}

	for (int32_t i = 0; i < (int) m_video_tracks.size(); ++i) {
		YangRtcVideoRecvTrack *track = m_video_tracks.at(i);
		if ((err = track->check_send_nacks()) != Yang_Ok) {
			return yang_error_wrap(err, "video track=%s",
					track->get_track_id().c_str());
		}
	}

	for (int32_t i = 0; i < (int) m_audio_tracks.size(); ++i) {
		YangRtcAudioRecvTrack *track = m_audio_tracks.at(i);
		if ((err = track->check_send_nacks()) != Yang_Ok) {
			return yang_error_wrap(err, "audio track=%s",
					track->get_track_id().c_str());
		}
	}

	return err;

}
YangRtcVideoRecvTrack* YangRtcPlayStream::get_video_track(uint32_t ssrc) {
	for (int32_t i = 0; i < (int) m_video_tracks.size(); ++i) {
		YangRtcVideoRecvTrack *track = m_video_tracks.at(i);
		if (track->has_ssrc(ssrc)) {
			return track;
		}
	}

	return NULL;
}

YangRtcAudioRecvTrack* YangRtcPlayStream::get_audio_track(uint32_t ssrc) {
	for (int32_t i = 0; i < (int) m_audio_tracks.size(); ++i) {
		YangRtcAudioRecvTrack *track = m_audio_tracks.at(i);
		if (track->has_ssrc(ssrc)) {
			return track;
		}
	}

	return NULL;
}
void YangRtcPlayStream::on_before_decode_payload(YangRtpPacket *pkt,
		YangBuffer *buf, IYangRtpPayloader **ppayload,
		YangRtspPacketPayloadType *ppt) {
	// No payload, ignore.
	if (buf->empty()) {
		return;
	}

	uint32_t ssrc = pkt->m_header.get_ssrc();
	YangRtcAudioRecvTrack *audio_track = get_audio_track(ssrc);
	YangRtcVideoRecvTrack *video_track = get_video_track(ssrc);

	if (audio_track) {
		audio_track->on_before_decode_payload(pkt, buf, ppayload, ppt);
	} else if (video_track) {
		video_track->on_before_decode_payload(pkt, buf, ppayload, ppt);
	}

}

int32_t YangRtcPlayStream::send_periodic_twcc() {
	int32_t err = Yang_Ok;
	m_last_time_send_twcc = yang_get_system_time();

	if (!m_rtcp_twcc.need_feedback()) {
		return err;
	}

	// limit the max count=1024 to avoid dead loop.
	for (int32_t i = 0; i < 1024 && m_rtcp_twcc.need_feedback(); ++i) {
		char pkt[kMaxUDPDataSize];
		YangBuffer *buffer = new YangBuffer(pkt, sizeof(pkt));
		YangAutoFree(YangBuffer, buffer);

		m_rtcp_twcc.set_feedback_count(m_twcc_fb_count);
		m_twcc_fb_count++;

		if ((err = m_rtcp_twcc.encode(buffer)) != Yang_Ok) {
			return yang_error_wrap(err, "encode, count=%u", m_twcc_fb_count);
		}

		if ((err = m_session->send_rtcp(pkt, buffer->pos())) != Yang_Ok) {
			return yang_error_wrap(err, "send twcc, count=%u", m_twcc_fb_count);
		}
	}
	return err;
}

int32_t YangRtcPlayStream::on_rtcp(YangRtcpCommon *rtcp) {
	if (YangRtcpType_sr == rtcp->type()) {
		YangRtcpSR *sr = dynamic_cast<YangRtcpSR*>(rtcp);
		return on_rtcp_sr(sr);
	} else if (YangRtcpType_xr == rtcp->type()) {
		YangRtcpXr *xr = dynamic_cast<YangRtcpXr*>(rtcp);
		return on_rtcp_xr(xr);
	} else if (YangRtcpType_sdes == rtcp->type()) {
		//ignore RTCP SDES
		return Yang_Ok;
	} else if (YangRtcpType_bye == rtcp->type()) {
		// TODO: FIXME: process rtcp bye.
		return Yang_Ok;
	} else {
		return yang_error_wrap(ERROR_RTC_RTCP_CHECK, "unknown rtcp type=%u",
				rtcp->type());
	}
}

int32_t YangRtcPlayStream::on_rtcp_sr(YangRtcpSR *rtcp) {
	int32_t err = Yang_Ok;
	YangNtp srs_ntp = YangNtp::to_time_ms(rtcp->get_ntp());

	//  yang_debug("sender report, ssrc_of_sender=%u, rtp_time=%u, sender_packet_count=%u, sender_octec_count=%u",
	//  rtcp->get_ssrc(), rtcp->get_rtp_ts(), rtcp->get_rtp_send_packets(), rtcp->get_rtp_send_bytes());

	update_send_report_time(rtcp->get_ssrc(), srs_ntp);

	return err;
}

int32_t YangRtcPlayStream::on_rtcp_xr(YangRtcpXr *rtcp) {
	int32_t err = Yang_Ok;

	/*
	 @see: http://www.rfc-editor.org/rfc/rfc3611.html#section-2

	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |V=2|P|reserved |   PT=XR=207   |             length            |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                              SSRC                             |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 :                         report blocks                         :
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 */

	YangBuffer stream(rtcp->data(), rtcp->size());
	/*uint8_t first = */stream.read_1bytes();
	uint8_t pt = stream.read_1bytes();
	if (pt != kXR)
		return yang_error_wrap(ERROR_RTC_RTCP_CHECK, "invalid XR packet");
	//srs_assert(pt == kXR);
	uint16_t length = (stream.read_2bytes() + 1) * 4;
	/*uint32_t ssrc = */stream.read_4bytes();

	if (length > rtcp->size()) {
		return yang_error_wrap(ERROR_RTC_RTCP_CHECK,
				"invalid XR packet, length=%u, nb_buf=%d", length, rtcp->size());
	}

	while (stream.pos() + 4 < length) {
		uint8_t bt = stream.read_1bytes();
		stream.skip(1);
		uint16_t block_length = (stream.read_2bytes() + 1) * 4;

		if (stream.pos() + block_length - 4 > rtcp->size()) {
			return yang_error_wrap(ERROR_RTC_RTCP_CHECK,
					"invalid XR packet block, block_length=%u, nb_buf=%d",
					block_length, rtcp->size());
		}

		if (bt == 5) {
			for (int32_t i = 4; i < block_length; i += 12) {
				uint32_t ssrc = stream.read_4bytes();
				uint32_t lrr = stream.read_4bytes();
				uint32_t dlrr = stream.read_4bytes();

				YangNtp cur_ntp = YangNtp::from_time_ms(
				yang_update_system_time() / 1000);
				uint32_t compact_ntp = (cur_ntp.ntp_second_ << 16)
						| (cur_ntp.ntp_fractions_ >> 16);

				int32_t rtt_ntp = compact_ntp - lrr - dlrr;
				int32_t rtt = ((rtt_ntp * 1000) >> 16)
						+ ((rtt_ntp >> 16) * 1000);
				yang_trace("ssrc=%u, compact_ntp=%u, lrr=%u, dlrr=%u, rtt=%d",
						ssrc, compact_ntp, lrr, dlrr, rtt);

				update_rtt(ssrc, rtt);
			}
		}
	}

	return err;
}

void YangRtcPlayStream::request_keyframe(uint32_t ssrc) {

}

void YangRtcPlayStream::simulate_nack_drop(int32_t nn) {
	m_nn_simulate_nack_drop = nn;
}

void YangRtcPlayStream::simulate_drop_packet(YangRtpHeader *h,
		int32_t nn_bytes) {


	m_nn_simulate_nack_drop--;
}

void YangRtcPlayStream::update_rtt(uint32_t ssrc, int32_t rtt) {
	YangRtcVideoRecvTrack *video_track = get_video_track(ssrc);
	if (video_track) {
		return video_track->update_rtt(rtt);
	}

	YangRtcAudioRecvTrack *audio_track = get_audio_track(ssrc);
	if (audio_track) {
		return audio_track->update_rtt(rtt);
	}
}

void YangRtcPlayStream::update_send_report_time(uint32_t ssrc,
		const YangNtp &ntp) {
	YangRtcVideoRecvTrack *video_track = get_video_track(ssrc);
	if (video_track) {
		return video_track->update_send_report_time(ntp);
	}

	YangRtcAudioRecvTrack *audio_track = get_audio_track(ssrc);
	if (audio_track) {
		return audio_track->update_send_report_time(ntp);
	}
}

YangRecvTrack::YangRecvTrack(int32_t uid, YangRtcContext* conf,YangRtcSessionI *session,
		YangRtcTrack *ptrack_desc, YangRtpBuffer *rtpBuffer,
		YangMixQueue *pmixque, bool is_audio) {
	m_uid = uid;
	m_session = session;
	m_track_desc = ptrack_desc->copy();
	m_rtpBuffer = rtpBuffer;

	if (is_audio) {
		m_rtp_queue = new YangRtpRingBuffer(100);
		m_nack_receiver = new YangRtpNackForReceiver(m_rtp_queue, 100 * 2 / 3);
	} else {
		m_rtp_queue = new YangRtpRingBuffer(1000);
		m_nack_receiver = new YangRtpNackForReceiver(m_rtp_queue, 1000 * 2 / 3);
	}

	m_last_sender_report_sys_time = 0;
	m_recvcb = NULL;
	m_mixQueue = pmixque;
	m_conf=conf;
	m_usingMixav=m_conf->context->rtc.mixAvqueue;
}

YangRecvTrack::~YangRecvTrack() {
	yang_freep(m_rtp_queue);
	yang_freep(m_nack_receiver);
	yang_freep(m_track_desc);
	m_recvcb = NULL;
}
void YangRecvTrack::setReceiveCallback(YangReceiveCallback *cbk) {
	m_recvcb = cbk;
}
bool YangRecvTrack::has_ssrc(uint32_t ssrc) {
	return m_track_desc->has_ssrc(ssrc);
}

uint32_t YangRecvTrack::get_ssrc() {
	return m_track_desc->ssrc_;
}

void YangRecvTrack::update_rtt(int32_t rtt) {
	m_nack_receiver->update_rtt(rtt);
}

void YangRecvTrack::update_send_report_time(const YangNtp &ntp) {
	m_last_sender_report_ntp = ntp;
	// TODO: FIXME: Use system wall clock.
	m_last_sender_report_sys_time = yang_update_system_time();
}

int32_t YangRecvTrack::send_rtcp_rr() {
	int32_t err = Yang_Ok;

	uint32_t ssrc = m_track_desc->ssrc_;
	const uint64_t &last_time = m_last_sender_report_sys_time;
	if ((err = m_session->send_rtcp_rr(ssrc, m_rtp_queue, last_time,
			m_last_sender_report_ntp)) != Yang_Ok) {
		return yang_error_wrap(err, "ssrc=%u, last_time=%lld", ssrc, last_time);
	}

	return err;
}

int32_t YangRecvTrack::send_rtcp_xr_rrtr() {
	int32_t err = Yang_Ok;

	if ((err = m_session->send_rtcp_xr_rrtr(m_track_desc->ssrc_)) != Yang_Ok) {
		return yang_error_wrap(err, "ssrc=%u", m_track_desc->ssrc_);
	}

	return err;
}

std::string YangRecvTrack::get_track_id() {
	return m_track_desc->id_;
}

int32_t YangRecvTrack::on_nack(YangRtpPacket *pkt) {
	int32_t err = Yang_Ok;

	uint16_t seq = pkt->m_header.get_sequence();
	YangRtpNackInfo *nack_info = m_nack_receiver->find(seq);
	if (nack_info) {
		// seq had been received.
		m_nack_receiver->remove(seq);
		return err;
	}

	// insert check nack list
	uint16_t nack_first = 0, nack_last = 0;
	if (!m_rtp_queue->update(seq, nack_first, nack_last)) {
		yang_info("\nNACK: too old seq %u, range [%u, %u]", seq,
				m_rtp_queue->m_begin, m_rtp_queue->m_end);
	}
	if (yang_rtp_seq_distance(nack_first, nack_last) > 0) {
		yang_trace("\nNACK: update seq=%u, nack range [%u, %u]", seq, nack_first,
				nack_last);
		m_nack_receiver->insert(nack_first, nack_last);
		m_nack_receiver->check_queue_size();
	}

	// insert into video_queue and audio_queue
	m_rtp_queue->set(seq, pkt->copy());
	return err;
}

int32_t YangRecvTrack::do_check_send_nacks(uint32_t &timeout_nacks) {
	int32_t err = Yang_Ok;

	uint32_t sent_nacks = 0;
	m_session->check_send_nacks(m_nack_receiver, m_track_desc->ssrc_,
			sent_nacks, timeout_nacks);

	return err;
}
void YangRecvTrack::on_audio_data(YangMessage *msg) {
	m_audioFrame.uid = m_uid;
    m_audioFrame.payload =(uint8_t*) msg->payload;
	m_audioFrame.nb = msg->nb;
	m_audioFrame.timestamp = msg->timestamp;
	if (m_recvcb)
		m_recvcb->receiveAudio(&m_audioFrame);
	//printf("a%ld,",m_audioFrame.timestamp);
	yang_delete(msg);
}
void YangRecvTrack::on_video_data(YangMessage *msg) {
	m_videoFrame.uid = m_uid;
    m_videoFrame.payload = (uint8_t*) msg->payload;
	m_videoFrame.nb = msg->nb;
	m_videoFrame.timestamp = msg->timestamp;
	if (m_recvcb)
		m_recvcb->receiveVideo(&m_videoFrame);
	yang_deleteA(msg->payload);
	if(msg) delete msg;
}
YangRtcAudioRecvTrack::YangRtcAudioRecvTrack(int32_t uid,YangRtcContext* conf,
		YangRtcSessionI *session, YangRtcTrack *track_desc,
		YangRtpBuffer *rtpBuffer, YangMixQueue *pmixque) :
		YangRecvTrack(uid, conf,session, track_desc, rtpBuffer, pmixque, true) {
}

YangRtcAudioRecvTrack::~YangRtcAudioRecvTrack() {
}

void YangRtcAudioRecvTrack::on_before_decode_payload(YangRtpPacket *pkt,
		YangBuffer *buf, IYangRtpPayloader **ppayload,
		YangRtspPacketPayloadType *ppt) {
	// No payload, ignore.
	if (buf->empty()) {
		return;
	}

	*ppayload = new YangRtpRawPayload();
	*ppt = YangRtspPacketPayloadTypeRaw;
}

int32_t YangRtcAudioRecvTrack::on_rtp(YangRtpPacket *pkt) {
	int32_t err = Yang_Ok;
	YangRtpRawPayload *payload =
			dynamic_cast<YangRtpRawPayload*>(pkt->payload());
	if(m_usingMixav){
		YangMessage *prt = new YangMessage();

		prt->mediaType = 0;
		prt->nb = payload->m_nn_payload;
		prt->timestamp = pkt->m_header.get_timestamp();
		prt->payload =  payload->m_payload;
		m_mixQueue->push(prt);
		YangMessage *msg = m_mixQueue->pop();
		if (msg == NULL)
			return Yang_Ok;
		if (msg->mediaType) {
			 on_video_data(msg);

		} else {
			 on_audio_data(msg);
		}
	}else{
		m_audioFrame.uid = m_uid;
		m_audioFrame.payload = (uint8_t*) payload->m_payload;
		m_audioFrame.nb =  payload->m_nn_payload;
		m_audioFrame.timestamp = pkt->m_header.get_timestamp();
		if (m_recvcb)			m_recvcb->receiveAudio(&m_audioFrame);
	}
	return err;
}

int32_t YangRtcAudioRecvTrack::check_send_nacks() {
	int32_t err = Yang_Ok;

	uint32_t timeout_nacks = 0;
	if ((err = do_check_send_nacks(timeout_nacks)) != Yang_Ok) {
		return yang_error_wrap(err, "audio");
	}

	return err;
}

YangRtcVideoRecvTrack::YangRtcVideoRecvTrack(int32_t uid,YangRtcContext* conf,
		YangRtcSessionI *session, YangRtcTrack *track_desc,
		YangRtpBuffer *rtpBuffer, YangMixQueue *pmixque) :
		YangRecvTrack(uid, conf,session, track_desc, rtpBuffer, pmixque, false) {
	m_key_frame_ts = -1;
	m_lost_sn = 0;
	m_header_sn = 0;
	m_hasRequestKeyframe = 0;
	memset(m_cache_video_pkts, 0, sizeof(m_cache_video_pkts));
}

YangRtcVideoRecvTrack::~YangRtcVideoRecvTrack() {

}

void YangRtcVideoRecvTrack::on_before_decode_payload(YangRtpPacket *pkt,
		YangBuffer *buf, IYangRtpPayloader **ppayload,
		YangRtspPacketPayloadType *ppt) {
	// No payload, ignore.
	if (buf->empty()) {
		return;
	}

	uint8_t v = (uint8_t) (buf->head()[0] & kNalTypeMask);
	pkt->m_nalu_type = YangAvcNaluType(v);

	if (v == kStapA) {
		*ppayload = new YangRtpSTAPPayload();
		*ppt = YangRtspPacketPayloadTypeSTAP;
	} else if (v == kFuA) {
		*ppayload = new YangRtpFUAPayload2();
		*ppt = YangRtspPacketPayloadTypeFUA2;
	} else {
		*ppayload = new YangRtpRawPayload();
		*ppt = YangRtspPacketPayloadTypeRaw;
	}
}
int32_t YangRtcVideoRecvTrack::packet_video(const uint16_t start,
		const uint16_t end) {
	int32_t err = Yang_Ok;
	int32_t nb_payload = 0;
	uint16_t cnt = end - start + 1;

	for (uint16_t i = 0; i < cnt; ++i) {
		uint16_t sn = start + i;
		uint16_t index = cache_index(sn);
		YangRtpPacket *pkt = m_cache_video_pkts[index].m_pkt;
		// calculate nalu len
		YangRtpFUAPayload2 *fua_payload =
				dynamic_cast<YangRtpFUAPayload2*>(pkt->payload());
		if (fua_payload && fua_payload->m_size > 0) {
			if (fua_payload->m_start) {
				nb_payload += 1 + 4;
			}
			nb_payload += fua_payload->m_size;
			continue;
		}

		YangRtpSTAPPayload *stap_payload =
				dynamic_cast<YangRtpSTAPPayload*>(pkt->payload());
		if (stap_payload) {
			for (int32_t j = 0; j < (int) stap_payload->m_nalus.size(); ++j) {
				YangSample *sample = stap_payload->m_nalus.at(j);
				if (sample->m_size > 0) {
					nb_payload += 4 + sample->m_size;

				}
			}
			continue;
		}

		YangRtpRawPayload *raw_payload =
				dynamic_cast<YangRtpRawPayload*>(pkt->payload());
		if (raw_payload && raw_payload->m_nn_payload > 0) {
			nb_payload += 4 + raw_payload->m_nn_payload;
			continue;
		}
	}

	if (0 == nb_payload) {
		yang_error("empty nalu");
		return err;
	}

	//type_codec1 + avc_type + composition time + nalu size + nalu
	nb_payload += 1 + 1 + 3;

	char *buf = new char[nb_payload];
	int32_t bufLen = nb_payload;
	int64_t timestamp = 0;


	YangRtpPacket *header = m_cache_video_pkts[cache_index(start)].m_pkt;
	YangBuffer payload(buf, bufLen);



	if (header->is_keyframe()) {
		payload.write_1bytes(0x17); // type(4 bits): key frame; code(4bits): avc
		m_key_frame_ts = -1;
	} else {
		payload.write_1bytes(0x27); // type(4 bits): inter frame; code(4bits): avc
	}
	payload.write_1bytes(0x01); // avc_type: nalu
	payload.write_1bytes(0x0);  // composition time
	payload.write_1bytes(0x0);
	payload.write_1bytes(0x0);

	int32_t nalu_len = 0;
	for (uint16_t i = 0; i < cnt; ++i) {
		uint16_t index = cache_index((start + i));
		YangRtpPacket *pkt = m_cache_video_pkts[index].m_pkt;
		m_cache_video_pkts[index].m_in_use = false;
		m_cache_video_pkts[index].m_pkt = NULL;
		m_cache_video_pkts[index].m_ts = 0;
		m_cache_video_pkts[index].m_sn = 0;

		YangRtpFUAPayload2 *fua_payload =
				dynamic_cast<YangRtpFUAPayload2*>(pkt->payload());
		if (fua_payload && fua_payload->m_size > 0) {
			if (fua_payload->m_start) {
				nalu_len = fua_payload->m_size + 1;
				//skip 4 bytes to write nalu_len future
				payload.skip(4);
				payload.write_1bytes(
						fua_payload->m_nri | fua_payload->m_nalu_type);
				payload.write_bytes(fua_payload->m_payload,
						fua_payload->m_size);
				timestamp = pkt->m_header.get_timestamp();

			} else {
				nalu_len += fua_payload->m_size;
				payload.write_bytes(fua_payload->m_payload,
						fua_payload->m_size);

				if (fua_payload->m_end) {
					//write nalu_len back
					payload.skip(-(4 + nalu_len));
					payload.write_4bytes(nalu_len);
					payload.skip(nalu_len);
				}
			}
			yang_freep(pkt);
			continue;
		}

		YangRtpSTAPPayload *stap_payload =
				dynamic_cast<YangRtpSTAPPayload*>(pkt->payload());
		if (stap_payload) {
			for (int32_t j = 0; j < (int) stap_payload->m_nalus.size(); ++j) {
				YangSample *sample = stap_payload->m_nalus.at(j);
				if (sample->m_size > 0) {

					payload.write_4bytes(sample->m_size);
					payload.write_bytes(sample->m_bytes, sample->m_size);

				}
			}
			yang_freep(pkt);
			continue;
		}

		YangRtpRawPayload *raw_payload =
				dynamic_cast<YangRtpRawPayload*>(pkt->payload());
		if (raw_payload && raw_payload->m_nn_payload > 0) {
			payload.write_4bytes(raw_payload->m_nn_payload);
			payload.write_bytes(raw_payload->m_payload,
					raw_payload->m_nn_payload);

			yang_freep(pkt);
			continue;
		}

		yang_freep(pkt);
	}

	if ((err = put_frame_video(buf, timestamp, bufLen)) != Yang_Ok) {
		   yang_warn("fail to pack video frame");
	}

	m_header_sn = end + 1;
	uint16_t tail_sn = 0;
	int32_t sn = find_next_lost_sn(m_header_sn, tail_sn);
	if (-1 == sn) {
		if (check_frame_complete(m_header_sn, tail_sn)) {
			err = packet_video(m_header_sn, tail_sn);
		}
	} else if (-2 == sn) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "video cache is overflow");
	} else {
		m_lost_sn = sn;
	}

	return err;
}

int32_t YangRtcVideoRecvTrack::packet_video_key_frame(YangRtpPacket *pkt) {
	int32_t err = Yang_Ok;
	YangRtpSTAPPayload *stap_payload =
			dynamic_cast<YangRtpSTAPPayload*>(pkt->payload());
	if (stap_payload) {
		YangSample *sps = stap_payload->get_sps();
		YangSample *pps = stap_payload->get_pps();
		if (NULL == sps || NULL == pps) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER,
					"no sps or pps in stap-a rtp. sps: %p, pps:%p", sps, pps);
		} else {
			//type_codec1 + avc_type + composition time + fix header + count of sps + len of sps + sps + count of pps + len of pps + pps
			// int32_t nb_payload = 1 + 1 + 3 + 5 + 1 + 2 + sps->size + 1 + 2 + pps->size;
			char *spss = new char[1024];
			YangBuffer payload(spss, 1024);
			//TODO: call api
			payload.write_1bytes(0x17); // type(4 bits): key frame; code(4bits): avc
			payload.write_1bytes(0x0); // avc_type: sequence header
			payload.write_1bytes(0x0); // composition time
			payload.write_1bytes(0x0);
			payload.write_1bytes(0x0);
			payload.write_1bytes(0x01); // version
			payload.write_1bytes(sps->m_bytes[1]);
			payload.write_1bytes(sps->m_bytes[2]);
			payload.write_1bytes(sps->m_bytes[3]);
			payload.write_1bytes(0xff);
			payload.write_1bytes(0xe1);
			payload.write_2bytes(sps->m_size);
			payload.write_bytes(sps->m_bytes, sps->m_size);
			payload.write_1bytes(0x01);
			payload.write_2bytes(pps->m_size);
			payload.write_bytes(pps->m_bytes, pps->m_size);

			if ((err = put_frame_video(payload.data(), 0, payload.pos()))
					!= Yang_Ok) {
				return err;
			}
		}
	}
	if (-1 == m_key_frame_ts) {
		m_key_frame_ts = pkt->m_header.get_timestamp();
		m_header_sn = pkt->m_header.get_sequence();
		m_lost_sn = m_header_sn + 1;
		// Received key frame and clean cache of old p frame pkts
		clear_cached_video();
		//yang_trace("set ts=%lld, header=%hu, lost=%hu", key_frame_ts_, header_sn_, lost_sn_);
	} else if (m_key_frame_ts != pkt->m_header.get_timestamp()) {
		//new key frame, clean cache
		//int64_t old_ts = key_frame_ts_;
		//uint16_t old_header_sn = header_sn_;
		//uint16_t old_lost_sn = lost_sn_;
		m_key_frame_ts = pkt->m_header.get_timestamp();
		m_header_sn = pkt->m_header.get_sequence();
		m_lost_sn = m_header_sn + 1;
		clear_cached_video();
		// yang_trace("drop old ts=%lld, header=%hu, lost=%hu, set new ts=%lld, header=%hu, lost=%hu",
		//   old_ts, old_header_sn, old_lost_sn, key_frame_ts_, header_sn_, lost_sn_);
	}

	uint16_t index = cache_index(pkt->m_header.get_sequence());
	m_cache_video_pkts[index].m_in_use = true;
	m_cache_video_pkts[index].m_pkt = pkt;
	m_cache_video_pkts[index].m_sn = pkt->m_header.get_sequence();
	m_cache_video_pkts[index].m_ts = pkt->m_header.get_timestamp();

	int32_t sn = m_lost_sn;
	uint16_t tail_sn = 0;
	if (yang_rtp_seq_distance(m_header_sn, pkt->m_header.get_sequence()) < 0) {
		// When receive previous pkt in the same frame, update header sn;
		m_header_sn = pkt->m_header.get_sequence();
		sn = find_next_lost_sn(m_header_sn, tail_sn);
	} else if (m_lost_sn == pkt->m_header.get_sequence()) {
		sn = find_next_lost_sn(m_lost_sn, tail_sn);
	}

	if (-1 == sn) {
		if (check_frame_complete(m_header_sn, tail_sn)) {
			if ((err = packet_video(m_header_sn, tail_sn)) != Yang_Ok) {
				err = yang_error_wrap(err, "fail to packet key frame");
			}
		}
	} else if (-2 == sn) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "video cache is overflow");
	} else {
		m_lost_sn = (uint16_t) sn;
	}

	return err;
}

void YangRtcVideoRecvTrack::clear_cached_video() {
	for (size_t i = 0; i < s_cache_size; i++) {
		if (m_cache_video_pkts[i].m_in_use) {
			yang_freep(m_cache_video_pkts[i].m_pkt);
			m_cache_video_pkts[i].m_sn = 0;
			m_cache_video_pkts[i].m_ts = 0;
			m_cache_video_pkts[i].m_in_use = false;
		}
	}
}
int32_t YangRtcVideoRecvTrack::put_frame_video(char *p, int64_t timestamp,
		int32_t nb) {
	//printf("p%d-%p,",nb,m_recvcb);
	//printf("\n");
	//for(int32_t i=0;i<50;i++) printf("%02x,",(uint8_t)p[i]);
	if(m_usingMixav){
		YangMessage *prt = new YangMessage();
		prt->mediaType = 1;
		prt->nb = nb;
		prt->timestamp = timestamp;
		prt->payload = p;
		m_mixQueue->push(prt);
		YangMessage *msg = m_mixQueue->pop();
		if (msg == NULL)
			return Yang_Ok;
		if (msg->mediaType) {
			on_video_data(msg);

		} else {
			on_audio_data(msg);
		}
	}else{
		m_videoFrame.uid = m_uid;
        m_videoFrame.payload = (uint8_t*)p;
		m_videoFrame.nb = nb;
		m_videoFrame.timestamp = timestamp;
			if (m_recvcb)
				m_recvcb->receiveVideo(&m_videoFrame);
            yang_deleteA(p);
	}

	return Yang_Ok;
}

bool YangRtcVideoRecvTrack::check_frame_complete(const uint16_t start,
		const uint16_t end) {
	uint16_t cnt = (end - start + 1);
	uint16_t fu_s_c = 0;
	uint16_t fu_e_c = 0;
	for (uint16_t i = 0; i < cnt; ++i) {
		int32_t index = cache_index((start + i));
		YangRtpPacket *pkt = m_cache_video_pkts[index].m_pkt;
		YangRtpFUAPayload2 *fua_payload =
				dynamic_cast<YangRtpFUAPayload2*>(pkt->payload());
		if (fua_payload) {
			if (fua_payload->m_start) {
				++fu_s_c;
			}

			if (fua_payload->m_end) {
				++fu_e_c;
			}
		}
	}

	return fu_s_c == fu_e_c;
}
int32_t YangRtcVideoRecvTrack::find_next_lost_sn(uint16_t current_sn,
		uint16_t &end_sn) {
	uint32_t last_ts = m_cache_video_pkts[cache_index(m_header_sn)].m_ts;
	for (int32_t i = 0; i < s_cache_size; ++i) {
		uint16_t lost_sn = current_sn + i;
		int32_t index = cache_index(lost_sn);

		if (!m_cache_video_pkts[index].m_in_use) {
			return lost_sn;
		}
		//check time first, avoid two small frame mixed case decode fail
		if (last_ts != m_cache_video_pkts[index].m_ts) {
			end_sn = lost_sn - 1;
			return -1;
		}

		if (m_cache_video_pkts[index].m_pkt->m_header.get_marker()) {
			end_sn = lost_sn;
			return -1;
		}
	}

	yang_error(
			"the cache is mess. the packet count of video frame is more than %u",
			s_cache_size);
	return -2;
}

int32_t YangRtcVideoRecvTrack::on_rtp(YangRtpPacket *src) {
	int32_t err = Yang_Ok;
	YangRtpPacket *pkt = src->copy();
	int32_t seq = pkt->m_header.get_sequence();
	if (pkt->is_keyframe()) {
		return packet_video_key_frame(pkt);
	}
	if (!m_hasRequestKeyframe) {
		m_session->send_rtcp_fb_pli(this->m_track_desc->ssrc_);
		m_hasRequestKeyframe = 1;
	}
	// store in cache

	int32_t index = cache_index(seq);
	m_cache_video_pkts[index].m_in_use = true;
	m_cache_video_pkts[index].m_pkt = pkt;
	m_cache_video_pkts[index].m_sn = pkt->m_header.get_sequence();
	m_cache_video_pkts[index].m_ts = pkt->m_header.get_timestamp();

	// check whether to recovery lost packet and can construct a video frame
	if (m_lost_sn == pkt->m_header.get_sequence()) {
		uint16_t tail_sn = 0;
		int32_t sn = find_next_lost_sn(m_lost_sn, tail_sn);
		if (-1 == sn) {
			//if (tail_sn != seq) {
				//printf("\nrecv lostsn==%hu", seq);
			//}
			if (check_frame_complete(m_header_sn, tail_sn)) {
				if ((err = packet_video(m_header_sn, tail_sn)) != Yang_Ok) {
					err = yang_error_wrap(err, "fail to pack video frame");
				}
			}
		} else if (-2 == sn) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER,
					"video cache is overflow");
		} else {
			m_lost_sn = (uint16_t) sn;
		}
	}
	//if (nack_enabled_)

	return err;
}

int32_t YangRtcVideoRecvTrack::check_send_nacks() {
	int32_t err = Yang_Ok;

	uint32_t timeout_nacks = 0;
	if ((err = do_check_send_nacks(timeout_nacks)) != Yang_Ok) {
		return yang_error_wrap(err, "video");
	}

	// If NACK timeout, start PLI if not requesting.
	if (timeout_nacks == 0) {
		return err;
	}



	return err;
}

