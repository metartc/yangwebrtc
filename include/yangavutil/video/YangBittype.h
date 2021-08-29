#ifndef YANGbittype_H_
#define YANGbittype_H_
#include "stdint.h"
#define Yang_QP_MAX_NUM (51 + 6*6)
typedef struct YangGetBitContext {
	uint8_t *buffer, *buffer_end;
	int index;
	int size_in_bits;
	int size_in_bits_plus8;
} YangGetBitContext;
enum YangPictureType {
    Yang_PICTURE_TYPE_NONE = 0, ///< Undefined
    Yang_PICTURE_TYPE_I,     ///< Intra
    Yang_PICTURE_TYPE_P,     ///< Predicted
    Yang_PICTURE_TYPE_B,     ///< Bi-dir predicted
    Yang_PICTURE_TYPE_S,     ///< S(GMC)-VOP MPEG-4
    Yang_PICTURE_TYPE_SI,    ///< Switching Intra
    Yang_PICTURE_TYPE_SP,    ///< Switching Predicted
    Yang_PICTURE_TYPE_BI,    ///< BI type
};
typedef struct YangPredWeightTable {
    int use_weight;
    int use_weight_chroma;
    int luma_log2_weight_denom;
    int chroma_log2_weight_denom;
    int luma_weight_flag[2];    ///< 7.4.3.2 luma_weight_lX_flag
    int chroma_weight_flag[2];  ///< 7.4.3.2 chroma_weight_lX_flag
    // The following 2 can be changed to int8_t but that causes a 10 CPU cycles speed loss
    int luma_weight[48][2][2];
    int chroma_weight[48][2][2][2];
    int implicit_weight[48][48][2];
} YangPredWeightTable;
typedef struct YangNAL {
    uint8_t *rbsp_buffer;

    int size;
    uint8_t *data;

    /**
     * Size, in bits, of just the data, excluding the stop bit and any trailing
     * padding. I.e. what HEVC calls SODB.
     */
    int size_bits;

    int raw_size;
     uint8_t *raw_data;

    YangGetBitContext gb;

    /**
     * NAL unit type
     */
    int type;

    /**
     * HEVC only, nuh_temporal_id_plus_1 - 1
     */
    int temporal_id;

    int skipped_bytes;
    int skipped_bytes_pos_size;
    int *skipped_bytes_pos;
    /**
     * H.264 only, nal_ref_idc
     */
    int ref_idc;
} YangNAL;

typedef struct YangRBSP {
    uint8_t *rbsp_buffer;
    int rbsp_buffer_alloc_size;
    int rbsp_buffer_size;
} YangRBSP;

/* an input packet split into unescaped NAL units */
typedef struct YangPacket {
    YangNAL *nals;
    YangRBSP rbsp;
    int nb_nals;
    int nals_allocated;
} YangPacket;
typedef struct YangRational{
    int num; ///< Numerator
    int den; ///< Denominator
} YangRational;
typedef enum Yang_MMCOOpcode {
    Yang_MMCO_END = 0,
    Yang_MMCO_SHORT2UNUSED,
    Yang_MMCO_LONG2UNUSED,
    Yang_MMCO_SHORT2LONG,
    Yang_MMCO_SET_MAX_LONG,
    Yang_MMCO_RESET,
    Yang_MMCO_LONG,
} Yang_MMCOOpcode;
typedef struct YangMMCO {
    Yang_MMCOOpcode opcode;
    int short_pic_num;  ///< pic_num without wrapping (pic_num & max_pic_num)
    int long_arg;       ///< index, pic_num, or num long refs depending on opcode
} YangMMCO;

