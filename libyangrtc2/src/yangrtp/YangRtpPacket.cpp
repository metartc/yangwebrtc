#include <yangutil/sys/YangLog.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtp.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/yangavinfotype.h>
YangRtpPacketUtil::YangRtpPacketUtil() {
	m_videoSsrc = 0;
	m_videoSeq = 0;
	m_audioSeq = 0;
	m_audioSsrc = 0;
	m_rtpBuffer=NULL;
}

YangRtpPacketUtil::~YangRtpPacketUtil() {

}
void YangRtpPacketUtil::init(YangRtpBuffer* rtpBuffer){
	m_rtpBuffer=rtpBuffer;
}
int32_t YangRtpPacketUtil::on_audio(YangStreamCapture *audioFrame, YangRtpPacket *pkt) {
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
int32_t YangRtpPacketUtil::on_h264_video(YangStreamCapture *videoFrame,
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

int32_t YangRtpPacketUtil::package_single_nalu(YangStreamCapture *videoFrame,
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
int32_t YangRtpPacketUtil::package_single_nalu(char* p,int32_t plen,int64_t timestamp,
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
int32_t YangRtpPacketUtil::package_fu_a(YangStreamCapture *videoFrame,
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

int32_t YangRtpPacketUtil::package_stap_a(YangStreamCapture *videoFrame,
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

YangRtpHeader::YangRtpHeader() {
	m_cc = 0;
	m_marker = false;
	m_payload_type = 0;
	m_sequence = 0;
	m_timestamp = 0;
	m_ssrc = 0;
	m_padding_length = 0;
	m_ignore_padding = false;
	memset(m_csrc, 0, sizeof(m_csrc));
}

YangRtpHeader::~YangRtpHeader() {
}

int32_t YangRtpHeader::decode(YangBuffer *buf) {
	int32_t err = Yang_Ok;

	if (!buf->require(kRtpHeaderFixedSize)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d+ bytes",
				kRtpHeaderFixedSize);
	}

	/* @see https://tools.ietf.org/html/rfc1889#section-5.1
	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |V=2|P|X|  CC   |M|     PT      |       sequence number         |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                           timestamp                           |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |           synchronization source (SSRC) identifier            |
	 +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	 |            contributing source (CSRC) identifiers             |
	 |                             ....                              |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 */

	uint8_t first = buf->read_1bytes();
	bool padding = (first & 0x20);
	bool extension = (first & 0x10);
	m_cc = (first & 0x0F);

	uint8_t second = buf->read_1bytes();
	m_marker = (second & 0x80);
	m_payload_type = (second & 0x7F);

	m_sequence = buf->read_2bytes();
	m_timestamp = buf->read_4bytes();
	m_ssrc = buf->read_4bytes();

	int32_t ext_bytes = nb_bytes() - kRtpHeaderFixedSize;
	if (!buf->require(ext_bytes)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d+ bytes",
				ext_bytes);
	}

	for (uint8_t i = 0; i < m_cc; ++i) {
		m_csrc[i] = buf->read_4bytes();
	}

	if (extension) {
		if ((err = parse_extensions(buf)) != Yang_Ok) {
			return yang_error_wrap(err, "fail to parse extension");
		}
	}

	if (padding && !m_ignore_padding && !buf->empty()) {
		m_padding_length = *(reinterpret_cast<uint8_t*>(buf->data() + buf->size()
				- 1));
		if (!buf->require(m_padding_length)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER,
					"padding requires %d bytes", m_padding_length);
		}
	}

	return err;
}

int32_t YangRtpHeader::parse_extensions(YangBuffer *buf) {
	int32_t err = Yang_Ok;

	if (Yang_Ok != (err = m_extensions.decode(buf))) {
		return yang_error_wrap(err, "decode rtp extension");
	}

	return err;
}

int32_t YangRtpHeader::encode(YangBuffer *buf) {
	int32_t err = Yang_Ok;

	// Encode the RTP fix header, 12bytes.
	// @see https://tools.ietf.org/html/rfc1889#section-5.1
	// The version, padding, extension and cc, total 1 byte.
	uint8_t v = 0x80 | m_cc;
	if (m_padding_length > 0) {
		v |= 0x20;
	}
	if (m_extensions.exists()) {
		v |= 0x10;
	}
	buf->write_1bytes(v);

	// The marker and payload type, total 1 byte.
	v = m_payload_type;
	if (m_marker) {
		v |= kRtpMarker;
	}
	buf->write_1bytes(v);

	// The sequence number, 2 bytes.
	buf->write_2bytes(m_sequence);

	// The timestamp, 4 bytes.
	buf->write_4bytes(m_timestamp);

	// The SSRC, 4 bytes.
	buf->write_4bytes(m_ssrc);

	// The CSRC list: 0 to 15 items, each is 4 bytes.
	for (size_t i = 0; i < m_cc; ++i) {
		buf->write_4bytes(m_csrc[i]);
	}

	if (m_extensions.exists()) {
		if (Yang_Ok != (err = m_extensions.encode(buf))) {
			return yang_error_wrap(err, "encode rtp extension");
		}
	}

	return err;
}
void YangRtpHeader::set_extensions(YangRtpExtensionTypes *extmap) {
	if (extmap) {
		m_extensions.set_types_(extmap);
	}
}

void YangRtpHeader::ignore_padding(bool v) {
	m_ignore_padding = v;
}

int32_t YangRtpHeader::get_twcc_sequence_number(uint16_t &twcc_sn) {
	if (m_extensions.exists()) {
		return m_extensions.get_twcc_sequence_number(twcc_sn);
	}
	return yang_error_wrap(ERROR_RTC_RTP_MUXER, "no rtp extension");
}

int32_t YangRtpHeader::set_twcc_sequence_number(uint8_t id, uint16_t sn) {
	return m_extensions.set_twcc_sequence_number(id, sn);
}

uint64_t YangRtpHeader::nb_bytes() {
	return kRtpHeaderFixedSize + m_cc * 4
			+ (m_extensions.exists() ? m_extensions.nb_bytes() : 0);
}

void YangRtpHeader::set_marker(bool v) {
	m_marker = v;
}

bool YangRtpHeader::get_marker() const {
	return m_marker;
}

void YangRtpHeader::set_payload_type(uint8_t v) {
	m_payload_type = v;
}

uint8_t YangRtpHeader::get_payload_type() const {
	return m_payload_type;
}

void YangRtpHeader::set_sequence(uint16_t v) {
	m_sequence = v;
}

uint16_t YangRtpHeader::get_sequence() const {
	return m_sequence;
}

void YangRtpHeader::set_timestamp(uint32_t v) {
	m_timestamp = v;
}

uint32_t YangRtpHeader::get_timestamp() const {
	return m_timestamp;
}

void YangRtpHeader::set_ssrc(uint32_t v) {
	m_ssrc = v;
}

void YangRtpHeader::set_padding(uint8_t v) {
	m_padding_length = v;
}

uint8_t YangRtpHeader::get_padding() const {
	return m_padding_length;
}

IYangRtpPayloader::IYangRtpPayloader() {
}

IYangRtpPayloader::~IYangRtpPayloader() {
}

IYangRtspPacketDecodeHandler::IYangRtspPacketDecodeHandler() {
}

IYangRtspPacketDecodeHandler::~IYangRtspPacketDecodeHandler() {
}

YangRtpPacket::YangRtpPacket() {
	m_payload = NULL;
	m_payload_type = YangRtspPacketPayloadTypeUnknown;
	m_actual_buffer_size = 0;

	m_nalu_type = YangAvcNaluTypeReserved;
	m_frame_type = SrsFrameTypeReserved;
	m_cached_payload_size = 0;
	m_decode_handler = NULL;

}

YangRtpPacket::~YangRtpPacket() {
	yang_delete(m_payload);

}
char* YangRtpPacket::wrap(YangRtpBuffer* rtpb,char* data,int32_t nb){
	m_actual_buffer_size=nb;

	char* p=rtpb->getBuffer();
	memcpy(p,data,nb);
	return p;
}

YangRtpPacket* YangRtpPacket::copy() {
	YangRtpPacket *cp = new YangRtpPacket();

	cp->m_header = m_header;
	cp->m_payload = m_payload ? m_payload->copy() : NULL;
	cp->m_payload_type = m_payload_type;

	cp->m_nalu_type = m_nalu_type;
	//cp->shared_buffer_ = shared_buffer_ ? shared_buffer_->copy2() : NULL;
	cp->m_actual_buffer_size = m_actual_buffer_size;
	cp->m_frame_type = m_frame_type;

	cp->m_cached_payload_size = m_cached_payload_size;
	// For performance issue, do not copy the unused field.
	cp->m_decode_handler = m_decode_handler;
	cp->m_avsync_time = m_avsync_time;

	return cp;
}

void YangRtpPacket::set_padding(int32_t size) {
	m_header.set_padding(size);
	if (m_cached_payload_size) {
		m_cached_payload_size += size - m_header.get_padding();
	}
}

void YangRtpPacket::add_padding(int32_t size) {
	m_header.set_padding(m_header.get_padding() + size);
	if (m_cached_payload_size) {
		m_cached_payload_size += size;
	}
}

void YangRtpPacket::set_decode_handler(IYangRtspPacketDecodeHandler *h) {
	m_decode_handler = h;
}

bool YangRtpPacket::is_audio() {
	return m_frame_type == SrsFrameTypeAudio;
}

void YangRtpPacket::set_extension_types(YangRtpExtensionTypes *v) {
	return m_header.set_extensions(v);
}

uint64_t YangRtpPacket::nb_bytes() {
	if (!m_cached_payload_size) {
		int32_t nn_payload = (m_payload ? m_payload->nb_bytes() : 0);
		m_cached_payload_size = m_header.nb_bytes() + nn_payload
				+ m_header.get_padding();
	}
	return m_cached_payload_size;
}

int32_t YangRtpPacket::encode(YangBuffer *buf) {
	int32_t err = Yang_Ok;

	if ((err = m_header.encode(buf)) != Yang_Ok) {
		return yang_error_wrap(err, "rtp header");
	}

	if (m_payload && (err = m_payload->encode(buf)) != Yang_Ok) {
		return yang_error_wrap(err, "rtp payload");
	}

	if (m_header.get_padding() > 0) {
		uint8_t padding = m_header.get_padding();
		if (!buf->require(padding)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
					padding);
		}
		memset(buf->head(), padding, padding);
		buf->skip(padding);
	}

	return err;
}

int32_t YangRtpPacket::decode(YangBuffer *buf) {
	int32_t err = Yang_Ok;

	if ((err = m_header.decode(buf)) != Yang_Ok) {
		return yang_error_wrap(err, "rtp header");
	}

	// We must skip the padding bytes before parsing payload.
	uint8_t padding = m_header.get_padding();
	if (!buf->require(padding)) {
		return yang_error_wrap(err, "requires padding %d bytes", padding);
	}
	buf->set_size(buf->size() - padding);

	// TODO: FIXME: We should keep payload to NULL and return if buffer is empty.
	// If user set the decode handler, call it to set the payload.
	if (m_decode_handler) {
		m_decode_handler->on_before_decode_payload(this, buf, &m_payload,
				&m_payload_type);
	}

	// By default, we always use the RAW payload.
	if (!m_payload) {
		m_payload = new YangRtpRawPayload();
		m_payload_type = YangRtspPacketPayloadTypeRaw;
	}

	if ((err = m_payload->decode(buf)) != Yang_Ok) {
		return yang_error_wrap(err, "rtp payload");
	}

	return err;
}

bool YangRtpPacket::is_keyframe() {
	// False if audio packet
	if (SrsFrameTypeAudio == m_frame_type) {
		return false;
	}
	// It's normal H264 video rtp packet
	if (m_nalu_type == kStapA) {
		YangRtpSTAPPayload *stap_payload =
				dynamic_cast<YangRtpSTAPPayload*>(m_payload);
		if (NULL != stap_payload->get_sps()
				|| NULL != stap_payload->get_pps()) {
			return true;
		}
	} else if (m_nalu_type == kFuA) {
		YangRtpFUAPayload2 *fua_payload =
				dynamic_cast<YangRtpFUAPayload2*>(m_payload);

		if (YangAvcNaluTypeIDR == fua_payload->m_nalu_type) {

			return true;
		}
	} else {
		if ((YangAvcNaluTypeIDR == m_nalu_type) || (YangAvcNaluTypeSPS == m_nalu_type)
				|| (YangAvcNaluTypePPS == m_nalu_type)) {
			return true;
		}
	}

	return false;
}

YangRtpRawPayload::YangRtpRawPayload() {
	m_payload = NULL;
	m_nn_payload = 0;
}

YangRtpRawPayload::~YangRtpRawPayload() {
}

uint64_t YangRtpRawPayload::nb_bytes() {
	return m_nn_payload;
}

int32_t YangRtpRawPayload::encode(YangBuffer *buf) {
	if (m_nn_payload <= 0) {
		return Yang_Ok;
	}

	if (!buf->require(m_nn_payload)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
				m_nn_payload);
	}

	buf->write_bytes(m_payload, m_nn_payload);

	return Yang_Ok;
}

int32_t YangRtpRawPayload::decode(YangBuffer *buf) {
	if (buf->empty()) {
		return Yang_Ok;
	}

	m_payload = buf->head();
	m_nn_payload = buf->left();

	return Yang_Ok;
}

IYangRtpPayloader* YangRtpRawPayload::copy() {
	YangRtpRawPayload *cp = new YangRtpRawPayload();

	cp->m_payload = m_payload;
	cp->m_nn_payload = m_nn_payload;

	return cp;
}

YangRtpRawNALUs::YangRtpRawNALUs() {
	m_cursor = 0;
	m_nn_bytes = 0;
}

YangRtpRawNALUs::~YangRtpRawNALUs() {
	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		yang_delete(p);
	}
}

