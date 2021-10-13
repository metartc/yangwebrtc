#ifndef SRC_YANGRTP_YANGRTPPACKET_H_
#define SRC_YANGRTP_YANGRTPPACKET_H_
#include <stdint.h>
#include <yangutil/buffer/YangBuffer.h>
#include <yangrtp/YangRtpCodec.h>
#include <yangrtp/YangRtcp.h>
#include <yangrtp/YangRtpQueue.h>
#include <yangstream/YangStreamCapture.h>


class YangRtpPacketUtil {
public:
	YangRtpPacketUtil();
	virtual ~YangRtpPacketUtil();
	void init(YangRtpBuffer* rtpBuffer);
	int32_t on_h264_video(YangStreamCapture* videoFrame,vector<YangRtpPacket*> &pkts);
	int32_t on_audio(YangStreamCapture* audioFrame,YangRtpPacket* pkt);
	int32_t package_single_nalu(char* p,int32_t plen, int64_t timestamp,vector<YangRtpPacket*>& pkts);
	int32_t package_single_nalu(YangStreamCapture* videoFrame, vector<YangRtpPacket*>& pkts);
	int32_t package_fu_a(YangStreamCapture* videoFrame,int32_t fu_payload_size, vector<YangRtpPacket*>& pkts);
	int32_t package_stap_a(YangStreamCapture* videoFrame,YangRtpPacket* pkt);
public:
	uint32_t m_videoSsrc;
	uint32_t m_audioSsrc;
	uint16_t m_videoSeq;
	uint16_t m_audioSeq;
private:
	YangRtpBuffer* m_rtpBuffer;
};
class YangRtpPacket;
class YangRtpExtensionTypes;
// Note that the header should never extends from any class, for performance.
class YangRtpHeader //: public IYangCodec
{
private:
    uint8_t m_padding_length;
    uint8_t m_cc;
    bool m_marker;
    uint8_t m_payload_type;
    uint16_t m_sequence;
    uint32_t m_timestamp;
    uint32_t m_ssrc;
    uint32_t m_csrc[15];
    YangRtpExtensions m_extensions;
    bool m_ignore_padding;
public:
    YangRtpHeader();
    virtual ~YangRtpHeader();
public:
    virtual int32_t decode(YangBuffer* buf);
private:
    int32_t parse_extensions(YangBuffer* buf);
public:
    virtual int32_t encode(YangBuffer* buf);
    virtual uint64_t nb_bytes();
public:
    void enable_twcc_decode() { m_extensions.enable_twcc_decode(); } // SrsRtpHeader::enable_twcc_decode
    void set_marker(bool v);
    bool get_marker() const;
    void set_payload_type(uint8_t v);
    uint8_t get_payload_type() const;
    void set_sequence(uint16_t v);
    uint16_t get_sequence() const;
    void set_timestamp(uint32_t v);
    uint32_t get_timestamp() const;
    void set_ssrc(uint32_t v);
    inline uint32_t get_ssrc() const { return m_ssrc; } // SrsRtpHeader::get_ssrc
    void set_padding(uint8_t v);
    uint8_t get_padding() const;
    void set_extensions(YangRtpExtensionTypes* extmap);
    void ignore_padding(bool v);
    int32_t get_twcc_sequence_number(uint16_t& twcc_sn);
    int32_t set_twcc_sequence_number(uint8_t id, uint16_t sn);
};


// The common payload interface for RTP packet.
class IYangRtpPayloader //: public IYangCodec
{
public:
    IYangRtpPayloader();
    virtual ~IYangRtpPayloader();
public:
    virtual IYangRtpPayloader* copy() = 0;
    virtual int32_t decode(YangBuffer* buf) = 0;
    virtual int32_t encode(YangBuffer* buf) = 0;
    virtual uint64_t nb_bytes() = 0;
};

// The payload type, for performance to avoid dynamic cast.
enum YangRtspPacketPayloadType
{
   YangRtspPacketPayloadTypeRaw,
    YangRtspPacketPayloadTypeFUA2,
	YangRtspPacketPayloadTypeFUA,
	YangRtspPacketPayloadTypeNALU,
	YangRtspPacketPayloadTypeSTAP,
	YangRtspPacketPayloadTypeUnknown,
};

