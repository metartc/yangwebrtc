#ifndef SRC_YANGRTP_YANGRTPRAWPAYLOAD_H_
#define SRC_YANGRTP_YANGRTPRAWPAYLOAD_H_
#include <yangrtp/YangRtp.h>
// Single payload data.
class YangRtpRawPayload : public YangRtpPayloader
{
public:
    // The RAW payload, directly point32_t to the shared memory.
    // @remark We only refer to the memory, user must free its bytes.

public:
    YangRtpRawPayload();
    virtual ~YangRtpRawPayload();
// interface ISrsRtpPayloader
public:
     uint64_t nb_bytes();
     int32_t encode(YangBuffer* buf);
     int32_t decode(YangBuffer* buf);
     YangRtpPayloader* copy();
     bool getStart(){return false;};
};

#endif /* SRC_YANGRTP_YANGRTPRAWPAYLOAD_H_ */
