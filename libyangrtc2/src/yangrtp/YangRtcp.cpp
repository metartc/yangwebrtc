
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif
#include <map>
#include "stdint.h"
#include <inttypes.h>
#include <yangrtp/YangRtcp.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLog.h>
using namespace std;

//YangRtcRtcp::YangRtcRtcp() {


//}

//YangRtcRtcp::~YangRtcRtcp() {

//}

YangRtcpCommon::YangRtcpCommon(): m_ssrc(0), m_data(NULL), m_nb_data(0)
{
    m_payload_len = 0;
}

YangRtcpCommon::~YangRtcpCommon()
{
}

uint8_t YangRtcpCommon::type() const
{
    return m_header.type;
}

uint8_t YangRtcpCommon::get_rc() const
{
    return m_header.rc;
}

uint32_t YangRtcpCommon::get_ssrc()
{
    return m_ssrc;
}

void YangRtcpCommon::set_ssrc(uint32_t ssrc)
{
    m_ssrc = ssrc;
}

char* YangRtcpCommon::data()
{
    return m_data;
}

int32_t YangRtcpCommon::size()
{
    return m_nb_data;
}

int32_t YangRtcpCommon::decode_header(YangBuffer *buffer)
{
    if (!buffer->require(sizeof(YangRtcpHeader) + 4)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "require %d", sizeof(YangRtcpHeader) + 4);
    }

    buffer->read_bytes((char*)(&m_header), sizeof(YangRtcpHeader));
    m_header.length = ntohs(m_header.length);

    int32_t payload_len = m_header.length * 4;
    if (payload_len > buffer->left()) {
        return yang_error_wrap(ERROR_RTC_RTCP,
                "require payload len=%u, buffer left=%u", payload_len, buffer->left());
    }
    m_ssrc = buffer->read_4bytes();

    return Yang_Ok;
}

int32_t YangRtcpCommon::encode_header(YangBuffer *buffer)
{
    if(! buffer->require(sizeof(YangRtcpHeader) + 4)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "require %d", sizeof(YangRtcpHeader) + 4);
    }
    m_header.length = htons(m_header.length);
    buffer->write_bytes((char*)(&m_header), sizeof(YangRtcpHeader));
    buffer->write_4bytes(m_ssrc);

    return Yang_Ok;
}

int32_t YangRtcpCommon::decode(YangBuffer *buffer)
{
    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    m_payload_len = (m_header.length + 1) * 4 - sizeof(YangRtcpHeader) - 4;
    buffer->read_bytes((char *)m_payload, m_payload_len);

    return err;
}

uint64_t YangRtcpCommon::nb_bytes()
{
    return sizeof(YangRtcpHeader) + 4 + m_payload_len;
}

int32_t YangRtcpCommon::encode(YangBuffer *buffer)
{
    return yang_error_wrap(ERROR_RTC_RTCP, "not implement");
}

YangRtcpApp::YangRtcpApp()
{
    m_ssrc = 0;
    m_header.padding = 0;
    m_header.type = YangRtcpType_app;
    m_header.rc = 0;
    m_header.version = kRtcpVersion;
}

YangRtcpApp::~YangRtcpApp()
{
}

bool YangRtcpApp::is_rtcp_app(uint8_t *data, int32_t nb_data)
{
    if (!data || nb_data <12) {
        return false;
    }

    YangRtcpHeader *header = (YangRtcpHeader*)data;
    if (header->version == kRtcpVersion
            && header->type == YangRtcpType_app
            && ntohs(header->length) >= 2) {
        return true;
    }

    return false;
}

uint8_t YangRtcpApp::type() const
{
    return YangRtcpType_app;
}

uint8_t YangRtcpApp::get_subtype() const
{
    return m_header.rc;
}

string YangRtcpApp::get_name() const
{
    return string((char*)m_name, strnlen((char*)m_name, 4));
}

int32_t YangRtcpApp::get_payload(uint8_t*& payload, int& len)
{
    len = m_payload_len;
    payload = m_payload;

    return Yang_Ok;
}

int32_t YangRtcpApp::set_subtype(uint8_t type)
{
    if(31 < type) {
        return yang_error_wrap(ERROR_RTC_RTCP, "invalid type: %d", type);
    }

    m_header.rc = type;

    return Yang_Ok;
}

int32_t YangRtcpApp::set_name(std::string name)
{
    if(name.length() > 4) {
        return yang_error_wrap(ERROR_RTC_RTCP, "invalid name length %zu", name.length());
    }

    memset(m_name, 0, sizeof(m_name));
    memcpy(m_name, name.c_str(), name.length());

    return Yang_Ok;
}

int32_t YangRtcpApp::set_payload(uint8_t* payload, int32_t len)
{
    if(len > (kRtcpPacketSize - 12)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "invalid payload length %d", len);
    }

    m_payload_len = (len + 3)/ 4 * 4;;
    memcpy(m_payload, payload, len);
    if (m_payload_len > len) {
        memset(&m_payload[len], 0, m_payload_len - len); //padding
    }
    m_header.length = m_payload_len/4 + 3 - 1;

    return Yang_Ok;
}

int32_t YangRtcpApp::decode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/rfc3550#section-6.7
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P| subtype |   PT=APP=204  |             length            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           SSRC/CSRC                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                          name (ASCII)                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                   application-dependent data                ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    if (m_header.type != YangRtcpType_app || !buffer->require(4)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "not rtcp app");
    }

    buffer->read_bytes((char *)m_name, sizeof(m_name));

    // TODO: FIXME: Should check size?
    m_payload_len = (m_header.length + 1) * 4 - 8 - sizeof(m_name);
    if (m_payload_len > 0) {
        buffer->read_bytes((char *)m_payload, m_payload_len);
    }

    return Yang_Ok;
}

