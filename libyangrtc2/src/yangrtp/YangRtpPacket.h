#ifndef SRC_YANGRTP_YANGRTPPACKET_H_
#define SRC_YANGRTP_YANGRTPPACKET_H_
#include <yangutil/yangavtype.h>
#include <yangutil/buffer/YangBuffer.h>
#include <yangrtp/YangRtcp.h>

#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpBuffer.h>
enum YangFrameType
{
    // set to the zero to reserved, for array map.
    SrsFrameTypeReserved = 0,
    SrsFrameTypeForbidden = 0,

    // 8 = audio
    SrsFrameTypeAudio = 8,
    // 9 = video
    SrsFrameTypeVideo = 9,
    // 18 = script data
    SrsFrameTypeScript = 18,
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