void YangRtpRawNALUs::push_back(YangSample *sample) {
	if (sample->m_size <= 0) {
		return;
	}

	if (!m_nalus.empty()) {
		YangSample *p = new YangSample();
		p->m_bytes = (char*) "\0\0\1";
		p->m_size = 3;
		m_nn_bytes += 3;
		m_nalus.push_back(p);
	}

	m_nn_bytes += sample->m_size;
	m_nalus.push_back(sample);
}

uint8_t YangRtpRawNALUs::skip_first_byte() {
	// srs_assert (cursor >= 0 && nn_bytes > 0 && cursor < nn_bytes);
	m_cursor++;
	return uint8_t(m_nalus[0]->m_bytes[0]);
}

int32_t YangRtpRawNALUs::read_samples(vector<YangSample*> &samples, int32_t packet_size) {
	if (m_cursor + packet_size < 0 || m_cursor + packet_size > m_nn_bytes) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER,
				"cursor=%d, max=%d, size=%d", m_cursor, m_nn_bytes, packet_size);
	}

	int32_t pos = m_cursor;
	m_cursor += packet_size;
	int32_t left = packet_size;

	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; left > 0 && i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];

		// Ignore previous consumed samples.
		if (pos && pos - p->m_size >= 0) {
			pos -= p->m_size;
			continue;
		}

		// Now, we are working at the sample.
		int32_t nn = yang_min(left, p->m_size - pos);
		// srs_assert(nn > 0);

		YangSample *sample = new YangSample();
		samples.push_back(sample);

		sample->m_bytes = p->m_bytes + pos;
		sample->m_size = nn;

		left -= nn;
		pos = 0;
	}

	return Yang_Ok;
}

