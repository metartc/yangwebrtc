#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangrtp/YangRtpPacketWrap.h>
#include <yangutil/sys/YangLog.h>
YangRtpPacketWrap::YangRtpPacketWrap() {
	m_videoSsrc = 0;
	m_videoSeq = 0;
	m_audioSeq = 0;
	m_audioSsrc = 0;
	m_rtpBuffer=NULL;
}

YangRtpPacketWrap::~YangRtpPacketWrap() {

}
void YangRtpPacketWrap::init(YangRtpBuffer* rtpBuffer){
	m_rtpBuffer=rtpBuffer;
}
int32_t YangRtpPacketWrap::on_audio(YangStreamCapture *audioFrame, YangRtpPacket *pkt) {
	pkt->m_header.set_payload_type(kAudioPayloadType);
	pkt->m_header.set_ssrc(m_audioSsrc);
	pkt->m_frame_type = SrsFrameTypeAudio;
	pkt->m_header.set_marker(true);

	pkt->m_header.set_sequence(m_audioSeq++);
	pkt->m_header.set_timestamp(audioFrame->getAudioTimestamp());
	pkt->m_header.set_padding(0);
	YangRtpRawPayload *payload = new YangRtpRawPayload();
	pkt->set_payload(payload, YangRtspPacketPayloadTypeRaw);

	payload->m_payload = m_rtpBuffer->getBuffer();//(char*) audioFrame->getAudioData();
	payload->m_nn_payload = audioFrame->getAudioLen();
	memcpy(payload->m_payload,audioFrame->getAudioData(),payload->m_nn_payload);
	return Yang_Ok;
}
int32_t YangRtpPacketWrap::on_h264_video(YangStreamCapture *videoFrame,
		vector<YangRtpPacket*> &pkts) {
	int32_t err = Yang_Ok;

	if (videoFrame->getVideoLen() <= kRtpMaxPayloadSize) {
		if ((err = package_single_nalu(videoFrame, pkts)) != Yang_Ok) {
			return yang_error_wrap(err, "package single nalu");
		}
	} else {
		if ((err = package_fu_a(videoFrame, kRtpMaxPayloadSize, pkts))
				!= Yang_Ok) {
			return yang_error_wrap(err, "package fu-a");
		}
	}

	if (!pkts.empty()) {
		pkts.back()->m_header.set_marker(true);
	}

	return err;
}

int32_t YangRtpPacketWrap::package_single_nalu(YangStreamCapture *videoFrame,
		vector<YangRtpPacket*> &pkts) {
	int32_t err = Yang_Ok;

	YangRtpPacket *pkt = new YangRtpPacket();
	pkts.push_back(pkt);

	pkt->m_header.set_payload_type(kVideoPayloadType);
	pkt->m_header.set_ssrc(m_videoSsrc);
	pkt->m_frame_type = SrsFrameTypeVideo;
	pkt->m_header.set_sequence(m_videoSeq++);
	pkt->m_header.set_timestamp(videoFrame->getVideoTimestamp());

	YangRtpRawPayload *raw = new YangRtpRawPayload();
	pkt->set_payload(raw, YangRtspPacketPayloadTypeRaw);

	raw->m_payload = m_rtpBuffer->getBuffer();
	raw->m_nn_payload = videoFrame->getVideoLen();
	memcpy(raw->m_payload,videoFrame->getVideoData(),raw->m_nn_payload);
	return err;
}
int32_t YangRtpPacketWrap::package_single_nalu(char* p,int32_t plen,int64_t timestamp,
		vector<YangRtpPacket*> &pkts) {

	int32_t err = Yang_Ok;

	YangRtpPacket *pkt = new YangRtpPacket();
	pkts.push_back(pkt);

	pkt->m_header.set_payload_type(kVideoPayloadType);
	pkt->m_header.set_ssrc(m_videoSsrc);
	pkt->m_frame_type = SrsFrameTypeVideo;
	pkt->m_header.set_sequence(m_videoSeq++);
	pkt->m_header.set_timestamp(timestamp);

	YangRtpRawPayload *raw = new YangRtpRawPayload();
	pkt->set_payload(raw, YangRtspPacketPayloadTypeRaw);

	raw->m_payload = m_rtpBuffer->getBuffer();
	raw->m_nn_payload = plen;
	memcpy(raw->m_payload,p,plen);
	return err;
}
int32_t YangRtpPacketWrap::package_fu_a(YangStreamCapture *videoFrame,
		int32_t fu_payload_size, vector<YangRtpPacket*> &pkts) {
	int32_t err = Yang_Ok;
	int32_t plen = videoFrame->getVideoLen();
	uint8_t *pdata = videoFrame->getVideoData();
	char *p = (char*) pdata + 1;
	int32_t nb_left = plen - 1;
	uint8_t header = pdata[0];
	uint8_t nal_type = header & kNalTypeMask;

	int32_t num_of_packet = 1 + (plen - 1) / fu_payload_size;
	for (int32_t i = 0; i < num_of_packet; ++i) {
		int32_t packet_size = yang_min(nb_left, fu_payload_size);

		YangRtpPacket *pkt = new YangRtpPacket();
		pkts.push_back(pkt);

		pkt->m_header.set_payload_type(kVideoPayloadType);
		pkt->m_header.set_ssrc(m_videoSsrc);
		pkt->m_frame_type = SrsFrameTypeVideo;
		pkt->m_header.set_sequence(m_videoSeq++);
		pkt->m_header.set_timestamp(videoFrame->getVideoTimestamp());

		YangRtpFUAPayload2 *fua = new YangRtpFUAPayload2();
		pkt->set_payload(fua, YangRtspPacketPayloadTypeFUA2);

		fua->m_nri = (YangAvcNaluType) header;
		fua->m_nalu_type = (YangAvcNaluType) nal_type;
		fua->m_start = bool(i == 0);
		fua->m_end = bool(i == num_of_packet - 1);

		fua->m_payload = m_rtpBuffer->getBuffer();
		fua->m_size = packet_size;
		memcpy(fua->m_payload,p,packet_size);
		p += packet_size;
		nb_left -= packet_size;
	}

	return err;
}

int32_t YangRtpPacketWrap::package_stap_a(YangStreamCapture *videoFrame,
		YangRtpPacket *pkt) {
	uint8_t *buf = videoFrame->getVideoData();
	int32_t spsLen = *(buf + 12) + 1;
	uint8_t *sps = buf + 13;
	int32_t ppsLen = *(sps + spsLen + 1) + 1;
	uint8_t *pps = buf + 13 + spsLen + 2;


	pkt->m_header.set_payload_type(kVideoPayloadType);
	pkt->m_header.set_ssrc(m_videoSsrc);
	pkt->m_frame_type = SrsFrameTypeVideo;
	pkt->m_nalu_type = (YangAvcNaluType) kStapA;
	pkt->m_header.set_marker(false);
	pkt->m_header.set_sequence(m_videoSeq++);
	pkt->m_header.set_timestamp(0);

	YangRtpSTAPPayload *stap = new YangRtpSTAPPayload();
	pkt->set_payload(stap, YangRtspPacketPayloadTypeSTAP);

	uint8_t header = sps[0];
	stap->m_nri = (YangAvcNaluType) header;


	if (true) {
		YangSample *sample = new YangSample();
		sample->m_bytes = (char*) sps;
		sample->m_size = spsLen;
		stap->m_nalus.push_back(sample);

	}

	if (true) {
		YangSample *sample = new YangSample();
		sample->m_bytes = (char*)pps;
		sample->m_size = ppsLen;
		stap->m_nalus.push_back(sample);
	}
	return Yang_Ok;

}

