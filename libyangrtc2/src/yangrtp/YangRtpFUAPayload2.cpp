#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>

int32_t yang_decode_h264_fua2(YangBuffer* buf,Fua2H264DataCache* pkt){
	if (!buf->require(2)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
		}

		// FU indicator, @see https://tools.ietf.org/html/rfc6184#section-5.8
		uint8_t v = buf->read_1bytes();
		pkt->nri = YangAvcNaluType(v & (~kNalTypeMask));

		// FU header, @see https://tools.ietf.org/html/rfc6184#section-5.8
		v = buf->read_1bytes();
		pkt->start = v & kStart;
		pkt->end = v & kEnd;
		pkt->nalu_type = YangAvcNaluType(v & kNalTypeMask);
		if (!buf->require(1)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
		}

		return Yang_Ok;
}
int32_t yang_decode_h265_fua2(YangBuffer* buf,Fua2H265DataCache* pkt){
	if (!buf->require(2)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
		}

		// FU indicator, @see https://tools.ietf.org/html/rfc6184#section-5.8
		uint8_t v = buf->read_1bytes();
		pkt->nri = YangHevcNaluType(v & (~kNalTypeMask));

		// FU header, @see https://tools.ietf.org/html/rfc6184#section-5.8
		v = buf->read_1bytes();
		pkt->start = v & kStart;
		pkt->end = v & kEnd;
		pkt->nalu_type = YangHevcNaluType(v & kNalTypeMask);
		if (!buf->require(1)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
		}

		return Yang_Ok;
}
int32_t yang_encode_h264_fua2(YangBuffer* buf,YangFua2H264Data* pkt){
	if (!buf->require(2 + pkt->nb)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
		}

		// Fast encoding.
		char *p = buf->head();

		// FU indicator, @see https://tools.ietf.org/html/rfc6184#section-5.8
		uint8_t fu_indicate = kFuA;
		fu_indicate |= (pkt->nri & (~kNalTypeMask));
		*p++ = fu_indicate;

		// FU header, @see https://tools.ietf.org/html/rfc6184#section-5.8
		uint8_t fu_header = pkt->nalu_type;
		if (pkt->start) {
			fu_header |= kStart;
		}
		if (pkt->end) {
			fu_header |= kEnd;
		}
		*p++ = fu_header;

		// FU payload, @see https://tools.ietf.org/html/rfc6184#section-5.8
		memcpy(p, pkt->payload, pkt->nb);

		// Consume bytes.
		buf->skip(2 + pkt->nb);

		return Yang_Ok;
}

int32_t yang_encode_h265_fua2(YangBuffer* buf,YangFua2H265Data* pkt){
	if (!buf->require(2 + pkt->nb)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
		}

		// Fast encoding.
		char *p = buf->head();

		// FU indicator, @see https://tools.ietf.org/html/rfc6184#section-5.8
		uint8_t fu_indicate = kFuA;
		fu_indicate |= (pkt->nri & (~kNalTypeMask));
		*p++ = fu_indicate;

		// FU header, @see https://tools.ietf.org/html/rfc6184#section-5.8
		uint8_t fu_header = pkt->nalu_type;
		if (pkt->start) {
			fu_header |= kStart;
		}
		if (pkt->end) {
			fu_header |= kEnd;
		}
		*p++ = fu_header;

		// FU payload, @see https://tools.ietf.org/html/rfc6184#section-5.8
		memcpy(p, pkt->payload, pkt->nb);

		// Consume bytes.
		buf->skip(2 + pkt->nb);

		return Yang_Ok;
}
