#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangutil/sys/YangLog.h>
#include <yangrtp/YangRtpConstant.h>

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
void YangRtpSTAPPayload::reset(){
	int32_t nn_nalus = (int) m_nalus.size();
    if(nn_nalus>0){
        for (int32_t i = 0; i < nn_nalus; i++) {
            YangSample *p = m_nalus[i];
            yang_delete(p);
        }
        m_nalus.clear();
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
bool YangRtpSTAPPayload::getStart(){

	return false;
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

YangRtpPayloader* YangRtpSTAPPayload::copy() {
	YangRtpSTAPPayload *cp = new YangRtpSTAPPayload();

	cp->m_nri = m_nri;

	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		cp->m_nalus.push_back(p->copy());
	}

	return cp;
}
