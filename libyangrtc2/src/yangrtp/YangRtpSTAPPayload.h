#ifndef SRC_YANGRTP_YANGRTPSTAPPAYLOAD_H_
#define SRC_YANGRTP_YANGRTPSTAPPAYLOAD_H_
#include <yangrtp/YangRtp.h>

// STAP-A, for multiple NALUs.
class YangRtpSTAPPayload : public IYangRtpPayloader
{
public:
    // The NRI in NALU type.
    YangAvcNaluType m_nri;
    // The NALU samples, we will manage the samples.
    // @remark We only refer to the memory, user must free its bytes.
    std::vector<YangSample*> m_nalus;
public:
    YangRtpSTAPPayload();
    virtual ~YangRtpSTAPPayload();
public:
    YangSample* get_sps();
    YangSample* get_pps();
// interface ISrsRtpPayloader
public:
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer* buf);
    virtual int32_t decode(YangBuffer* buf);
    virtual IYangRtpPayloader* copy();
};

#endif /* SRC_YANGRTP_YANGRTPSTAPPAYLOAD_H_ */