enum YangColorTransferCharacteristic {
    Yang_TRC_RESERVED0    = 0,
    Yang_TRC_BT709        = 1,  ///< also ITU-R BT1361
    Yang_TRC_UNSPECIFIED  = 2,
    Yang_TRC_RESERVED     = 3,
    Yang_TRC_GAMMA22      = 4,  ///< also ITU-R BT470M / ITU-R BT1700 625 PAL & SECAM
    Yang_TRC_GAMMA28      = 5,  ///< also ITU-R BT470BG
    Yang_TRC_SMPTE170M    = 6,  ///< also ITU-R BT601-6 525 or 625 / ITU-R BT1358 525 or 625 / ITU-R BT1700 NTSC
    Yang_TRC_SMPTE240M    = 7,
    Yang_TRC_LINEAR       = 8,  ///< "Linear transfer characteristics"
    Yang_TRC_LOG          = 9,  ///< "Logarithmic transfer characteristic (100:1 range)"
    Yang_TRC_LOG_SQRT     = 10, ///< "Logarithmic transfer characteristic (100 * Sqrt(10) : 1 range)"
    Yang_TRC_IEC61966_2_4 = 11, ///< IEC 61966-2-4
    Yang_TRC_BT1361_ECG   = 12, ///< ITU-R BT1361 Extended Colour Gamut
    Yang_TRC_IEC61966_2_1 = 13, ///< IEC 61966-2-1 (sRGB or sYCC)
    Yang_TRC_BT2020_10    = 14, ///< ITU-R BT2020 for 10-bit system
    Yang_TRC_BT2020_12    = 15, ///< ITU-R BT2020 for 12-bit system
    Yang_TRC_SMPTE2084    = 16, ///< SMPTE ST 2084 for 10-, 12-, 14- and 16-bit systems
    Yang_TRC_SMPTEST2084  = Yang_TRC_SMPTE2084,
    Yang_TRC_SMPTE428     = 17, ///< SMPTE ST 428-1
    Yang_TRC_SMPTEST428_1 = Yang_TRC_SMPTE428,
    Yang_TRC_ARIB_STD_B67 = 18, ///< ARIB STD-B67, known as "Hybrid log-gamma"
    Yang_TRC_NB                 ///< Not part of ABI
};
enum YangColorPrimaries {
    Yang_PRI_RESERVED0   = 0,
    Yang_PRI_BT709       = 1,  ///< also ITU-R BT1361 / IEC 61966-2-4 / SMPTE RP177 Annex B
    Yang_PRI_UNSPECIFIED = 2,
    Yang_PRI_RESERVED    = 3,
    Yang_PRI_BT470M      = 4,  ///< also FCC Title 47 Code of Federal Regulations 73.682 (a)(20)

    Yang_PRI_BT470BG     = 5,  ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM
    Yang_PRI_SMPTE170M   = 6,  ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC
    Yang_PRI_SMPTE240M   = 7,  ///< functionally identical to above
    Yang_PRI_FILM        = 8,  ///< colour filters using Illuminant C
    Yang_PRI_BT2020      = 9,  ///< ITU-R BT2020
    Yang_PRI_SMPTE428    = 10, ///< SMPTE ST 428-1 (CIE 1931 XYZ)
    Yang_PRI_SMPTEST428_1 = Yang_PRI_SMPTE428,
    Yang_PRI_SMPTE431    = 11, ///< SMPTE ST 431-2 (2011) / DCI P3
    Yang_PRI_SMPTE432    = 12, ///< SMPTE ST 432-1 (2010) / P3 D65 / Display P3
    Yang_PRI_JEDEC_P22   = 22, ///< JEDEC P22 phosphors
    Yang_PRI_NB                ///< Not part of ABI
};
enum YangColorSpace {
    Yang_SPC_RGB         = 0,  ///< order of coefficients is actually GBR, also IEC 61966-2-1 (sRGB)
    Yang_SPC_BT709       = 1,  ///< also ITU-R BT1361 / IEC 61966-2-4 xvYCC709 / SMPTE RP177 Annex B
    Yang_SPC_UNSPECIFIED = 2,
    Yang_SPC_RESERVED    = 3,
    Yang_SPC_FCC         = 4,  ///< FCC Title 47 Code of Federal Regulations 73.682 (a)(20)
    Yang_SPC_BT470BG     = 5,  ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM / IEC 61966-2-4 xvYCC601
    Yang_SPC_SMPTE170M   = 6,  ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC
    Yang_SPC_SMPTE240M   = 7,  ///< functionally identical to above
    Yang_SPC_YCGCO       = 8,  ///< Used by Dirac / VC-2 and H.264 FRext, see ITU-T SG16
    Yang_SPC_YCOCG       = Yang_SPC_YCGCO,
    Yang_SPC_BT2020_NCL  = 9,  ///< ITU-R BT2020 non-constant luminance system
    Yang_SPC_BT2020_CL   = 10, ///< ITU-R BT2020 constant luminance system
    Yang_SPC_SMPTE2085   = 11, ///< SMPTE 2085, Y'D'zD'x
    Yang_SPC_CHROMA_DERIVED_NCL = 12, ///< Chromaticity-derived non-constant luminance system
    Yang_SPC_CHROMA_DERIVED_CL = 13, ///< Chromaticity-derived constant luminance system
    Yang_SPC_ICTCP       = 14, ///< ITU-R BT.2100-0, ICtCp
    Yang_SPC_NB                ///< Not part of ABI
};
typedef struct Yang_SPS {
    unsigned int sps_id;
    int profile_idc;
    int level_idc;
    int chroma_format_idc;
    int transform_bypass;              ///< qpprime_y_zero_transform_bypass_flag
    int log2_max_frame_num;            ///< log2_max_frame_num_minus4 + 4
    int poc_type;                      ///< pic_order_cnt_type
    int log2_max_poc_lsb;              ///< log2_max_pic_order_cnt_lsb_minus4
    int delta_pic_order_always_zero_flag;
    int offset_for_non_ref_pic;
    int offset_for_top_to_bottom_field;
    int poc_cycle_length;              ///< num_ref_frames_in_pic_order_cnt_cycle
    int ref_frame_count;               ///< num_ref_frames
    int gaps_in_frame_num_allowed_flag;
    int mb_width;                      ///< pic_width_in_mbs_minus1 + 1
    ///< (pic_height_in_map_units_minus1 + 1) * (2 - frame_mbs_only_flag)
    int mb_height;
    int frame_mbs_only_flag;
    int mb_aff;                        ///< mb_adaptive_frame_field_flag
    int direct_8x8_inference_flag;
    int crop;                          ///< frame_cropping_flag

    /* those 4 are already in luma samples */
    unsigned int crop_left;            ///< frame_cropping_rect_left_offset
    unsigned int crop_right;           ///< frame_cropping_rect_right_offset
    unsigned int crop_top;             ///< frame_cropping_rect_top_offset
    unsigned int crop_bottom;          ///< frame_cropping_rect_bottom_offset
    int vui_parameters_present_flag;
    YangRational sar;
    int video_signal_type_present_flag;
    int full_range;
    int colour_description_present_flag;
    //enum YangorPrimaries
	int color_primaries;
   //enum YangorTransferCharacteristic
	int color_trc;
    //enum YangorSpace
	int colorspace;
    int timing_info_present_flag;
    uint32_t num_units_in_tick;
    uint32_t time_scale;
    int fixed_frame_rate_flag;
    short offset_for_ref_frame[256]; // FIXME dyn aloc?
    int bitstream_restriction_flag;
    int num_reorder_frames;
    int scaling_matrix_present;
    uint8_t scaling_matrix4[6][16];
    uint8_t scaling_matrix8[6][64];
    int nal_hrd_parameters_present_flag;
    int vcl_hrd_parameters_present_flag;
    int pic_struct_present_flag;
    int time_offset_length;
    int cpb_cnt;                          ///< See H.264 E.1.2
    int initial_cpb_removal_delay_length; ///< initial_cpb_removal_delay_length_minus1 + 1
    int cpb_removal_delay_length;         ///< cpb_removal_delay_length_minus1 + 1
    int dpb_output_delay_length;          ///< dpb_output_delay_length_minus1 + 1
    int bit_depth_luma;                   ///< bit_depth_luma_minus8 + 8
    int bit_depth_chroma;                 ///< bit_depth_chroma_minus8 + 8
    int residual_color_transform_flag;    ///< residual_colour_transform_flag
    int constraint_set_flags;             ///< constraint_set[0-3]_flag
   // uint8_t data[4096];
   // size_t data_size;
} Yang_SPS;