uint64_t YangRtcpApp::nb_bytes()
{
    return sizeof(YangRtcpHeader) + sizeof(m_ssrc) + sizeof(m_name) + m_payload_len;
}

int32_t YangRtcpApp::encode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/rfc3550#section-6.7
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P| subtype |   PT=APP=204  |             length            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           SSRC/CSRC                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                          name (ASCII)                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                   application-dependent data                ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    int32_t err = Yang_Ok;

    if(!buffer->require(nb_bytes())) {
        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", nb_bytes());
    }

    if(Yang_Ok != (err = encode_header(buffer))) {
        return yang_error_wrap(err, "encode header");
    }

    buffer->write_bytes((char*)m_name, sizeof(m_name));
    buffer->write_bytes((char*)m_payload, m_payload_len);

    return Yang_Ok;
}

YangRtcpSR::YangRtcpSR()
{
    m_header.padding = 0;
    m_header.type = YangRtcpType_sr;
    m_header.rc = 0;
    m_header.version = kRtcpVersion;
    m_header.length = 6;

    m_ssrc = 0;
    m_ntp = 0;
    m_rtp_ts = 0;
    m_send_rtp_packets = 0;
    m_send_rtp_bytes = 0;
    m_send_rtp_bytes = 0;
}

YangRtcpSR::~YangRtcpSR()
{
}

uint8_t YangRtcpSR::get_rc() const
{
    return m_header.rc;
}

uint8_t YangRtcpSR::type() const
{
    return YangRtcpType_sr;
}

uint64_t YangRtcpSR::get_ntp() const
{
    return m_ntp;
}

uint32_t YangRtcpSR::get_rtp_ts() const
{
    return m_rtp_ts;
}

uint32_t YangRtcpSR::get_rtp_send_packets() const
{
    return m_send_rtp_packets;
}

uint32_t YangRtcpSR::get_rtp_send_bytes() const
{
    return m_send_rtp_bytes;
}

void YangRtcpSR::set_ntp(uint64_t ntp)
{
    m_ntp = ntp;
}

void YangRtcpSR::set_rtp_ts(uint32_t ts)
{
    m_rtp_ts = ts;
}

void YangRtcpSR::set_rtp_send_packets(uint32_t packets)
{
    m_send_rtp_packets = packets;
}

void YangRtcpSR::set_rtp_send_bytes(uint32_t bytes)
{
    m_send_rtp_bytes = bytes;
}

