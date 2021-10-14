#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>

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
