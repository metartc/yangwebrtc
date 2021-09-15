#include <yangrtp/YangRtpCodec.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangString.h>
#include <string>
#include <string.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>
#include <sstream>

YangVideoPayload::YangVideoPayload()
{
    m_type = "video";
}

YangVideoPayload::YangVideoPayload(uint8_t pt, std::string encode_name, int32_t sample)
    :YangCodecPayload(pt, encode_name, sample)
{
    m_type = "video";
    m_h264_param.profile_level_id = "";
    m_h264_param.packetization_mode = "";
    m_h264_param.level_asymmerty_allow = "";
}

YangVideoPayload::~YangVideoPayload()
{
}

YangVideoPayload* YangVideoPayload::copy()
{
    YangVideoPayload* cp = new YangVideoPayload();

    cp->m_type = m_type;
    cp->m_pt = m_pt;
    cp->m_pt_of_publisher = m_pt_of_publisher;
    cp->m_name = m_name;
    cp->m_sample = m_sample;
    cp->m_rtcp_fbs = m_rtcp_fbs;
    cp->m_h264_param = m_h264_param;

    return cp;
}

YangMediaPayloadType YangVideoPayload::generate_media_payload_type()
{
    YangMediaPayloadType media_payload_type(m_pt);

    media_payload_type.m_encoding_name = m_name;
    media_payload_type.m_clock_rate = m_sample;
    media_payload_type.m_rtcp_fb = m_rtcp_fbs;

    std::ostringstream format_specific_param;
    if (!m_h264_param.level_asymmerty_allow.empty()) {
        format_specific_param << "level-asymmetry-allowed=" << m_h264_param.level_asymmerty_allow;
    }
    if (!m_h264_param.packetization_mode.empty()) {
        format_specific_param << ";packetization-mode=" << m_h264_param.packetization_mode;
    }
    if (!m_h264_param.profile_level_id.empty()) {
        format_specific_param << ";profile-level-id=" << m_h264_param.profile_level_id;
    }

    media_payload_type.m_format_specific_param = format_specific_param.str();

    return media_payload_type;
}

int32_t YangVideoPayload::set_h264_param_desc(std::string fmtp)
{
    int32_t err = Yang_Ok;

    // For example: level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f
    std::vector<std::string> attributes = yang_splits(fmtp, ";");

    for (size_t i = 0; i < attributes.size(); ++i) {
        std::string attribute = attributes.at(i);

        std::vector<std::string> kv = yang_splits(attribute, "=");
        if (kv.size() != 2) {
            return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid h264 param=%s", attribute.c_str());
        }

        if (kv[0] == "profile-level-id") {
            m_h264_param.profile_level_id = kv[1];
        } else if (kv[0] == "packetization-mode") {
            // 6.3.  Non-Interleaved Mode
            // This mode is in use when the value of the OPTIONAL packetization-mode
            // media type parameter is equal to 1.  This mode SHOULD be supported.
            // It is primarily intended for low-delay applications.  Only single NAL
            // unit packets, STAP-As, and FU-As MAY be used in this mode.  STAP-Bs,
            // MTAPs, and FU-Bs MUST NOT be used.  The transmission order of NAL
            // units MUST comply with the NAL unit decoding order.
            // @see https://tools.ietf.org/html/rfc6184#section-6.3
            m_h264_param.packetization_mode = kv[1];
        } else if (kv[0] == "level-asymmetry-allowed") {
            m_h264_param.level_asymmerty_allow = kv[1];
        } else {
            return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid h264 param=%s", kv[0].c_str());
        }
    }

    return err;
}

YangAudioPayload::YangAudioPayload()
{
    m_channel = 0;
}

YangAudioPayload::YangAudioPayload(uint8_t pt, std::string encode_name, int32_t sample, int32_t channel)
    :YangCodecPayload(pt, encode_name, sample)
{
    m_type = "audio";
    m_channel = channel;
    m_opus_param.minptime = 0;
    m_opus_param.use_inband_fec = false;
    m_opus_param.usedtx = false;
}

YangAudioPayload::~YangAudioPayload()
{
}

YangAudioPayload* YangAudioPayload::copy()
{
    YangAudioPayload* cp = new YangAudioPayload();

    cp->m_type = m_type;
    cp->m_pt = m_pt;
    cp->m_pt_of_publisher = m_pt_of_publisher;
    cp->m_name = m_name;
    cp->m_sample = m_sample;
    cp->m_rtcp_fbs = m_rtcp_fbs;
    cp->m_channel = m_channel;
    cp->m_opus_param = m_opus_param;

    return cp;
}