int32_t YangRtcpSR::decode(YangBuffer *buffer)
{
    /* @doc: https://tools.ietf.org/html/rfc3550#section-6.4.1
        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
header |V=2|P|    RC   |   PT=SR=200   |             length            |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         SSRC of sender                        |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
sender |              NTP timestamp, most significant word             |
info   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |             NTP timestamp, least significant word             |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         RTP timestamp                         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                     sender's packet count                     |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      sender's octet count                     |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_1 (SSRC of first source)                 |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  1    | fraction lost |       cumulative number of packets lost       |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |           extended highest sequence number received           |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      interarrival jitter                      |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         last SR (LSR)                         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                   delay since last SR (DLSR)                  |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_2 (SSRC of second source)                |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  2    :                               ...                             :
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
       |                  profile-specific extensions                  |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    m_ntp = buffer->read_8bytes();
    m_rtp_ts = buffer->read_4bytes();
    m_send_rtp_packets = buffer->read_4bytes();
    m_send_rtp_bytes = buffer->read_4bytes();

    if(m_header.rc > 0) {
        char buf[1500];
        buffer->read_bytes(buf, m_header.rc * 24);
    }

    return err;
}

uint64_t YangRtcpSR::nb_bytes()
{
    return (m_header.length + 1) * 4;
}

int32_t YangRtcpSR::encode(YangBuffer *buffer)
{
    int32_t err = Yang_Ok;
 /* @doc: https://tools.ietf.org/html/rfc3550#section-6.4.1
        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
header |V=2|P|    RC   |   PT=SR=200   |             length            |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         SSRC of sender                        |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
sender |              NTP timestamp, most significant word             |
info   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |             NTP timestamp, least significant word             |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         RTP timestamp                         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                     sender's packet count                     |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      sender's octet count                     |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_1 (SSRC of first source)                 |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  1    | fraction lost |       cumulative number of packets lost       |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |           extended highest sequence number received           |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      interarrival jitter                      |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         last SR (LSR)                         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                   delay since last SR (DLSR)                  |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_2 (SSRC of second source)                |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  2    :                               ...                             :
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
       |                  profile-specific extensions                  |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    if(!buffer->require(nb_bytes())) {
        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", nb_bytes());
    }

    if(Yang_Ok != (err = encode_header(buffer))) {
        return yang_error_wrap(err, "encode header");
    }

    buffer->write_8bytes(m_ntp);
    buffer->write_4bytes(m_rtp_ts);
    buffer->write_4bytes(m_send_rtp_packets);
    buffer->write_4bytes(m_send_rtp_bytes);

    return err;
}

YangRtcpRR::YangRtcpRR(uint32_t sender_ssrc)
{
    m_header.padding = 0;
    m_header.type = YangRtcpType_rr;
    m_header.rc = 0;
    m_header.version = kRtcpVersion;
    m_header.length = 7;
    m_ssrc = sender_ssrc;
    // TODO: FIXME: Fix warning.
   // memset(&m_rb, 0, sizeof(YangRtcpRB));
}

YangRtcpRR::~YangRtcpRR()
{
}

uint8_t YangRtcpRR::type() const
{
    return YangRtcpType_rr;
}

uint32_t YangRtcpRR::get_rb_ssrc() const
{
    return m_rb.ssrc;
}

float YangRtcpRR::get_lost_rate() const
{
    return m_rb.fraction_lost / 256;
}

uint32_t YangRtcpRR::get_lost_packets() const
{
    return m_rb.lost_packets;
}

uint32_t YangRtcpRR::get_highest_sn() const
{
    return m_rb.highest_sn;
}

uint32_t YangRtcpRR::get_jitter() const
{
    return m_rb.jitter;
}

uint32_t YangRtcpRR::get_lsr() const
{
    return m_rb.lsr;
}

uint32_t YangRtcpRR::get_dlsr() const
{
    return m_rb.dlsr;
}

void YangRtcpRR::set_rb_ssrc(uint32_t ssrc)
{
    m_rb.ssrc = ssrc;
}

void YangRtcpRR::set_lost_rate(float rate)
{
    m_rb.fraction_lost = rate * 256;
}

void YangRtcpRR::set_lost_packets(uint32_t count)
{
    m_rb.lost_packets = count;
}

void YangRtcpRR::set_highest_sn(uint32_t sn)
{
    m_rb.highest_sn = sn;
}

void YangRtcpRR::set_jitter(uint32_t jitter)
{
    m_rb.jitter = jitter;
}

void YangRtcpRR::set_lsr(uint32_t lsr)
{
    m_rb.lsr = lsr;
}

void YangRtcpRR::set_dlsr(uint32_t dlsr)
{
    m_rb.dlsr = dlsr;
}

void YangRtcpRR::set_sender_ntp(uint64_t ntp)
{
    uint32_t lsr = (uint32_t)((ntp >> 16) & 0x00000000FFFFFFFF);
    m_rb.lsr = lsr;
}

int32_t YangRtcpRR::decode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/rfc3550#section-6.4.2

        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
header |V=2|P|    RC   |   PT=RR=201   |             length            |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                     SSRC of packet sender                     |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_1 (SSRC of first source)                 |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  1    | fraction lost |       cumulative number of packets lost       |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |           extended highest sequence number received           |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      interarrival jitter                      |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         last SR (LSR)                         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                   delay since last SR (DLSR)                  |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_2 (SSRC of second source)                |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  2    :                               ...                             :
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
       |                  profile-specific extensions                  |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    // @doc https://tools.ietf.org/html/rfc3550#section-6.4.2
    // An empty RR packet (RC = 0) MUST be put at the head of a compound
    // RTCP packet when there is no data transmission or reception to
    // report. e.g. {80 c9 00 01 00 00 00 01}
    if(m_header.rc == 0) {
        return yang_error_wrap(ERROR_RTC_RTCP_EMPTY_RR, "rc=0");
    }

    // TODO: FIXME: Security check for read.
    m_rb.ssrc = buffer->read_4bytes();
    m_rb.fraction_lost = buffer->read_1bytes();
    m_rb.lost_packets = buffer->read_3bytes();
    m_rb.highest_sn = buffer->read_4bytes();
    m_rb.jitter = buffer->read_4bytes();
    m_rb.lsr = buffer->read_4bytes();
    m_rb.dlsr = buffer->read_4bytes();

    // TODO: FIXME: Security check for read.
    if(m_header.rc > 1) {
        char buf[1500];
        buffer->read_bytes(buf, (m_header.rc -1 ) * 24);
    }

    return err;
}

uint64_t YangRtcpRR::nb_bytes()
{
    return (m_header.length + 1) * 4;
}

int32_t YangRtcpRR::encode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/rfc3550#section-6.4.2

        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
header |V=2|P|    RC   |   PT=RR=201   |             length            |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                     SSRC of packet sender                     |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_1 (SSRC of first source)                 |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  1    | fraction lost |       cumulative number of packets lost       |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |           extended highest sequence number received           |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      interarrival jitter                      |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         last SR (LSR)                         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                   delay since last SR (DLSR)                  |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_2 (SSRC of second source)                |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  2    :                               ...                             :
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
       |                  profile-specific extensions                  |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    int32_t err = Yang_Ok;

    if(!buffer->require(nb_bytes())) {
        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", nb_bytes());
    }

    m_header.rc = 1;
    if(Yang_Ok != (err = encode_header(buffer))) {
        return yang_error_wrap(err, "encode header");
    }

    buffer->write_4bytes(m_rb.ssrc);
    buffer->write_1bytes(m_rb.fraction_lost);
    buffer->write_3bytes(m_rb.lost_packets);
    buffer->write_4bytes(m_rb.highest_sn);
    buffer->write_4bytes(m_rb.jitter);
    buffer->write_4bytes(m_rb.lsr);
    buffer->write_4bytes(m_rb.dlsr);

    return err;
}

YangRtcpTWCC::SrsRtcpTWCCChunk::SrsRtcpTWCCChunk()
        : size(0), all_same(true), has_large_delta(false)
{
}

YangRtcpTWCC::YangRtcpTWCC(uint32_t sender_ssrc) : m_pkt_len(0)
{
    m_header.padding = 0;
    m_header.type = YangRtcpType_rtpfb;
    m_header.rc = 15;
    m_header.version = kRtcpVersion;
    m_ssrc = sender_ssrc;
    m_media_ssrc = 0;
    m_base_sn = 0;
    m_reference_time = 0;
    m_fb_pkt_count = 0;
    m_next_base_sn = 0;
}

YangRtcpTWCC::~YangRtcpTWCC()
{
}

void YangRtcpTWCC::clear()
{
    m_encoded_chucks.clear();
    m_pkt_deltas.clear();
    m_recv_packets.clear();
    m_recv_sns.clear();
    m_next_base_sn = 0;
}

uint32_t YangRtcpTWCC::get_media_ssrc() const
{
    return m_media_ssrc;
}
uint16_t YangRtcpTWCC::get_base_sn() const
{
    return m_base_sn;
}

uint32_t YangRtcpTWCC::get_reference_time() const
{
    return m_reference_time;
}

uint8_t YangRtcpTWCC::get_feedback_count() const
{
    return m_fb_pkt_count;
}

vector<uint16_t> YangRtcpTWCC::get_packet_chucks() const
{
    return m_encoded_chucks;
}

vector<uint16_t> YangRtcpTWCC::get_recv_deltas() const
{
    return m_pkt_deltas;
}

void YangRtcpTWCC::set_media_ssrc(uint32_t ssrc)
{
    m_media_ssrc = ssrc;
}
void YangRtcpTWCC::set_base_sn(uint16_t sn)
{
    m_base_sn = sn;
}

void YangRtcpTWCC::set_reference_time(uint32_t time)
{
    m_reference_time = time;
}

void YangRtcpTWCC::set_feedback_count(uint8_t count)
{
    m_fb_pkt_count = count;
}

void YangRtcpTWCC::add_packet_chuck(uint16_t chunk)
{
    m_encoded_chucks.push_back(chunk);
}

void YangRtcpTWCC::add_recv_delta(uint16_t delta)
{
    m_pkt_deltas.push_back(delta);
}

int32_t YangRtcpTWCC::recv_packet(uint16_t sn, int64_t ts)
{
    map<uint16_t, int64_t>::iterator it = m_recv_packets.find(sn);
    if(it != m_recv_packets.end()) {
        return yang_error_wrap(ERROR_RTC_RTCP, "TWCC dup seq: %d", sn);
    }

    m_recv_packets[sn] = ts;
    m_recv_sns.insert(sn);

    return Yang_Ok;
}

bool YangRtcpTWCC::need_feedback()
{
    return m_recv_packets.size() > 0;
}

int32_t YangRtcpTWCC::decode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/draft-holmer-rmcat-transport-wide-cc-extensions-01#section-3.1
            0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |V=2|P|  FMT=15 |    PT=205     |           length              |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                     SSRC of packet sender                     |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      SSRC of media source                     |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |      base sequence number     |      packet status count      |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                 reference time                | fb pkt. count |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |          packet chunk         |         packet chunk          |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       .                                                               .
       .                                                               .
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |         packet chunk          |  recv delta   |  recv delta   |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       .                                                               .
       .                                                               .
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |           recv delta          |  recv delta   | zero padding  |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    m_payload_len = (m_header.length + 1) * 4 - sizeof(YangRtcpHeader) - 4;
    buffer->read_bytes((char *)m_payload, m_payload_len);

    return err;
}

uint64_t YangRtcpTWCC::nb_bytes()
{
    return kMaxUDPDataSize;
}

int64_t YangRtcpTWCC::calculate_delta_us(int64_t ts, int64_t last)
{
    int64_t divisor = kTwccFbReferenceTimeDivisor;
    int64_t delta_us = (ts - last) % divisor;

    if (delta_us > (divisor >> 1))
        delta_us -= divisor;

    delta_us += (delta_us < 0) ? (-kTwccFbDeltaUnit / 2) : (kTwccFbDeltaUnit / 2);
    delta_us /= kTwccFbDeltaUnit;

    return delta_us;
}

bool YangRtcpTWCC::can_add_to_chunk(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk, int32_t delta_size)
{
	yang_info("can_add %d chunk->size %u delta_sizes %d %d %d %d %d %d %d %d %d %d %d %d %d %d all_same %d has_large_delta %d",
	    delta_size, chunk.size, chunk.delta_sizes[0], chunk.delta_sizes[1], chunk.delta_sizes[2], chunk.delta_sizes[3],
	    chunk.delta_sizes[4], chunk.delta_sizes[5], chunk.delta_sizes[6], chunk.delta_sizes[7], chunk.delta_sizes[8],
	    chunk.delta_sizes[9], chunk.delta_sizes[10], chunk.delta_sizes[11], chunk.delta_sizes[12], chunk.delta_sizes[13],
	    (int)chunk.all_same, (int)chunk.has_large_delta);

    if (chunk.size < kTwccFbTwoBitElements) {
        return true;
    }

    if (chunk.size < kTwccFbOneBitElements && !chunk.has_large_delta && delta_size != kTwccFbLargeRecvDeltaBytes) {
        return true;
    }

    if (chunk.size < kTwccFbMaxRunLength && chunk.all_same && chunk.delta_sizes[0] == delta_size) {
        yang_info("< %d && all_same && delta_size[0] %d == %d", kTwccFbMaxRunLength, chunk.delta_sizes[0], delta_size);
        return true;
    }

    return false;
}

void YangRtcpTWCC::add_to_chunk(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk, int32_t delta_size)
{
    if (chunk.size < kTwccFbMaxBitElements) {
        chunk.delta_sizes[chunk.size] = delta_size;
    }

    chunk.size += 1;
    chunk.all_same = chunk.all_same && delta_size == chunk.delta_sizes[0];
    chunk.has_large_delta = chunk.has_large_delta || delta_size >= kTwccFbLargeRecvDeltaBytes;
}

int32_t YangRtcpTWCC::encode_chunk_run_length(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk)
{
    if (!chunk.all_same || chunk.size > kTwccFbMaxRunLength) {
        return yang_error_wrap(ERROR_RTC_RTCP, "invalid run all_same:%d, size:%d", chunk.all_same, chunk.size);
    }

    uint16_t encoded_chunk = (chunk.delta_sizes[0] << 13) | chunk.size;

    m_encoded_chucks.push_back(encoded_chunk);
    m_pkt_len += sizeof(encoded_chunk);

    return Yang_Ok;
}

int32_t YangRtcpTWCC::encode_chunk_one_bit(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk)
{
    int32_t i = 0;
    if (chunk.has_large_delta) {
        return yang_error_wrap(ERROR_RTC_RTCP, "invalid large delta");
    }

    uint16_t encoded_chunk = 0x8000;
    for (i = 0; i < chunk.size; ++i) {
        encoded_chunk |= (chunk.delta_sizes[i] << (kTwccFbOneBitElements - 1 - i));
    }

    m_encoded_chucks.push_back(encoded_chunk);
    m_pkt_len += sizeof(encoded_chunk);

    // 1 0 symbol_list
    return Yang_Ok;
}

int32_t YangRtcpTWCC::encode_chunk_two_bit(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk, size_t size, bool shift)
{
    uint32_t  i = 0;
    uint8_t delta_size = 0;

    uint16_t encoded_chunk = 0xc000;
    // 1 1 symbol_list
    for (i = 0; i < size; ++i) {
        encoded_chunk |= (chunk.delta_sizes[i] << (2 * (kTwccFbTwoBitElements - 1 - i)));
    }
    m_encoded_chucks.push_back(encoded_chunk);
    m_pkt_len += sizeof(encoded_chunk);

    if (shift) {
        chunk.size -= size;
        chunk.all_same = true;
        chunk.has_large_delta = false;
        for (i = 0; i < chunk.size; ++i) {
            delta_size = chunk.delta_sizes[i + size];
            chunk.delta_sizes[i] = delta_size;
            chunk.all_same = (chunk.all_same && delta_size == chunk.delta_sizes[0]);
            chunk.has_large_delta = chunk.has_large_delta || delta_size == kTwccFbLargeRecvDeltaBytes;
        }
    }

    return Yang_Ok;
}

void YangRtcpTWCC::reset_chunk(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk)
{
    chunk.size = 0;

    chunk.all_same = true;
    chunk.has_large_delta = false;
}

int32_t YangRtcpTWCC::encode_chunk(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk)
{
    int32_t err = Yang_Ok;

    if (can_add_to_chunk(chunk, 0) && can_add_to_chunk(chunk, 1) && can_add_to_chunk(chunk, 2)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "TWCC chunk");
    }

    if (chunk.all_same) {
        if ((err = encode_chunk_run_length(chunk)) != Yang_Ok) {
            return yang_error_wrap(err, "encode run");
        }
        reset_chunk(chunk);
        return err;
    }

    if (chunk.size == kTwccFbOneBitElements) {
        if ((err = encode_chunk_one_bit(chunk)) != Yang_Ok) {
            return yang_error_wrap(err, "encode chunk");
        }
        reset_chunk(chunk);
        return err;
    }

    if ((err = encode_chunk_two_bit(chunk, kTwccFbTwoBitElements, true)) != Yang_Ok) {
        return yang_error_wrap(err, "encode chunk");
    }

    return err;
}

int32_t YangRtcpTWCC::encode_remaining_chunk(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk)
{
    if (chunk.all_same) {
        return encode_chunk_run_length(chunk);
    } else if (chunk.size <= kTwccFbTwoBitElements) {
        // FIXME, TRUE or FALSE
        return encode_chunk_two_bit(chunk, chunk.size, false);
    }
    return encode_chunk_one_bit(chunk);
}

int32_t YangRtcpTWCC::process_pkt_chunk(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk, int32_t delta_size)
{
    int32_t err = Yang_Ok;

    size_t needed_chunk_size = chunk.size == 0 ? kTwccFbChunkBytes : 0;

    size_t might_occupied = m_pkt_len + needed_chunk_size + delta_size;
    if (might_occupied > kRtcpPacketSize) {
        return yang_error_wrap(ERROR_RTC_RTCP, "might_occupied %zu", might_occupied);
    }

    if (can_add_to_chunk(chunk, delta_size)) {
        //pkt_len += needed_chunk_size;
        add_to_chunk(chunk, delta_size);
        return err;
    }
    if ((err = encode_chunk(chunk)) != Yang_Ok) {
        return yang_error_wrap(err, "encode chunk, delta_size %u", delta_size);
    }
    add_to_chunk(chunk, delta_size);
    return err;
}

int32_t YangRtcpTWCC::encode(YangBuffer *buffer)
{
    int32_t err = Yang_Ok;

    err = do_encode(buffer);

    if (err != Yang_Ok || m_next_base_sn == 0) {
        clear();
    }

    return err;
}

int32_t YangRtcpTWCC::do_encode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/draft-holmer-rmcat-transport-wide-cc-extensions-01#section-3.1
            0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |V=2|P|  FMT=15 |    PT=205     |           length              |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                     SSRC of packet sender                     |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      SSRC of media source                     |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |      base sequence number     |      packet status count      |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                 reference time                | fb pkt. count |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |          packet chunk         |         packet chunk          |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       .                                                               .
       .                                                               .
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |         packet chunk          |  recv delta   |  recv delta   |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       .                                                               .
       .                                                               .
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |           recv delta          |  recv delta   | zero padding  |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    int32_t err = Yang_Ok;

    if(!buffer->require(nb_bytes())) {
        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", nb_bytes());
    }

    m_pkt_len = kTwccFbPktHeaderSize;

    set<uint16_t, YangSeqCompareLess>::iterator it_sn = m_recv_sns.begin();
    if (!m_next_base_sn) {
        m_base_sn = *it_sn;
    } else {
        m_base_sn = m_next_base_sn;
        it_sn = m_recv_sns.find(m_base_sn);
    }

    map<uint16_t, int64_t>::iterator it_ts = m_recv_packets.find(m_base_sn);
    int64_t ts = it_ts->second;

    m_reference_time = (ts % kTwccFbReferenceTimeDivisor) / kTwccFbTimeMultiplier;
    int64_t last_ts = (int64_t)(m_reference_time) * kTwccFbTimeMultiplier;

    uint16_t last_sn = m_base_sn;
    uint16_t packet_count = 0;

    // encode chunk
    YangRtcpTWCC::SrsRtcpTWCCChunk chunk;
    for(; it_sn != m_recv_sns.end(); ++it_sn) {
        // check whether exceed buffer len
        // max recv_delta_size = 2
        if (m_pkt_len + 2 >= buffer->left()) {
            break;
        }

        uint16_t current_sn = *it_sn;
        // calculate delta
        it_ts = m_recv_packets.find(current_sn);
        if (it_ts == m_recv_packets.end()) {
            continue;
        }

        packet_count++;
        int64_t delta_us = calculate_delta_us(it_ts->second, last_ts);
        int16_t delta = delta_us;
        if(delta != delta_us) {
            return yang_error_wrap(ERROR_RTC_RTCP, "twcc: delta:%lld, exceeds the 16bits", delta_us);
        }

        if(current_sn > (last_sn + 1)) {
            // lost packet
            for(uint16_t lost_sn = last_sn + 1; lost_sn < current_sn; ++lost_sn) {
                process_pkt_chunk(chunk, 0);
                packet_count++;
            }
        }

        // FIXME 24-bit base receive delta not supported
        int32_t recv_delta_size = (delta >= 0 && delta <= 0xff) ? 1 : 2;
        if ((err = process_pkt_chunk(chunk, recv_delta_size)) != Yang_Ok) {
            return yang_error_wrap(err, "delta_size %d, failed to append_recv_delta", recv_delta_size);
        }

        m_pkt_deltas.push_back(delta);
        last_ts += delta * kTwccFbDeltaUnit;
        m_pkt_len += recv_delta_size;
        last_sn = current_sn;

        m_recv_packets.erase(it_ts);
    }

    m_next_base_sn = 0;
    if (it_sn != m_recv_sns.end()) {
        m_next_base_sn = *it_sn;
    }

    if(0 < chunk.size) {
        if((err = encode_remaining_chunk(chunk)) != Yang_Ok) {
            return yang_error_wrap(err, "encode chunk");
        }
    }

    // encode rtcp twcc packet
    if((m_pkt_len % 4) == 0) {
        m_header.length = m_pkt_len / 4;
    } else {
        m_header.length = (m_pkt_len + 4 - (m_pkt_len%4)) / 4;
    }
    m_header.length -= 1;

    if(Yang_Ok != (err = encode_header(buffer))) {
        return yang_error_wrap(err, "encode header");
    }
    buffer->write_4bytes(m_media_ssrc);
    buffer->write_2bytes(m_base_sn);
    buffer->write_2bytes(packet_count);
    buffer->write_3bytes(m_reference_time);
    buffer->write_1bytes(m_fb_pkt_count);

    for(vector<uint16_t>::iterator it = m_encoded_chucks.begin(); it != m_encoded_chucks.end(); ++it) {
        buffer->write_2bytes(*it);
    }
    for(vector<uint16_t>::iterator it = m_pkt_deltas.begin(); it != m_pkt_deltas.end(); ++it) {
        if(0 <= *it && 0xFF >= *it) {
            // small delta
            uint8_t delta = *it;
            buffer->write_1bytes(delta);
        } else {
            // large or negative delta
            buffer->write_2bytes(*it);
        }
    }
    while((m_pkt_len % 4) != 0) {
        buffer->write_1bytes(0);
        m_pkt_len++;
    }

    m_encoded_chucks.clear();
    m_pkt_deltas.clear();

    return err;
}

YangRtcpNack::YangRtcpNack(uint32_t sender_ssrc)
{
    m_header.padding = 0;
    m_header.type = YangRtcpType_rtpfb;
    m_header.rc = 1;
    m_header.version = kRtcpVersion;
    m_ssrc = sender_ssrc;
    m_media_ssrc_ = 0;
}

YangRtcpNack::~YangRtcpNack()
{
}

uint32_t YangRtcpNack::get_media_ssrc() const
{
    return m_media_ssrc_;
}

vector<uint16_t> YangRtcpNack::get_lost_sns() const
{
    vector<uint16_t> sn;
    for(set<uint16_t, YangSeqCompareLess>::iterator it = m_lost_sns.begin(); it != m_lost_sns.end(); ++it) {
        sn.push_back(*it);
    }
    return sn;
}

bool YangRtcpNack::empty()
{
    return m_lost_sns.empty();
}

void YangRtcpNack::set_media_ssrc(uint32_t ssrc)
{
    m_media_ssrc_ = ssrc;
}

void YangRtcpNack::add_lost_sn(uint16_t sn)
{
    m_lost_sns.insert(sn);
}

int32_t YangRtcpNack::decode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/rfc4585#section-6.1
        0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|   FMT   |       PT      |          length               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of packet sender                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of media source                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   :            Feedback Control Information (FCI)                 :
   :                                                               :

    Generic NACK
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |            PID                |             BLP               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    m_media_ssrc_ = buffer->read_4bytes();
    char bitmask[20];
    for(int32_t i = 0; i < (m_header.length - 2); i++) {
        uint16_t pid = buffer->read_2bytes();
        uint16_t blp = buffer->read_2bytes();
        m_lost_sns.insert(pid);
        memset(bitmask, 0, 20);
        for(int32_t j=0; j<16; j++) {
            bitmask[j] = (blp & ( 1 << j )) >> j ? '1' : '0';
            if((blp & ( 1 << j )) >> j)
                m_lost_sns.insert(pid+j+1);
        }
        bitmask[16] = '\n';
        //srs_info("[%d] %d / %s", i, pid, bitmask);
    }

    return err;
}
uint64_t YangRtcpNack::nb_bytes()
{
    return kRtcpPacketSize;
}

int32_t YangRtcpNack::encode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/rfc4585#section-6.1
        0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|   FMT   |       PT      |          length               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of packet sender                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of media source                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   :            Feedback Control Information (FCI)                 :
   :                                                               :

    Generic NACK
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |            PID                |             BLP               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    int32_t err = Yang_Ok;
    if(!buffer->require(nb_bytes())) {
        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", nb_bytes());
    }

    vector<YangPidBlp> chunks;
    do {
        YangPidBlp chunk;
        chunk.in_use = false;
        uint16_t pid = 0;
        for(set<uint16_t, YangSeqCompareLess>::iterator it = m_lost_sns.begin(); it != m_lost_sns.end(); ++it) {
            uint16_t sn = *it;
            if(!chunk.in_use) {
                chunk.pid = sn;
                chunk.blp = 0;
                chunk.in_use = true;
                pid = sn;
                continue;
            }
            if((sn - pid) < 1) {
                yang_info("skip seq %d", sn);
            } else if( (sn - pid) > 16) {
                // add new chunk
                chunks.push_back(chunk);
                chunk.in_use = false;
            } else {
                chunk.blp |= 1 << (sn-pid-1);
            }
        }
        if(chunk.in_use) {
            chunks.push_back(chunk);
        }

        m_header.length = 2 + chunks.size();
        if(Yang_Ok != (err = encode_header(buffer))) {
            err = yang_error_wrap(err, "encode header");
            break;
        }

        buffer->write_4bytes(m_media_ssrc_);
        for(vector<YangPidBlp>::iterator it_chunk = chunks.begin(); it_chunk != chunks.end(); it_chunk++) {
            buffer->write_2bytes(it_chunk->pid);
            buffer->write_2bytes(it_chunk->blp);
        }
    } while(0);

    return err;
}

YangRtcpPsfbCommon::YangRtcpPsfbCommon()
{
    m_header.padding = 0;
    m_header.type = YangRtcpType_psfb;
    m_header.rc = 1;
    m_header.version = kRtcpVersion;
    //ssrc_ = sender_ssrc;
}

YangRtcpPsfbCommon::~YangRtcpPsfbCommon()
{

}

uint32_t YangRtcpPsfbCommon::get_media_ssrc() const
{
    return m_media_ssrc;
}

void YangRtcpPsfbCommon::set_media_ssrc(uint32_t ssrc)
{
    m_media_ssrc = ssrc;
}

int32_t YangRtcpPsfbCommon::decode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/rfc4585#section-6.1
        0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|   FMT   |       PT      |          length               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of packet sender                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of media source                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   :            Feedback Control Information (FCI)                 :
   :                                                               :
   */

    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    m_media_ssrc = buffer->read_4bytes();
    int32_t len = (m_header.length + 1) * 4 - 12;
    buffer->skip(len);
    return err;
}

