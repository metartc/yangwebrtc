#ifndef SRC_YANGRTP_YANGRTPRAWPAYLOAD_H_
#define SRC_YANGRTP_YANGRTPRAWPAYLOAD_H_
#include <yangrtp/YangRtp.h>
// Single payload data.
class YangRtpRawPayload : public IYangRtpPayloader
{
public:
    // The RAW payload, directly point32_t to the shared memory.
    // @remark We only refer to the memory, user must free its bytes.
    char* m_payload;
    int32_t m_nn_payload;
public:
    YangRtpRawPayload();
    virtual ~YangRtpRawPayload();
// interface ISrsRtpPayloader
public:
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer* buf);
    virtual int32_t decode(YangBuffer* buf);
    virtual IYangRtpPayloader* copy();
};

#endif /* SRC_YANGRTP_YANGRTPRAWPAYLOAD_H_ */
