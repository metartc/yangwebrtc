#include <yangwebrtc/YangRtcVideoRecvTrack.h>
#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>

YangRtcVideoRecvTrack::YangRtcVideoRecvTrack(int32_t uid, YangRtcContext *conf,
		YangRtcSessionI *session, YangRtcTrack *track_desc,
		YangRtpBuffer *rtpBuffer, YangMixQueue *pmixque) :
		YangRecvTrack(uid, conf, session, track_desc, rtpBuffer, pmixque, false) {
	m_key_frame_ts = -1;
	m_lost_sn = 0;
	m_header_sn = 0;
	m_hasRequestKeyframe = false;
	memset(m_cache_video_pkts, 0, sizeof(m_cache_video_pkts));
	m_hasReceiveStap=-1;
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
            if(m_hasReceiveStap>-1&&m_hasReceiveStap==pkt->m_header.get_sequence()){
                continue;
            }else{
				for (int32_t j = 0; j < (int) stap_payload->m_nalus.size(); ++j) {
					YangSample *sample = stap_payload->m_nalus.at(j);
					if (sample->m_size > 0) {
						nb_payload += 4 + sample->m_size;

					}
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

	YangRtpPacket *header = m_cache_video_pkts[cache_index(start)].m_pkt;
	YangBuffer payload(buf, bufLen);

	if (header->is_keyframe()) {
		payload.write_1bytes(0x17); // type(4 bits): key frame; code(4bits): avc
		m_key_frame_ts = -1;
	} else {
		payload.write_1bytes(0x27); // type(4 bits): inter frame; code(4bits): avc
	}

	int64_t timestamp = header->get_avsync_time();
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
			yang_delete(pkt);
			continue;
		}

		YangRtpSTAPPayload *stap_payload =
				dynamic_cast<YangRtpSTAPPayload*>(pkt->payload());
		if (stap_payload) {
            if(m_hasReceiveStap>-1&&m_hasReceiveStap==pkt->m_header.get_sequence()){
                m_hasReceiveStap=-1;
                continue;
            }else{
				for (int32_t j = 0; j < (int) stap_payload->m_nalus.size(); ++j) {
					YangSample *sample = stap_payload->m_nalus.at(j);
					if (sample->m_size > 0) {

						payload.write_4bytes(sample->m_size);
						payload.write_bytes(sample->m_bytes, sample->m_size);

					}
				}
            }
			yang_delete(pkt);
			continue;
		}

		YangRtpRawPayload *raw_payload =
				dynamic_cast<YangRtpRawPayload*>(pkt->payload());
		if (raw_payload && raw_payload->m_nn_payload > 0) {
			payload.write_4bytes(raw_payload->m_nn_payload);
			payload.write_bytes(raw_payload->m_payload,
					raw_payload->m_nn_payload);

			yang_delete(pkt);
			continue;
		}

		yang_delete(pkt);
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
			m_hasReceiveStap=pkt->m_header.get_sequence();
		}
	}
	if (-1 == m_key_frame_ts) {
		m_key_frame_ts = pkt->get_avsync_time();
		m_header_sn = pkt->m_header.get_sequence();
		m_lost_sn = m_header_sn + 1;
		// Received key frame and clean cache of old p frame pkts
		clear_cached_video();
		//yang_trace("set ts=%lld, header=%hu, lost=%hu", key_frame_ts_, header_sn_, lost_sn_);
	} else if (m_key_frame_ts != pkt->get_avsync_time()) {
		//new key frame, clean cache
		//int64_t old_ts = key_frame_ts_;
		//uint16_t old_header_sn = header_sn_;
		//uint16_t old_lost_sn = lost_sn_;
		m_key_frame_ts = pkt->get_avsync_time();
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
	m_cache_video_pkts[index].m_ts = pkt->get_avsync_time();

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
			yang_delete(m_cache_video_pkts[i].m_pkt);
			m_cache_video_pkts[i].m_sn = 0;
			m_cache_video_pkts[i].m_ts = 0;
			m_cache_video_pkts[i].m_in_use = false;
		}
	}
}
int32_t YangRtcVideoRecvTrack::put_frame_mixvideo(char *p, int64_t timestamp,
		int32_t nb) {
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
	return Yang_Ok;
}
int32_t YangRtcVideoRecvTrack::put_frame_video(char *p, int64_t timestamp,
		int32_t nb) {

	//if(m_usingMixav) return put_frame_mixvideo(p,timestamp,nb);

	m_videoFrame.uid = m_uid;
	m_videoFrame.payload = (uint8_t*) p;
	m_videoFrame.nb = nb;
	m_videoFrame.timestamp = timestamp;
	if (m_recvcb)
		m_recvcb->receiveVideo(&m_videoFrame);
	yang_deleteA(p);

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

	yang_error("the cache is mess. the packet count of video frame is more than %u",
			s_cache_size);
	return -2;
}

int32_t YangRtcVideoRecvTrack::on_rtp(YangRtpPacket *src) {
	int32_t err = Yang_Ok;
	if (!m_hasRequestKeyframe) {
		m_session->send_rtcp_fb_pli(m_track_desc->ssrc_);
		m_hasRequestKeyframe = true;
	}

	src->set_avsync_time(src->m_header.get_timestamp());

	YangRtpPacket *pkt = src->copy();
	int32_t seq = pkt->m_header.get_sequence();
	if (pkt->is_keyframe()) {
		return packet_video_key_frame(pkt);
	}

	// store in cache

	int32_t index = cache_index(seq);
	m_cache_video_pkts[index].m_in_use = true;
	m_cache_video_pkts[index].m_pkt = pkt;
	m_cache_video_pkts[index].m_sn = pkt->m_header.get_sequence();
	m_cache_video_pkts[index].m_ts = pkt->get_avsync_time();

	// check whether to recovery lost packet and can construct a video frame
	if (m_lost_sn == pkt->m_header.get_sequence()) {
		uint16_t tail_sn = 0;
		int32_t sn = find_next_lost_sn(m_lost_sn, tail_sn);
		if (-1 == sn) {
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

	return err;
}

int32_t YangRtcVideoRecvTrack::check_send_nacks() {
	int32_t err = Yang_Ok;

	uint32_t timeout_nacks = 0;

	if ((err = do_check_send_nacks(timeout_nacks)) != Yang_Ok) {
		return yang_error_wrap(err, "video");
	}

	if (timeout_nacks == 0) {
		return err;
	}

	return err;
}