uint64_t YangRtcpPsfbCommon::nb_bytes()
{
    return kRtcpPacketSize;
}

int32_t YangRtcpPsfbCommon::encode(YangBuffer *buffer)
{
    return yang_error_wrap(ERROR_RTC_RTCP, "not support");
}

YangRtcpPli::YangRtcpPli(uint32_t sender_ssrc/*= 0*/)
{
    m_header.padding = 0;
    m_header.type = YangRtcpType_psfb;
    m_header.rc = kPLI;
    m_header.version = kRtcpVersion;
    m_ssrc = sender_ssrc;
}

YangRtcpPli::~YangRtcpPli()
{
}

int32_t YangRtcpPli::decode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/rfc4585#section-6.1
        0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|   FMT   |       PT      |          length               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of packet sender                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of media source                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   :            Feedback Control Information (FCI)                 :
   :                                                               :
   */

    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    m_media_ssrc = buffer->read_4bytes();
    return err;
}

uint64_t YangRtcpPli::nb_bytes()
{
    return 12;
}

int32_t YangRtcpPli::encode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/rfc4585#section-6.1
        0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|   FMT   |       PT      |          length               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of packet sender                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of media source                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   :            Feedback Control Information (FCI)                 :
   :                                                               :
    */
    int32_t err = Yang_Ok;
    if(!buffer->require(nb_bytes())) {
        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", nb_bytes());
    }

    m_header.length = 2;
    if(Yang_Ok != (err = encode_header(buffer))) {
        return yang_error_wrap(err, "encode header");
    }

    buffer->write_4bytes(m_media_ssrc);

    return err;
}