class IYangRtspPacketDecodeHandler
{
public:
    IYangRtspPacketDecodeHandler();
    virtual ~IYangRtspPacketDecodeHandler();
public:
    // We don't know the actual payload, so we depends on external handler.
    virtual void on_before_decode_payload(YangRtpPacket* pkt, YangBuffer* buf, IYangRtpPayloader** ppayload, YangRtspPacketPayloadType* ppt) = 0;
};

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

// Multiple NALUs, automatically insert 001 between NALUs.
class YangRtpRawNALUs : public IYangRtpPayloader
{
private:
    // We will manage the samples, but the sample itself point32_t to the shared memory.
    std::vector<YangSample*> m_nalus;
    int32_t m_nn_bytes;
    int32_t m_cursor;
public:
    YangRtpRawNALUs();
    virtual ~YangRtpRawNALUs();
public:
    void push_back(YangSample* sample);
public:
    uint8_t skip_first_byte();
    // We will manage the returned samples, if user want to manage it, please copy it.
    int32_t read_samples(std::vector<YangSample*>& samples, int32_t packet_size);
// interface ISrsRtpPayloader
public:
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer* buf);
    virtual int32_t decode(YangBuffer* buf);
    virtual IYangRtpPayloader* copy();
};

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

// FU-A, for one NALU with multiple fragments.
// With more than one payload.
class YangRtpFUAPayload : public IYangRtpPayloader
{
public:
    // The NRI in NALU type.
    YangAvcNaluType m_nri;
    // The FUA header.
    bool m_start;
    bool m_end;
    YangAvcNaluType m_nalu_type;
    // The NALU samples, we manage the samples.
    // @remark We only refer to the memory, user must free its bytes.
    std::vector<YangSample*> m_nalus;
public:
    YangRtpFUAPayload();
    virtual ~YangRtpFUAPayload();
// interface ISrsRtpPayloader
public:
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer* buf);
    virtual int32_t decode(YangBuffer* buf);
    virtual IYangRtpPayloader* copy();
};

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


class YangRtpPacket
{
public:
    YangRtpPacket();
    virtual ~YangRtpPacket();
public:
    YangRtpHeader m_header;
private:
    IYangRtpPayloader* m_payload;
    YangRtspPacketPayloadType m_payload_type;
public:
    // The first byte as nalu type, for video decoder only.
    int32_t m_actual_buffer_size;
    YangAvcNaluType m_nalu_type;
    // The frame type, for RTMP bridger or SFU source.
    YangFrameType m_frame_type;
// Fast cache for performance.
private:
    // The cached payload size for packet.
    int32_t m_cached_payload_size;
    // The helper handler for decoder, use RAW payload if NULL.
    IYangRtspPacketDecodeHandler* m_decode_handler;

public:
     virtual YangRtpPacket* copy();
     char* wrap(YangRtpBuffer* rtpb,char* data,int32_t nb);
public:
    // Parse the TWCC extension, ignore by default.
    void enable_twcc_decode() { m_header.enable_twcc_decode(); } // SrsRtpPacket::enable_twcc_decode
    // Get and set the payload of packet.
    // @remark Note that return NULL if no payload.
    void set_payload(IYangRtpPayloader* p, YangRtspPacketPayloadType pt) { m_payload = p; m_payload_type = pt; }
    IYangRtpPayloader* payload() { return m_payload; }
    // Set the padding of RTP packet.
    void set_padding(int32_t size);
    // Increase the padding of RTP packet.
    void add_padding(int32_t size);
    // Set the decode handler.
    void set_decode_handler(IYangRtspPacketDecodeHandler* h);
    // Whether the packet is Audio packet.
    bool is_audio();
    // Set RTP header extensions for encoding or decoding header extension
    void set_extension_types(YangRtpExtensionTypes* v);
public:
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer* buf);
    virtual int32_t decode(YangBuffer* buf);
private:
    int64_t m_avsync_time;
public:
    bool is_keyframe();
    void set_avsync_time(int64_t avsync_time) { m_avsync_time = avsync_time; }
    int64_t get_avsync_time() const { return m_avsync_time; }
};

#endif /* SRC_YANGRTP_YANGRTPPACKET_H_ */
