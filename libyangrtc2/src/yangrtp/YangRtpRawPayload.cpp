#include <yangrtp/YangRtpRawPayload.h>
#include <yangutil/sys/YangLog.h>
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
