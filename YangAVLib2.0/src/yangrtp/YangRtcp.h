
#ifndef YANGWEBRTC_YANGRTCRTCP_H_
#define YANGWEBRTC_YANGRTCRTCP_H_
#include <stdint.h>
#include <string>
#include <string.h>
#include <vector>
#include <set>
#include <map>
#include <yangutil/sys/YangSsrc.h>
#include <yangrtp/YangRtp.h>
#include <yangutil/sys/YangTime.h>
#include <yangrtp/YangRtpConstant.h>

using namespace std;
class YangRtpNackForReceiver;
class YangRtpRingBuffer;
class YangSRtp;
class YangRtcSessionI {
public:
	YangRtcSessionI(){};
	virtual ~YangRtcSessionI(){};
	virtual int32_t check_send_nacks(YangRtpNackForReceiver* nack, uint32_t ssrc, uint32_t& sent_nacks, uint32_t& timeout_nacks)=0;
	virtual int32_t send_rtcp_rr(uint32_t ssrc, YangRtpRingBuffer* rtp_queue, const uint64_t& last_send_systime, const YangNtp& last_send_ntp)=0;
	virtual int32_t send_rtcp_xr_rrtr(uint32_t ssrc)=0;
	virtual int32_t send_rtcp(char *data, int32_t nb_data)=0;
	virtual int32_t send_packet(YangRtpPacket* pkt)=0;
	virtual int32_t send_rtcp_fb_pli(uint32_t ssrc)=0;
	virtual void do_request_keyframe(uint32_t ssrc)=0;
	virtual YangSRtp* getSrtp()=0;
};



// RTCP Packet Types, @see http://www.networksorcery.com/enp/protocol/rtcp.htm
enum YangRtcpType
{
    YangRtcpType_fir = 192,
    YangRtcpType_sr = 200,
    YangRtcpType_rr = 201,
    YangRtcpType_sdes = 202,
    YangRtcpType_bye = 203,
    YangRtcpType_app = 204,
    YangRtcpType_rtpfb = 205,
    YangRtcpType_psfb = 206,
    YangRtcpType_xr = 207,
};



// RTCP Header, @see http://tools.ietf.org/html/rfc3550#section-6.1
// @remark The header must be 4 bytes, which align with the max field size 2B.
struct YangRtcpHeader
{
	uint16_t rc:5;
	uint16_t padding:1;
	uint16_t version:2;
	uint16_t type:8;

	uint16_t length:16;

    YangRtcpHeader() {
        rc = 0;
        padding = 0;
        version = 0;
        type = 0;
        length = 0;
    }
};

class YangRtcpCommon
{
protected:
    YangRtcpHeader m_header;
    uint32_t m_ssrc;
    uint8_t m_payload[kRtcpPacketSize];
    int32_t m_payload_len;

    char* m_data;
    int32_t m_nb_data;
protected:
    int32_t decode_header(YangBuffer *buffer);
    int32_t encode_header(YangBuffer *buffer);
public:
    YangRtcpCommon();
    virtual ~YangRtcpCommon();
    virtual uint8_t type() const;
    virtual uint8_t get_rc() const;

    uint32_t get_ssrc();
    void set_ssrc(uint32_t ssrc);

    char* data();
    int32_t size();
// interface ISrsCodec
public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};

class YangRtcpApp : public YangRtcpCommon
{
private:
    uint8_t m_name[4];
public:
    YangRtcpApp();
    virtual ~YangRtcpApp();

    static bool is_rtcp_app(uint8_t *data, int32_t nb_data);

    virtual uint8_t type() const;

    uint8_t get_subtype() const;
    std::string get_name() const;
    int32_t get_payload(uint8_t*& payload, int& len);

    int32_t set_subtype(uint8_t type);
    int32_t set_name(std::string name);
    int32_t set_payload(uint8_t* payload, int32_t len);
// interface ISrsCodec
public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};

struct YangRtcpRB
{
    uint32_t ssrc;
    uint8_t fraction_lost;
    uint32_t lost_packets;
    uint32_t highest_sn;
    uint32_t jitter;
    uint32_t lsr;
    uint32_t dlsr;

    YangRtcpRB() {
        ssrc = 0;
        fraction_lost = 0;
        lost_packets = 0;
        highest_sn = 0;
        jitter = 0;
        lsr = 0;
        dlsr = 0;
    }
};

