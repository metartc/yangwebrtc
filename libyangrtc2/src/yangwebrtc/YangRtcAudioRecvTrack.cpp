#include <yangwebrtc/YangRtcAudioRecvTrack.h>
#include <yangutil/sys/YangLog.h>
YangRtcAudioRecvTrack::YangRtcAudioRecvTrack(int32_t uid, YangRtcContext *conf,
		YangRtcSessionI *session, YangRtcTrack *track_desc,
		YangRtpBuffer *rtpBuffer, YangMixQueue *pmixque) :
		YangRecvTrack(uid, conf, session, track_desc, rtpBuffer, pmixque, true) {
	m_audioCacheSize = 3;
	m_aduioBuffer = NULL;
	if (conf->context->rtc.mixAvqueue == 0) {
		m_aduioBuffer = new YangRtpBuffer(m_audioCacheSize + 1, 5 * 512);
	}
}

YangRtcAudioRecvTrack::~YangRtcAudioRecvTrack() {
	for (std::map<int64_t, YangRtpPacket*>::iterator it = m_audioMap.begin();
			it != m_audioMap.end(); ++it) {
		yang_delete(it->second);
	}
	if (!m_audioMap.empty())
		m_audioMap.clear();
	yang_delete(m_aduioBuffer);
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
int32_t YangRtcAudioRecvTrack::on_mixrtp(YangRtpPacket *pkt) {
	YangMessage *prt = new YangMessage();
	YangRtpRawPayload *payload =
			dynamic_cast<YangRtpRawPayload*>(pkt->payload());
	prt->mediaType = 0;
	prt->nb = payload->m_nn_payload;
	prt->timestamp = pkt->get_avsync_time();
	prt->payload = payload->m_payload;
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

YangRtpPacket* YangRtcAudioRecvTrack::get_audiortp(YangRtpPacket *src){
	if (m_audioMap.find(src->get_avsync_time()) != m_audioMap.end()) {
		return NULL;
	}

	YangRtpPacket *rtpkt = src->copy();
	YangRtpRawPayload *tpayload1 =dynamic_cast<YangRtpRawPayload*>(rtpkt->payload());

	char *tmp = tpayload1->m_payload;
	tpayload1->m_payload = m_aduioBuffer->getBuffer();
	memcpy(tpayload1->m_payload, tmp, tpayload1->m_nn_payload);

	m_audioMap[rtpkt->get_avsync_time()] = rtpkt;


	if (m_audioMap.size() <= m_audioCacheSize) return NULL;

	map<int64_t,YangRtpPacket*>::iterator iter=m_audioMap.begin();
	YangRtpPacket* apkt=iter->second;
	m_audioMap.erase(iter);

	return apkt;
}
int32_t YangRtcAudioRecvTrack::on_rtp(YangRtpPacket *ppkt) {
	int32_t err = Yang_Ok;
	//pkt->set_avsync_time(cal_avsync_time(pkt->m_header.get_timestamp()));
	ppkt->set_avsync_time(ppkt->m_header.get_timestamp());

	//if(m_usingMixav) return on_mixrtp(ppkt);


	YangRtpPacket *pkt = get_audiortp(ppkt);
	if(!pkt) return err;
	YangRtpRawPayload *tpayload = dynamic_cast<YangRtpRawPayload*>(pkt->payload());
	if (tpayload){
		m_audioFrame.uid = m_uid;
		m_audioFrame.payload = (uint8_t*) tpayload->m_payload;
		m_audioFrame.nb = tpayload->m_nn_payload;
		m_audioFrame.timestamp = pkt->get_avsync_time();

		if (m_recvcb)	m_recvcb->receiveAudio(&m_audioFrame);
	}

	yang_delete(pkt);


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