YangRtcpSli::YangRtcpSli(uint32_t sender_ssrc/*= 0*/)
{
    m_first = 0;
    m_number = 0;
    m_picture = 0;

    m_header.padding = 0;
    m_header.type = YangRtcpType_psfb;
    m_header.rc = kSLI;
    m_header.version = kRtcpVersion;
    m_ssrc = sender_ssrc;
}

YangRtcpSli::~YangRtcpSli()
{
}


int32_t YangRtcpSli::decode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/rfc4585#section-6.1
        0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|   FMT   |       PT      |          length               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of packet sender                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of media source                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   :            Feedback Control Information (FCI)                 :
   :                                                               :


    @doc: https://tools.ietf.org/html/rfc4585#section-6.3.2
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |            First        |        Number           | PictureID |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   */

    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    m_media_ssrc = buffer->read_4bytes();
    int32_t len = (m_header.length + 1) * 4 - 12;
    buffer->skip(len);
    return err;
}

uint64_t YangRtcpSli::nb_bytes()
{
    return kRtcpPacketSize;
}

int32_t YangRtcpSli::encode(YangBuffer *buffer)
{
    int32_t err = Yang_Ok;

    return err;
}

YangRtcpRpsi::YangRtcpRpsi(uint32_t sender_ssrc/* = 0*/)
{
    m_pb = 0;
    m_payload_type = 0;
    m_native_rpsi = NULL;
    m_nb_native_rpsi = 0;

    m_header.padding = 0;
    m_header.type = YangRtcpType_psfb;
    m_header.rc = kRPSI;
    m_header.version = kRtcpVersion;
    m_ssrc = sender_ssrc;
}

