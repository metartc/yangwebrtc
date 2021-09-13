#ifndef SRC_YANGRTP_YANGRTPCODEC_H_
#define SRC_YANGRTP_YANGRTPCODEC_H_
#include <string>
#include <stdint.h>
#include <yangutil/buffer/YangBuffer.h>
class YangMediaPayloadType;
/**
 * Table 7-1 - NAL unit type codes, syntax element categories, and NAL unit type classes
 * ISO_IEC_14496-10-AVC-2012.pdf, page 83.
 */
enum YangAvcNaluType
{
    // Unspecified
    YangAvcNaluTypeReserved = 0,
    YangAvcNaluTypeForbidden = 0,

    // Coded slice of a non-IDR picture slice_layer_without_partitioning_rbsp( )
    YangAvcNaluTypeNonIDR = 1,
    // Coded slice data partition A slice_data_partition_a_layer_rbsp( )
    YangAvcNaluTypeDataPartitionA = 2,
    // Coded slice data partition B slice_data_partition_b_layer_rbsp( )
    YangAvcNaluTypeDataPartitionB = 3,
    // Coded slice data partition C slice_data_partition_c_layer_rbsp( )
    YangAvcNaluTypeDataPartitionC = 4,
    // Coded slice of an IDR picture slice_layer_without_partitioning_rbsp( )
    YangAvcNaluTypeIDR = 5,
    // Supplemental enhancement information (SEI) sei_rbsp( )
    YangAvcNaluTypeSEI = 6,
    // Sequence parameter set seq_parameter_set_rbsp( )
    YangAvcNaluTypeSPS = 7,
    // Picture parameter set pic_parameter_set_rbsp( )
    YangAvcNaluTypePPS = 8,
    // Access unit delimiter access_unit_delimiter_rbsp( )
    YangAvcNaluTypeAccessUnitDelimiter = 9,
    // End of sequence end_of_seq_rbsp( )
    YangAvcNaluTypeEOSequence = 10,
    // End of stream end_of_stream_rbsp( )
    YangAvcNaluTypeEOStream = 11,
    // Filler data filler_data_rbsp( )
    YangAvcNaluTypeFilterData = 12,
    // Sequence parameter set extension seq_parameter_set_extension_rbsp( )
    YangAvcNaluTypeSPSExt = 13,
    // Prefix NAL unit prefix_nal_unit_rbsp( )
    YangAvcNaluTypePrefixNALU = 14,
    // Subset sequence parameter set subset_seq_parameter_set_rbsp( )
    YangAvcNaluTypeSubsetSPS = 15,
    // Coded slice of an auxiliary coded picture without partitioning slice_layer_without_partitioning_rbsp( )
    YangAvcNaluTypeLayerWithoutPartition = 19,
    // Coded slice extension slice_layer_extension_rbsp( )
    YangAvcNaluTypeCodedSliceExt = 20,
};
std::string yang_avc_nalu2str(YangAvcNaluType nalu_type);
/**
 * the profile for avc/h.264.
 * @see Annex A Profiles and levels, ISO_IEC_14496-10-AVC-2003.pdf, page 205.
 */
enum YangAvcProfile
{
    YangAvcProfileReserved = 0,

    // @see ffmpeg, libavcodec/avcodec.h:2713
    YangAvcProfileBaseline = 66,
    // FF_PROFILE_H264_CONSTRAINED  (1<<9)  // 8+1; constraint_set1_flag
    // FF_PROFILE_H264_CONSTRAINED_BASELINE (66|FF_PROFILE_H264_CONSTRAINED)
    YangAvcProfileConstrainedBaseline = 578,
    YangAvcProfileMain = 77,
    YangAvcProfileExtended = 88,
    YangAvcProfileHigh = 100,
    YangAvcProfileHigh10 = 110,
    YangAvcProfileHigh10Intra = 2158,
    YangAvcProfileHigh422 = 122,
    YangAvcProfileHigh422Intra = 2170,
    YangAvcProfileHigh444 = 144,
    YangAvcProfileHigh444Predictive = 244,
    YangAvcProfileHigh444Intra = 2192,
};
std::string yang_avc_profile2str(YangAvcProfile profile);

/**
 * the level for avc/h.264.
 * @see Annex A Profiles and levels, ISO_IEC_14496-10-AVC-2003.pdf, page 207.
 */
enum YangAvcLevel
{
    YangAvcLevelReserved = 0,

