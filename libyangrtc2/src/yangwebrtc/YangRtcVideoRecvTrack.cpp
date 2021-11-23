#include <yangwebrtc/YangRtcVideoRecvTrack.h>
#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>
YangRtcVideoRecvTrack::YangRtcVideoRecvTrack(int32_t uid, YangRtcContext *conf,
		YangRtcSessionI *session, YangRtcTrack *track_desc,
		YangRtpBuffer *rtpBuffer, YangMixQueue *pmixque) :
		YangRecvTrack(uid, conf, session, track_desc, rtpBuffer, pmixque, false) {

	if(conf&&conf->context&&conf->streamConf)
		conf->context->streams.setRtcMessageNotify(conf->streamConf->uid,this);

	m_key_frame_ts = -1;
	m_lost_sn = 0;
	m_header_sn = 0;
	m_hasRequestKeyframe = false;

	m_hasReceiveStap=-1;

	memset(m_cache_video_pkts, 0, s_cache_size*sizeof(YangRtcPacketCache));
	m_video_buffer=new char[YANG_VIDEO_ENCODE_BUFFER_LEN];
	memset(m_video_buffer,0,YANG_VIDEO_ENCODE_BUFFER_LEN);
}


YangRtcVideoRecvTrack::~YangRtcVideoRecvTrack() {

	yang_deleteA(m_video_buffer);
}