YangRtcpRpsi::~YangRtcpRpsi()
{
}

int32_t YangRtcpRpsi::decode(YangBuffer *buffer)
{
/*
    @doc: https://tools.ietf.org/html/rfc4585#section-6.1
        0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|   FMT   |       PT      |          length               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of packet sender                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of media source                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   :            Feedback Control Information (FCI)                 :
   :                                                               :


    @doc: https://tools.ietf.org/html/rfc4585#section-6.3.3
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |      PB       |0| Payload Type|    Native RPSI bit string     |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |   defined per codec          ...                | Padding (0) |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   */

    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    m_media_ssrc = buffer->read_4bytes();
    int32_t len = (m_header.length + 1) * 4 - 12;
    buffer->skip(len);
    return err;
}

uint64_t YangRtcpRpsi::nb_bytes()
{
    return kRtcpPacketSize;
}

int32_t YangRtcpRpsi::encode(YangBuffer *buffer)
{
    int32_t err = Yang_Ok;

    return err;
}

YangRtcpXr::YangRtcpXr(uint32_t ssrc/*= 0*/)
{
    m_header.padding = 0;
    m_header.type = YangRtcpType_xr;
    m_header.rc = 0;
    m_header.version = kRtcpVersion;
    m_ssrc = ssrc;
}