uint64_t YangRtpRawNALUs::nb_bytes() {
	int32_t size = 0;

	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		size += p->m_size;
	}

	return size;
}

int32_t YangRtpRawNALUs::encode(YangBuffer *buf) {
	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];

		if (!buf->require(p->m_size)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
					p->m_size);
		}

		buf->write_bytes(p->m_bytes, p->m_size);
	}

	return Yang_Ok;
}

int32_t YangRtpRawNALUs::decode(YangBuffer *buf) {
	if (buf->empty()) {
		return Yang_Ok;
	}

	YangSample *sample = new YangSample();
	sample->m_bytes = buf->head();
	sample->m_size = buf->left();
	buf->skip(sample->m_size);

	m_nalus.push_back(sample);

	return Yang_Ok;
}

IYangRtpPayloader* YangRtpRawNALUs::copy() {
	YangRtpRawNALUs *cp = new YangRtpRawNALUs();

	cp->m_nn_bytes = m_nn_bytes;
	cp->m_cursor = m_cursor;

	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		cp->m_nalus.push_back(p->copy());
	}

	return cp;
}

YangRtpSTAPPayload::YangRtpSTAPPayload() {
	m_nri = (YangAvcNaluType) 0;
}

YangRtpSTAPPayload::~YangRtpSTAPPayload() {
	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		yang_delete(p);
	}
}