    YangAvcLevel_1 = 10,
    YangAvcLevel_11 = 11,
    YangAvcLevel_12 = 12,
    YangAvcLevel_13 = 13,
    YangAvcLevel_2 = 20,
    YangAvcLevel_21 = 21,
    YangAvcLevel_22 = 22,
    YangAvcLevel_3 = 30,
    YangAvcLevel_31 = 31,
    YangAvcLevel_32 = 32,
    YangAvcLevel_4 = 40,
    YangAvcLevel_41 = 41,
    YangAvcLevel_5 = 50,
    YangAvcLevel_51 = 51,
};
std::string yang_avc_level2str(YangAvcLevel level);

/**
 * Table 7-6 – Name association to slice_type
 * ISO_IEC_14496-10-AVC-2012.pdf, page 105.
 */
enum YangAvcSliceType
{
    YangAvcSliceTypeP   = 0,
    YangAvcSliceTypeB   = 1,
    YangAvcSliceTypeI   = 2,
    YangAvcSliceTypeSP  = 3,
    YangAvcSliceTypeSI  = 4,
    YangAvcSliceTypeP1  = 5,
    YangAvcSliceTypeB1  = 6,
    YangAvcSliceTypeI1  = 7,
    YangAvcSliceTypeSP1 = 8,
    YangAvcSliceTypeSI1 = 9,
};

/**
 * The frame type, for example, audio, video or data.
 * @doc video_file_format_spec_v10_1.pdf, page 75, E.4.1 FLV Tag
 */
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



class YangMediaPayloadType
{
public:
    YangMediaPayloadType(int32_t payload_type);
    virtual ~YangMediaPayloadType();

    int32_t encode(std::stringstream& os);
public:
    int32_t m_payload_type;

    std::string m_encoding_name;
    int32_t m_clock_rate;
    std::string m_encoding_param;

    std::vector<std::string> m_rtcp_fb;
    std::string m_format_specific_param;
};

// TODO: FIXME: Rename it.
class YangCodecPayload
{
public:
    std::string m_type;
    uint8_t m_pt;
    // for publish, equals to PT of itself;
    // for subscribe, is the PT of publisher;
    uint8_t m_pt_of_publisher;
    std::string m_name;
    int32_t m_sample;

    std::vector<std::string> m_rtcp_fbs;
public:
    YangCodecPayload();
    YangCodecPayload(uint8_t pt, std::string encode_name, int32_t sample);
    virtual ~YangCodecPayload();
public:
    virtual YangCodecPayload* copy();
    virtual YangMediaPayloadType generate_media_payload_type();
};

// TODO: FIXME: Rename it.
class YangVideoPayload : public YangCodecPayload
{
public:
    struct H264SpecificParameter
    {
        std::string profile_level_id;
        std::string packetization_mode;
        std::string level_asymmerty_allow;
    };
    H264SpecificParameter m_h264_param;

public:
    YangVideoPayload();
    YangVideoPayload(uint8_t pt, std::string encode_name, int32_t sample);
    virtual ~YangVideoPayload();
public:
    virtual YangVideoPayload* copy();
    virtual YangMediaPayloadType generate_media_payload_type();
public:
    int32_t set_h264_param_desc(std::string fmtp);
};

// TODO: FIXME: Rename it.
class YangAudioPayload : public YangCodecPayload
{
    struct SrsOpusParameter
    {
        int32_t minptime;
        bool use_inband_fec;
        bool usedtx;

        SrsOpusParameter() {
            minptime = 0;
            use_inband_fec = false;
            usedtx = false;
        }
    };

public:
    int32_t m_channel;
    SrsOpusParameter m_opus_param;
public:
    YangAudioPayload();
    YangAudioPayload(uint8_t pt, std::string encode_name, int32_t sample, int32_t channel);
    virtual ~YangAudioPayload();
public:
    virtual YangAudioPayload* copy();
    virtual YangMediaPayloadType generate_media_payload_type();
public:
    int32_t set_opus_param_desc(std::string fmtp);
};

// TODO: FIXME: Rename it.
class YangRedPayload : public YangCodecPayload
{
public:
    int32_t m_channel;
public:
    YangRedPayload();
    YangRedPayload(uint8_t pt, std::string encode_name, int32_t sample, int32_t channel);
    virtual ~YangRedPayload();
public:
    virtual YangRedPayload* copy();
    virtual YangMediaPayloadType generate_media_payload_type();
};
class YangRtxPayloadDes : public YangCodecPayload
{
public:
    uint8_t apt_;
public:
    YangRtxPayloadDes();
    YangRtxPayloadDes(uint8_t pt, uint8_t apt);
    virtual ~YangRtxPayloadDes();

public:
    virtual YangRtxPayloadDes* copy();
    virtual YangMediaPayloadType generate_media_payload_type();
};

#endif /* SRC_YANGRTP_YANGRTPCODEC_H_ */