class YangRtcpSR : public YangRtcpCommon
{
private:
    uint64_t m_ntp;
    uint32_t m_rtp_ts;
    uint32_t m_send_rtp_packets;
    uint32_t m_send_rtp_bytes;

public:
    YangRtcpSR();
    virtual ~YangRtcpSR();

    uint8_t get_rc() const;
    // overload SrsRtcpCommon
    virtual uint8_t type() const;
    uint64_t get_ntp() const;
    uint32_t get_rtp_ts() const;
    uint32_t get_rtp_send_packets() const;
    uint32_t get_rtp_send_bytes() const;

    void set_ntp(uint64_t ntp);
    void set_rtp_ts(uint32_t ts);
    void set_rtp_send_packets(uint32_t packets);
    void set_rtp_send_bytes(uint32_t bytes);

public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};

class YangRtcpRR : public YangRtcpCommon
{
private:
    YangRtcpRB m_rb;
public:
    YangRtcpRR(uint32_t sender_ssrc = 0);
    virtual ~YangRtcpRR();

    // overload SrsRtcpCommon
    virtual uint8_t type() const;

    uint32_t get_rb_ssrc() const;
    float get_lost_rate() const;
    uint32_t get_lost_packets() const;
    uint32_t get_highest_sn() const;
    uint32_t get_jitter() const;
    uint32_t get_lsr() const;
    uint32_t get_dlsr() const;

    void set_rb_ssrc(uint32_t ssrc);
    void set_lost_rate(float rate);
    void set_lost_packets(uint32_t count);
    void set_highest_sn(uint32_t sn);
    void set_jitter(uint32_t jitter);
    void set_lsr(uint32_t lsr);
    void set_dlsr(uint32_t dlsr);
    void set_sender_ntp(uint64_t ntp);

public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);

};

// The Message format of TWCC, @see https://tools.ietf.org/html/draft-holmer-rmcat-transport-wide-cc-extensions-01#section-3.1
//       0                   1                   2                   3
//       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |V=2|P|  FMT=15 |    PT=205     |           length              |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |                     SSRC of packet sender                     |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |                      SSRC of media source                     |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |      base sequence number     |      packet status count      |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |                 reference time                | fb pkt. count |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |          packet chunk         |         packet chunk          |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      .                                                               .
//      .                                                               .
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |         packet chunk          |  recv delta   |  recv delta   |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      .                                                               .
//      .                                                               .
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |           recv delta          |  recv delta   | zero padding  |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define kTwccFbPktHeaderSize (4 + 8 + 8)
#define kTwccFbChunkBytes (2)
#define kTwccFbPktFormat (15)
#define kTwccFbPayloadType (205)
#define kTwccFbMaxPktStatusCount (0xffff)
#define kTwccFbDeltaUnit (250)	 // multiple of 250us
#define kTwccFbTimeMultiplier (kTwccFbDeltaUnit * (1 << 8)) // multiplicand multiplier/* 250us -> 64ms  (1 << 8) */
#define kTwccFbReferenceTimeDivisor ((1ll<<24) * kTwccFbTimeMultiplier) // dividend divisor

#define kTwccFbMaxRunLength 		0x1fff
#define kTwccFbOneBitElements 		14
#define kTwccFbTwoBitElements 		7
#define kTwccFbLargeRecvDeltaBytes	2
#define kTwccFbMaxBitElements 		kTwccFbOneBitElements

class YangRtcpTWCC : public YangRtcpCommon
{
private:
    uint32_t m_media_ssrc;
    uint16_t m_base_sn;
    int32_t m_reference_time;
    uint8_t m_fb_pkt_count;
    std::vector<uint16_t> m_encoded_chucks;
    std::vector<uint16_t> m_pkt_deltas;

    std::map<uint16_t, int64_t> m_recv_packets;
    std::set<uint16_t, YangSeqCompareLess> m_recv_sns;

    struct SrsRtcpTWCCChunk {
        uint8_t delta_sizes[kTwccFbMaxBitElements];
        uint16_t size;
        bool all_same;
        bool has_large_delta;
        SrsRtcpTWCCChunk();
    };