int32_t YangRtcVideoRecvTrack::packet_video(const uint16_t start,
		const uint16_t end) {
	int32_t err = Yang_Ok;
	uint16_t cnt = end - start + 1;
	YangRtcPacketCache *header = &m_cache_video_pkts[cache_index(start)];
	int64_t timestamp = header->ts;
	char* p=m_video_buffer;
	if (is_keyframe(header)) {
		*p++=0x17; // type(4 bits): key frame; code(4bits): avc
		m_key_frame_ts = -1;
	} else {
		*p++=0x27;// type(4 bits): inter frame; code(4bits): avc
	}
	*p++=0x01;
	*p++=0x00;
	*p++=0x00;
	*p++=0x00;

	int32_t nalu_len = 0;
	for (uint16_t i = 0; i < cnt; ++i) {
		uint16_t index = cache_index((start + i));
		YangRtcPacketCache *pkt = &m_cache_video_pkts[index];
		m_cache_video_pkts[index].in_use = false;

		if(pkt->nalu_type==kFuA){
			if (pkt->nb > 0) {
				if (pkt->fua2.start) {
					nalu_len = pkt->nb + 1;
					p+=4;
					*p++=pkt->fua2.nri | pkt->fua2.nalu_type;
					memcpy(p,pkt->payload,pkt->nb);
					p+=pkt->nb;
				} else {
					nalu_len += pkt->nb;
					memcpy(p,pkt->payload,pkt->nb);
					p+=pkt->nb;
					if (pkt->fua2.end) {
						//write nalu_len back
						yang_put_be32(p-(4 + nalu_len),nalu_len);
					}
				}
				pkt->fua2.start = pkt->fua2.end = false;
				pkt->fua2.nri = pkt->fua2.nalu_type = (YangAvcNaluType) 0;
				continue;
			}
		}

		if (pkt->nalu_type==kStapA) {

            if(m_hasReceiveStap>-1&&m_hasReceiveStap==pkt->sn){
                m_hasReceiveStap=-1;
                continue;
            }else{
            	YangBuffer stap_buf(pkt->payload,pkt->nb);
            	m_stap.reset();
            	m_stap.decode(&stap_buf);

				for (int32_t j = 0; j < (int) m_stap.m_nalus.size(); ++j) {
					YangSample *sample = m_stap.m_nalus.at(j);
					if (sample->m_size > 0) {
						yang_put_be32(p,sample->m_size);
						p+=4;
						memcpy(p,sample->m_bytes, sample->m_size);
						p+=sample->m_size;
					}
				}
				m_stap.reset();
				continue;
            }

		}
		if(pkt->payload){

			if (pkt->nb> 0) {
				yang_put_be32(p,pkt->nb);
				p+=4;
				memcpy(p,pkt->payload,pkt->nb);
				p+=pkt->nb;

				pkt->payload=NULL;

				continue;
			}
		}
		memset(&m_cache_video_pkts[i],0,sizeof(struct YangRtcPacketCache));
	}

	if ((err = put_frame_video(m_video_buffer, timestamp, p-m_video_buffer)) != Yang_Ok) {
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
	uint16_t index = cache_index(pkt->m_header.get_sequence());

	if (pkt->m_nalu_type==kStapA) {
		YangBuffer stap_buf(pkt->m_payload, pkt->m_nb);
		m_stap.reset();
		m_stap.decode(&stap_buf);
		YangSample *sps = m_stap.get_sps();
		YangSample *pps = m_stap.get_pps();
		if (NULL == sps || NULL == pps) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER,
					"no sps or pps in stap-a rtp. sps: %p, pps:%p", sps, pps);
		} else {
			//type_codec1 + avc_type + composition time + fix header + count of sps + len of sps + sps + count of pps + len of pps + pps
			// int32_t nb_payload = 1 + 1 + 3 + 5 + 1 + 2 + sps->size + 1 + 2 + pps->size;

			YangBuffer payload(m_video_buffer, 1024);
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
			m_hasReceiveStap=pkt->m_header.get_sequence();
			m_stap.reset();
			if ((err = put_frame_video(payload.data(), 0, payload.pos()))!= Yang_Ok) {
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

		m_key_frame_ts = pkt->m_header.get_timestamp();
		m_header_sn =  pkt->m_header.get_sequence();
		m_lost_sn = m_header_sn + 1;
		clear_cached_video();

	}


	m_cache_video_pkts[index].in_use = true;
	copy(pkt,&m_cache_video_pkts[index]);

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
		if (m_cache_video_pkts[i].in_use) {
			memset(&m_cache_video_pkts[i],0,sizeof(struct YangRtcPacketCache));
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


	return Yang_Ok;
}

bool YangRtcVideoRecvTrack::check_frame_complete(const uint16_t start,
		const uint16_t end) {
	uint16_t cnt = (end - start + 1);
	uint16_t fu_s_c = 0;
	uint16_t fu_e_c = 0;
	for (uint16_t i = 0; i < cnt; ++i) {
		int32_t index = cache_index((start + i));
		YangRtcPacketCache *pkt = &m_cache_video_pkts[index];
		if (pkt->nalu_type == kFuA) {
			if (pkt->fua2.start) {
				++fu_s_c;
			}

			if (pkt->fua2.end) {
				++fu_e_c;
			}
		}
	}

	return fu_s_c == fu_e_c;
}
int32_t YangRtcVideoRecvTrack::find_next_lost_sn(uint16_t current_sn,
		uint16_t &end_sn) {
	uint32_t last_ts = m_cache_video_pkts[cache_index(m_header_sn)].ts;
	for (int32_t i = 0; i < s_cache_size; ++i) {
		uint16_t lost_sn = current_sn + i;
		int32_t index = cache_index(lost_sn);

		if (!m_cache_video_pkts[index].in_use) {
			return lost_sn;
		}
		//check time first, avoid two small frame mixed case decode fail
		if (last_ts != m_cache_video_pkts[index].ts) {
			end_sn = lost_sn - 1;
			return -1;
		}

		if (m_cache_video_pkts[index].end) {
			end_sn = lost_sn;
			return -1;
		}
	}

	yang_error("the cache is mess. the packet count of video frame is more than %u",
			s_cache_size);
	return -2;
}
bool YangRtcVideoRecvTrack::is_keyframe(YangRtpPacket *pkt) {
	// It's normal H264 video rtp packet
	if (pkt->m_nalu_type == kStapA) {

		YangAvcNaluType nalu_type = (YangAvcNaluType) ((*(pkt->m_payload+3))
				& kNalTypeMask);
		if (nalu_type == YangAvcNaluTypeSPS
				|| nalu_type == YangAvcNaluTypePPS) {
			return true;
		}
	} else if (pkt->m_nalu_type == kFuA) {
		uint8_t v = (uint8_t)(*(pkt->m_payload+1));
		if (YangAvcNaluTypeIDR == YangAvcNaluType(v & kNalTypeMask))
			return true;

	} else {
		if ((pkt->m_nalu_type == YangAvcNaluTypeIDR)
				|| (pkt->m_nalu_type == YangAvcNaluTypeSPS)
				|| (pkt->m_nalu_type == YangAvcNaluTypePPS)) {
			return true;
		}
	}
	return false;
}
bool YangRtcVideoRecvTrack::is_keyframe(YangRtcPacketCache *pkt) {
	// It's normal H264 video rtp packet
	if (pkt->nalu_type == kStapA) {

		YangAvcNaluType nalu_type = (YangAvcNaluType) ((*(pkt->payload+3))
				& kNalTypeMask);
		if (nalu_type == YangAvcNaluTypeSPS
				|| nalu_type == YangAvcNaluTypePPS) {
			return true;
		}
	} else if (pkt->nalu_type == kFuA) {
		uint8_t v = (uint8_t)(*pkt->payload);
		if (YangAvcNaluTypeIDR == YangAvcNaluType(v & kNalTypeMask)) return true;

	} else {
		if ((pkt->nalu_type == YangAvcNaluTypeIDR)
				|| (pkt->nalu_type == YangAvcNaluTypeSPS)
				|| (pkt->nalu_type == YangAvcNaluTypePPS)) {
			return true;
		}
	}
	return false;
}

int32_t YangRtcVideoRecvTrack::on_rtp(YangRtpPacket *src) {
	int32_t err = Yang_Ok;
	if (!m_hasRequestKeyframe) {
		m_session->send_rtcp_fb_pli(m_track_desc->ssrc_);

	}
	src->m_nalu_type = YangAvcNaluType((uint8_t) (src->m_payload[0] & kNalTypeMask));

	if (is_keyframe(src)) {
		m_hasRequestKeyframe = true;
		return packet_video_key_frame(src);
	}
    if(src->m_nalu_type<24){
        if(src->m_header.get_marker()) {

            //yang_trace("\n");
            //for(int i=0;i<src->m_nb;i++) yang_trace("%02x,",(uint8_t)src->m_payload[i]);
           // yang_trace("\nraw type===%d,sn===%hu,size==%d",src->m_nalu_type,src->m_header.get_sequence(),src->m_nb);
            packet_video_Raw(src);
            return Yang_Ok;
        }

    }
	// store in cache

	int32_t index = cache_index(src->m_header.get_sequence());
	m_cache_video_pkts[index].in_use = true;
	copy(src,&m_cache_video_pkts[index]);


	// check whether to recovery lost packet and can construct a video frame
	if (m_lost_sn == src->m_header.get_sequence()) {
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

void YangRtcVideoRecvTrack::copy(YangRtpPacket *src, YangRtcPacketCache *pkt) {
	pkt->sn = src->m_header.get_sequence();
	pkt->end = src->m_header.get_marker();

	// For performance issue, do not copy the unused field.

	pkt->ts = src->m_header.get_timestamp();
	pkt->nalu_type = src->m_nalu_type;

	pkt->payload = src->m_payload;
	pkt->nb = src->m_nb;

  if (pkt->nalu_type == kFuA) {
        m_buf.init(src->m_payload, src->m_nb);
        yang_parseFua2(&m_buf,&pkt->fua2);
        pkt->payload = m_buf.head();
        pkt->nb = m_buf.left();
	}
}
int32_t YangRtcVideoRecvTrack::notify(int puid,YangRtcMessageType mess){

	if(mess==YangRTC_Decoder_Input) {
		m_hasRequestKeyframe=false;
		return m_session->send_rtcp_fb_pli(m_track_desc->ssrc_);
	}

	return Yang_Ok;
}

int32_t YangRtcVideoRecvTrack::packet_video_Raw(YangRtpPacket* pkt){
    int err=Yang_Ok;
    char* p=m_video_buffer;
    int64_t timestamp=pkt->m_header.get_timestamp();
    *p++=0x27;// type(4 bits): inter frame; code(4bits): avc
    *p++=0x01;
    *p++=0x00;
    *p++=0x00;
    *p++=0x00;
    yang_put_be32(p,pkt->m_nb);
    p+=4;
    memcpy(p,pkt->m_payload,pkt->m_nb);
    p+=pkt->m_nb;
    if ((err = put_frame_video(m_video_buffer, timestamp, p-m_video_buffer)) != Yang_Ok) {
        yang_warn("fail to pack video frame");
    }

    m_header_sn = pkt->m_header.get_sequence() + 1;
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
