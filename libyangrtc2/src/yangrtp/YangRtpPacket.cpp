#include <yangrtp/YangRtp.h>
#include <yangutil/sys/YangLog.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/yangavinfotype.h>



YangRtpPacket::YangRtpPacket() {
	m_payload = NULL;
	m_payload_type = YangRtspPacketPayloadTypeUnknown;
	m_actual_buffer_size = 0;
	m_avsync_time=0;
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