YangRtcpXr::~YangRtcpXr()
{
}

int32_t YangRtcpXr::decode(YangBuffer *buffer)
{
/*
    @doc: https://tools.ietf.org/html/rfc3611#section-2
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|reserved |   PT=XR=207   |             length            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                              SSRC                             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   :                         report blocks                         :
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   */

    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    int32_t len = (m_header.length + 1) * 4 - 8;
    buffer->skip(len);
    return err;
}

uint64_t YangRtcpXr::nb_bytes()
{
    return kRtcpPacketSize;
}

int32_t YangRtcpXr::encode(YangBuffer *buffer)
{
    return yang_error_wrap(ERROR_RTC_RTCP, "not support");
}

YangRtcpCompound::YangRtcpCompound(): m_nb_bytes(0), m_data(NULL), m_nb_data(0)
{
}

YangRtcpCompound::~YangRtcpCompound()
{
   clear();
}

YangRtcpCommon* YangRtcpCompound::get_next_rtcp()
{
    if(m_rtcps.empty()) {
        return NULL;
    }
    YangRtcpCommon *rtcp = m_rtcps.back();
    m_rtcps.pop_back();
    return rtcp;
}

int32_t YangRtcpCompound::add_rtcp(YangRtcpCommon *rtcp)
{
    int32_t new_len = rtcp->nb_bytes();
    if((new_len + m_nb_bytes) > kRtcpPacketSize) {
        return yang_error_wrap(ERROR_RTC_RTCP, "overflow, new rtcp: %d, current: %d", new_len, m_nb_bytes);
    }
    m_nb_bytes += new_len;
    m_rtcps.push_back(rtcp);

    return Yang_Ok;
}