YangSample* YangRtpSTAPPayload::get_sps() {
	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		if (!p || !p->m_size) {
			continue;
		}

		YangAvcNaluType nalu_type = (YangAvcNaluType) (p->m_bytes[0] & kNalTypeMask);
		if (nalu_type == YangAvcNaluTypeSPS) {
			return p;
		}
	}

	return NULL;
}

YangSample* YangRtpSTAPPayload::get_pps() {
	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		if (!p || !p->m_size) {
			continue;
		}

		YangAvcNaluType nalu_type = (YangAvcNaluType) (p->m_bytes[0] & kNalTypeMask);
		if (nalu_type == YangAvcNaluTypePPS) {
			return p;
		}
	}

	return NULL;
}

uint64_t YangRtpSTAPPayload::nb_bytes() {
	int32_t size = 1;

	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		size += 2 + p->m_size;
	}

	return size;
}

int32_t YangRtpSTAPPayload::encode(YangBuffer *buf) {
	if (!buf->require(1)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	// STAP header, RTP payload format for aggregation packets
	// @see https://tools.ietf.org/html/rfc6184#section-5.7
	uint8_t v = kStapA;
	v |= (m_nri & (~kNalTypeMask));
	buf->write_1bytes(v);

	// NALUs.
	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];

		if (!buf->require(2 + p->m_size)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
					2 + p->m_size);
		}

		buf->write_2bytes(p->m_size);
		buf->write_bytes(p->m_bytes, p->m_size);
	}

	return Yang_Ok;
}

