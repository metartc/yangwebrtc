#ifndef SRC_YANGRTP_YANGRTPFUAPAYLOAD2_H_
#define SRC_YANGRTP_YANGRTPFUAPAYLOAD2_H_
#include <yangrtp/YangRtp.h>
// FU-A, for one NALU with multiple fragments.
// With only one payload.
class YangRtpFUAPayload2 : public IYangRtpPayloader
{
public:
    // The NRI in NALU type.
    YangAvcNaluType m_nri;
    // The FUA header.
    bool m_start;
    bool m_end;
    YangAvcNaluType m_nalu_type;
    // The payload and size,
    char* m_payload;
    int32_t m_size;
public:
    YangRtpFUAPayload2();
    virtual ~YangRtpFUAPayload2();
// interface ISrsRtpPayloader
public:
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer* buf);
    virtual int32_t decode(YangBuffer* buf);
    virtual IYangRtpPayloader* copy();
};
#endif /* SRC_YANGRTP_YANGRTPFUAPAYLOAD2_H_ */
