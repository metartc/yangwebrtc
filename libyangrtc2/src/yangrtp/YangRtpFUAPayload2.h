#ifndef SRC_YANGRTP_YANGRTPFUAPAYLOAD2_H_
#define SRC_YANGRTP_YANGRTPFUAPAYLOAD2_H_
#include <yangrtp/YangRtp.h>
struct YangFua2Packet{
	   // The FUA header.
    bool start;
    bool end;

    YangAvcNaluType nri;
    YangAvcNaluType nalu_type;
};
int32_t yang_parseFua2(YangBuffer* buf,YangFua2Packet* pkt);
// FU-A, for one NALU with multiple fragments.
// With only one payload.
class YangRtpFUAPayload2 : public YangRtpPayloader
{
public:
    YangRtpFUAPayload2();
    virtual ~YangRtpFUAPayload2();
public:
    // The NRI in NALU type.
    YangAvcNaluType m_nri;
    // The FUA header.
    bool m_start;
    bool m_end;
    YangAvcNaluType m_nalu_type;
    // The payload and size,
public:
     uint64_t nb_bytes();
     int32_t encode(YangBuffer* buf);
     int32_t decode(YangBuffer* buf);
    YangRtpPayloader* copy();
    bool getStart();
};
#endif /* SRC_YANGRTP_YANGRTPFUAPAYLOAD2_H_ */