int32_t YangRtpSTAPPayload::decode(YangBuffer *buf) {
	if (!buf->require(1)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	// STAP header, RTP payload format for aggregation packets
	// @see https://tools.ietf.org/html/rfc6184#section-5.7
	uint8_t v = buf->read_1bytes();

	// forbidden_zero_bit shoul be zero.
	// @see https://tools.ietf.org/html/rfc6184#section-5.3
	uint8_t f = (v & 0x80);
	if (f == 0x80) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER,
				"forbidden_zero_bit should be zero");
	}

	m_nri = YangAvcNaluType(v & (~kNalTypeMask));

	// NALUs.
	while (!buf->empty()) {
		if (!buf->require(2)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
		}

		int32_t size = buf->read_2bytes();
		if (!buf->require(size)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
					size);
		}

		YangSample *sample = new YangSample();
		sample->m_bytes = buf->head();
		sample->m_size = size;
		buf->skip(size);

		m_nalus.push_back(sample);
	}

	return Yang_Ok;
}

IYangRtpPayloader* YangRtpSTAPPayload::copy() {
	YangRtpSTAPPayload *cp = new YangRtpSTAPPayload();

	cp->m_nri = m_nri;

	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		cp->m_nalus.push_back(p->copy());
	}

	return cp;
}

YangRtpFUAPayload::YangRtpFUAPayload() {
	m_start = m_end = false;
	m_nri = m_nalu_type = (YangAvcNaluType) 0;
}

YangRtpFUAPayload::~YangRtpFUAPayload() {
	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		yang_delete(p);
	}
}

uint64_t YangRtpFUAPayload::nb_bytes() {
	int32_t size = 2;

	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		size += p->m_size;
	}

	return size;
}