YangMediaPayloadType YangAudioPayload::generate_media_payload_type()
{
    YangMediaPayloadType media_payload_type(m_pt);

    media_payload_type.m_encoding_name = m_name;
    media_payload_type.m_clock_rate = m_sample;
    if (m_channel != 0) {
        media_payload_type.m_encoding_param = yang_int2str(m_channel);
    }
    media_payload_type.m_rtcp_fb = m_rtcp_fbs;

    std::ostringstream format_specific_param;
    if (m_opus_param.minptime) {
        format_specific_param << "minptime=" << m_opus_param.minptime;
    }
    if (m_opus_param.use_inband_fec) {
        format_specific_param << ";useinbandfec=1";
    }
    if (m_opus_param.usedtx) {
        format_specific_param << ";usedtx=1";
    }
    media_payload_type.m_format_specific_param = format_specific_param.str();

    return media_payload_type;
}

int32_t YangAudioPayload::set_opus_param_desc(std::string fmtp)
{
    int32_t err = Yang_Ok;
    std::vector<std::string> vec = yang_splits(fmtp, ";");
    for (size_t i = 0; i < vec.size(); ++i) {
        std::vector<std::string> kv = yang_splits(vec[i], "=");
        if (kv.size() == 2) {
            if (kv[0] == "minptime") {
                m_opus_param.minptime = (int)::atol(kv[1].c_str());
            } else if (kv[0] == "useinbandfec") {
                m_opus_param.use_inband_fec = (kv[1] == "1") ? true : false;
            } else if (kv[0] == "usedtx") {
                m_opus_param.usedtx = (kv[1] == "1") ? true : false;
            }
        } else {
            return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid opus param=%s", vec[i].c_str());
        }
    }

    return err;
}
YangRedPayload::YangRedPayload()
{
    m_channel = 0;
}

YangRedPayload::YangRedPayload(uint8_t pt, std::string encode_name, int32_t sample, int32_t channel)
    :YangCodecPayload(pt, encode_name, sample)
{
    m_channel = channel;
}

YangRedPayload::~YangRedPayload()
{
}

YangRedPayload* YangRedPayload::copy()
{
    YangRedPayload* cp = new YangRedPayload();

    cp->m_type = m_type;
    cp->m_pt = m_pt;
    cp->m_pt_of_publisher = m_pt_of_publisher;
    cp->m_name = m_name;
    cp->m_sample = m_sample;
    cp->m_rtcp_fbs = m_rtcp_fbs;
    cp->m_channel = m_channel;

    return cp;
}

YangMediaPayloadType YangRedPayload::generate_media_payload_type()
{
    YangMediaPayloadType media_payload_type(m_pt);

    media_payload_type.m_encoding_name = m_name;
    media_payload_type.m_clock_rate = m_sample;
    if (m_channel != 0) {
        media_payload_type.m_encoding_param = yang_int2str(m_channel);
    }
    media_payload_type.m_rtcp_fb = m_rtcp_fbs;

    return media_payload_type;
}


YangRtxPayloadDes::YangRtxPayloadDes()
{
}

YangRtxPayloadDes::YangRtxPayloadDes(uint8_t pt, uint8_t apt):YangCodecPayload(pt, "rtx", 8000), apt_(apt)
{
}

YangRtxPayloadDes::~YangRtxPayloadDes()
{
}

YangRtxPayloadDes* YangRtxPayloadDes::copy()
{
    YangRtxPayloadDes* cp = new YangRtxPayloadDes();

    cp->m_type = m_type;
    cp->m_pt = m_pt;
    cp->m_pt_of_publisher = m_pt_of_publisher;
    cp->m_name = m_name;
    cp->m_sample = m_sample;
    cp->m_rtcp_fbs = m_rtcp_fbs;
    cp->apt_ = apt_;

    return cp;
}

YangMediaPayloadType YangRtxPayloadDes::generate_media_payload_type()
{
    YangMediaPayloadType media_payload_type(m_pt);

    media_payload_type.m_encoding_name = m_name;
    media_payload_type.m_clock_rate = m_sample;
    std::ostringstream format_specific_param;
    format_specific_param << "fmtp:" << m_pt << " apt="<< apt_;

    media_payload_type.m_format_specific_param = format_specific_param.str();

    return media_payload_type;
}


YangCodecPayload::YangCodecPayload(uint8_t pt, std::string encode_name, int32_t sample)
{
    m_pt_of_publisher = m_pt = pt;
    m_name = encode_name;
    m_sample = sample;
}

YangCodecPayload::~YangCodecPayload()
{
}

YangCodecPayload* YangCodecPayload::copy()
{
    YangCodecPayload* cp = new YangCodecPayload();

    cp->m_type = m_type;
    cp->m_pt = m_pt;
    cp->m_pt_of_publisher = m_pt_of_publisher;
    cp->m_name = m_name;
    cp->m_sample = m_sample;
    cp->m_rtcp_fbs = m_rtcp_fbs;

    return cp;
}

YangMediaPayloadType YangCodecPayload::generate_media_payload_type()
{
    YangMediaPayloadType media_payload_type(m_pt);

    media_payload_type.m_encoding_name = m_name;
    media_payload_type.m_clock_rate = m_sample;
    media_payload_type.m_rtcp_fb = m_rtcp_fbs;

    return media_payload_type;
}
