#include <yangrtp/YangRtpRawPayload.h>
#include <yangutil/sys/YangLog.h>
YangRtpRawPayload::YangRtpRawPayload() {

}

YangRtpRawPayload::~YangRtpRawPayload() {
}

uint64_t YangRtpRawPayload::nb_bytes() {
	return m_size;
}

int32_t YangRtpRawPayload::encode(YangBuffer *buf) {
	if (m_size <= 0) {
		return Yang_Ok;
	}

	if (!buf->require(m_size)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
				m_size);
	}

	buf->write_bytes(m_payload, m_size);

	return Yang_Ok;
}

int32_t YangRtpRawPayload::decode(YangBuffer *buf) {
	if (buf->empty()) {
		return Yang_Ok;
	}

	m_payload = buf->head();
	m_size = buf->left();

	return Yang_Ok;
}

YangRtpPayloader* YangRtpRawPayload::copy() {
	YangRtpRawPayload *cp = new YangRtpRawPayload();

	cp->m_payload = m_payload;
	cp->m_size = m_size;

	return cp;
}