    int32_t m_pkt_len;
    uint16_t m_next_base_sn;
private:
    void clear();
    int64_t calculate_delta_us(int64_t ts, int64_t last);
    int32_t process_pkt_chunk(SrsRtcpTWCCChunk& chunk, int32_t delta_size);
    bool can_add_to_chunk(SrsRtcpTWCCChunk& chunk, int32_t delta_size);
    void add_to_chunk(SrsRtcpTWCCChunk& chunk, int32_t delta_size);
    int32_t encode_chunk(SrsRtcpTWCCChunk& chunk);
    int32_t encode_chunk_run_length(SrsRtcpTWCCChunk& chunk);
    int32_t encode_chunk_one_bit(SrsRtcpTWCCChunk& chunk);
    int32_t encode_chunk_two_bit(SrsRtcpTWCCChunk& chunk, size_t size, bool shift);
    void reset_chunk(SrsRtcpTWCCChunk& chunk);
    int32_t encode_remaining_chunk(SrsRtcpTWCCChunk& chunk);
public:
    YangRtcpTWCC(uint32_t sender_ssrc = 0);
    virtual ~YangRtcpTWCC();

    uint32_t get_media_ssrc() const;
    uint16_t get_base_sn() const;
    uint32_t get_reference_time() const;
    uint8_t get_feedback_count() const;
    std::vector<uint16_t> get_packet_chucks() const;
    std::vector<uint16_t> get_recv_deltas() const;

    void set_media_ssrc(uint32_t ssrc);
    void set_base_sn(uint16_t sn);
    void set_reference_time(uint32_t time);
    void set_feedback_count(uint8_t count);
    void add_packet_chuck(uint16_t chuck);
    void add_recv_delta(uint16_t delta);

    int32_t recv_packet(uint16_t sn, int64_t ts);
    bool need_feedback();


public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
private:
    int32_t do_encode(YangBuffer *buffer);
};

class YangRtcpNack : public YangRtcpCommon
{
private:
    struct SrsPidBlp {
        uint16_t pid;
        uint16_t blp;
        bool in_use;
    };

    uint32_t m_media_ssrc_;
    std::set<uint16_t, YangSeqCompareLess> m_lost_sns;
public:
    YangRtcpNack(uint32_t sender_ssrc = 0);
    virtual ~YangRtcpNack();

    uint32_t get_media_ssrc() const;
    std::vector<uint16_t> get_lost_sns() const;
    bool empty();

    void set_media_ssrc(uint32_t ssrc);
    void add_lost_sn(uint16_t sn);

public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};

class YangRtcpPsfbCommon : public YangRtcpCommon
{
protected:
    uint32_t m_media_ssrc;
public:
    YangRtcpPsfbCommon();
    virtual ~YangRtcpPsfbCommon();

    uint32_t get_media_ssrc() const;
    void set_media_ssrc(uint32_t ssrc);

public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};

class YangRtcpPli : public YangRtcpPsfbCommon
{
public:
    YangRtcpPli(uint32_t sender_ssrc = 0);
    virtual ~YangRtcpPli();


public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};

class YangRtcpSli : public YangRtcpPsfbCommon
{
private:
    uint16_t m_first;
    uint16_t m_number;
    uint8_t m_picture;
public:
    YangRtcpSli(uint32_t sender_ssrc = 0);
    virtual ~YangRtcpSli();

 // interface ISrsCodec
public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};


class YangRtcpRpsi : public YangRtcpPsfbCommon
{
private:
    uint8_t m_pb;
    uint8_t m_payload_type;
    char* m_native_rpsi;
    int32_t m_nb_native_rpsi;

public:
    YangRtcpRpsi(uint32_t sender_ssrc = 0);
    virtual ~YangRtcpRpsi();

 // interface ISrsCodec
public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};

class YangRtcpXr : public YangRtcpCommon
{
public:
    YangRtcpXr (uint32_t ssrc = 0);
    virtual ~YangRtcpXr();

   // interface ISrsCodec
public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};

class YangRtcpCompound //: public IYangCodec
{
private:
    std::vector<YangRtcpCommon*> m_rtcps;
    int32_t m_nb_bytes;
    char* m_data;
    int32_t m_nb_data;
public:
    YangRtcpCompound();
    virtual ~YangRtcpCompound();

    // TODO: FIXME: Should rename it to pop(), because it's not a GET method.
    YangRtcpCommon* get_next_rtcp();
    int32_t add_rtcp(YangRtcpCommon *rtcp);
    void clear();

    char* data();
    int32_t size();

// interface ISrsCodec
public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};
#endif /* YANGWEBRTC_YANGRTCRTCP_H_ */