int32_t YangRtcpCompound::decode(YangBuffer *buffer)
{
    int32_t err = Yang_Ok;
    m_data = buffer->data();
    m_nb_data = buffer->size();

    while (!buffer->empty()) {
        YangRtcpCommon* rtcp = NULL;
        YangRtcpHeader* header = (YangRtcpHeader*)(buffer->head());
        if (header->type == YangRtcpType_sr) {
            rtcp = new YangRtcpSR();
        } else if (header->type == YangRtcpType_rr) {
            rtcp = new YangRtcpRR();
        } else if (header->type == YangRtcpType_rtpfb) {
            if(1 == header->rc) {
                //nack
                rtcp = new YangRtcpNack();
            } else if (15 == header->rc) {
                //twcc
                rtcp = new YangRtcpTWCC();
            }
        } else if(header->type == YangRtcpType_psfb) {
            if(1 == header->rc) {
                // pli
                rtcp = new YangRtcpPli();
            } else if(2 == header->rc) {
                //sli
                rtcp = new YangRtcpSli();
            } else if(3 == header->rc) {
                //rpsi
                rtcp = new YangRtcpRpsi();
            } else {
                // common psfb
                rtcp = new YangRtcpPsfbCommon();
            }
        } else if(header->type == YangRtcpType_xr) {
            rtcp = new YangRtcpXr();
        } else {
            rtcp = new YangRtcpCommon();
        }

        if(Yang_Ok != (err = rtcp->decode(buffer))) {
            yang_freep(rtcp);

            // @doc https://tools.ietf.org/html/rfc3550#section-6.4.2
            // An empty RR packet (RC = 0) MUST be put at the head of a compound
            // RTCP packet when there is no data transmission or reception to
            // report. e.g. {80 c9 00 01 00 00 00 01}
            if (ERROR_RTC_RTCP_EMPTY_RR == err) {
                //srs_freep(err);
                continue;
            }

            return yang_error_wrap(err, "decode rtcp type=%u rc=%u", header->type, header->rc);
        }

        m_rtcps.push_back(rtcp);
    }

    return err;
}

uint64_t YangRtcpCompound::nb_bytes()
{
    return kRtcpPacketSize;
}

int32_t YangRtcpCompound::encode(YangBuffer *buffer)
{
    int32_t err = Yang_Ok;
    if(!buffer->require(m_nb_bytes)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", m_nb_bytes);
    }

    vector<YangRtcpCommon*>::iterator it;
    for(it = m_rtcps.begin(); it != m_rtcps.end(); ++it) {
        YangRtcpCommon *rtcp = *it;
        if((err = rtcp->encode(buffer)) != Yang_Ok) {
            return yang_error_wrap(err, "encode compound type:%d", rtcp->type());
        }
    }

    clear();
    return err;
}

void YangRtcpCompound::clear()
{
    vector<YangRtcpCommon*>::iterator it;
    for(it = m_rtcps.begin(); it != m_rtcps.end(); ++it) {
        YangRtcpCommon *rtcp = *it;
        delete rtcp;
        rtcp = NULL;
    }
    m_rtcps.clear();
    m_nb_bytes = 0;
}

char* YangRtcpCompound::data()
{
    return m_data;
}

int32_t YangRtcpCompound::size()
{
    return m_nb_data;
}