int32_t YangRtpFUAPayload::encode(YangBuffer *buf) {
	if (!buf->require(2)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	// FU indicator, @see https://tools.ietf.org/html/rfc6184#section-5.8
	uint8_t fu_indicate = kFuA;
	fu_indicate |= (m_nri & (~kNalTypeMask));
	buf->write_1bytes(fu_indicate);

	// FU header, @see https://tools.ietf.org/html/rfc6184#section-5.8
	uint8_t fu_header = m_nalu_type;
	if (m_start) {
		fu_header |= kStart;
	}
	if (m_end) {
		fu_header |= kEnd;
	}
	buf->write_1bytes(fu_header);

	// FU payload, @see https://tools.ietf.org/html/rfc6184#section-5.8
	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];

		if (!buf->require(p->m_size)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
					p->m_size);
		}

		buf->write_bytes(p->m_bytes, p->m_size);
	}

	return Yang_Ok;
}

int32_t YangRtpFUAPayload::decode(YangBuffer *buf) {

	if (!buf->require(2)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
	}

	// FU indicator, @see https://tools.ietf.org/html/rfc6184#section-5.8
	uint8_t v = buf->read_1bytes();
	m_nri = YangAvcNaluType(v & (~kNalTypeMask));

	// FU header, @see https://tools.ietf.org/html/rfc6184#section-5.8
	v = buf->read_1bytes();
	m_start = v & kStart;
	m_end = v & kEnd;
	m_nalu_type = YangAvcNaluType(v & kNalTypeMask);

	if (!buf->require(1)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	YangSample *sample = new YangSample();
	sample->m_bytes = buf->head();
	sample->m_size = buf->left();
	buf->skip(sample->m_size);

	m_nalus.push_back(sample);

	return Yang_Ok;
}

IYangRtpPayloader* YangRtpFUAPayload::copy() {
	YangRtpFUAPayload *cp = new YangRtpFUAPayload();

	cp->m_nri = m_nri;
	cp->m_start = m_start;
	cp->m_end = m_end;
	cp->m_nalu_type = m_nalu_type;

	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		cp->m_nalus.push_back(p->copy());
	}

	return cp;
}

YangRtpFUAPayload2::YangRtpFUAPayload2() {

	m_start = m_end = false;
	m_nri = m_nalu_type = (YangAvcNaluType) 0;

	m_payload = NULL;
	m_size = 0;
}

YangRtpFUAPayload2::~YangRtpFUAPayload2() {
}

uint64_t YangRtpFUAPayload2::nb_bytes() {
	return 2 + m_size;
}

int32_t YangRtpFUAPayload2::encode(YangBuffer *buf) {
	if (!buf->require(2 + m_size)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	// Fast encoding.
	char *p = buf->head();

	// FU indicator, @see https://tools.ietf.org/html/rfc6184#section-5.8
	uint8_t fu_indicate = kFuA;
	fu_indicate |= (m_nri & (~kNalTypeMask));
	*p++ = fu_indicate;

	// FU header, @see https://tools.ietf.org/html/rfc6184#section-5.8
	uint8_t fu_header = m_nalu_type;
	if (m_start) {
		fu_header |= kStart;
	}
	if (m_end) {
		fu_header |= kEnd;
	}
	*p++ = fu_header;

	// FU payload, @see https://tools.ietf.org/html/rfc6184#section-5.8
	memcpy(p, m_payload, m_size);

	// Consume bytes.
	buf->skip(2 + m_size);

	return Yang_Ok;
}

int32_t YangRtpFUAPayload2::decode(YangBuffer *buf) {
	if (!buf->require(2)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
	}

	// FU indicator, @see https://tools.ietf.org/html/rfc6184#section-5.8
	uint8_t v = buf->read_1bytes();
	m_nri = YangAvcNaluType(v & (~kNalTypeMask));

	// FU header, @see https://tools.ietf.org/html/rfc6184#section-5.8
	v = buf->read_1bytes();
	m_start = v & kStart;
	m_end = v & kEnd;
	m_nalu_type = YangAvcNaluType(v & kNalTypeMask);
	if (!buf->require(1)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	m_payload = buf->head();
	m_size = buf->left();
	buf->skip(m_size);

	return Yang_Ok;
}

IYangRtpPayloader* YangRtpFUAPayload2::copy() {
	YangRtpFUAPayload2 *cp = new YangRtpFUAPayload2();

	cp->m_nri = m_nri;
	cp->m_start = m_start;
	cp->m_end = m_end;
	cp->m_nalu_type = m_nalu_type;
	cp->m_payload = m_payload;
	cp->m_size = m_size;

	return cp;
}