/**
 * Picture parameter set
 */
typedef struct Yang_PPS {
    unsigned int sps_id;
    int cabac;                  ///< entropy_coding_mode_flag
    int pic_order_present;      ///< pic_order_present_flag
    int slice_group_count;      ///< num_slice_groups_minus1 + 1
    int mb_slice_group_map_type;
    unsigned int ref_count[2];  ///< num_ref_idx_l0/1_active_minus1 + 1
    int weighted_pred;          ///< weighted_pred_flag
    int weighted_bipred_idc;
    int init_qp;                ///< pic_init_qp_minus26 + 26
    int init_qs;                ///< pic_init_qs_minus26 + 26
    int chroma_qp_index_offset[2];
    int deblocking_filter_parameters_present; ///< deblocking_filter_parameters_present_flag
    int constrained_intra_pred;     ///< constrained_intra_pred_flag
    int redundant_pic_cnt_present;  ///< redundant_pic_cnt_present_flag
    int transform_8x8_mode;         ///< transform_8x8_mode_flag
    uint8_t scaling_matrix4[6][16];
    uint8_t scaling_matrix8[6][64];
    uint8_t chroma_qp_table[2][Yang_QP_MAX_NUM+1];  ///< pre-scaled (with chroma_qp_index_offset) version of qp_table
    int chroma_qp_diff;
    //uint8_t data[4096];
   // size_t data_size;

    uint32_t dequant4_buffer[6][Yang_QP_MAX_NUM + 1][16];
    uint32_t dequant8_buffer[6][Yang_QP_MAX_NUM + 1][64];
    uint32_t(*dequant4_coeff[6])[16];
    uint32_t(*dequant8_coeff[6])[64];
} Yang_PPS;
#endif
