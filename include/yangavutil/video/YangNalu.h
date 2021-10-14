#ifndef INCLUDE_YANGAVUTIL_VIDEO_YANGNALU_H_
#define INCLUDE_YANGAVUTIL_VIDEO_YANGNALU_H_
#include <stdint.h>
#include <string>
#include <yangutil/yangavtype.h>
struct YangNaluData{
	int spsppsPos;
	int keyframePos;
};

int32_t yang_parseNalue(YangFrame* videoFrame,YangNaluData* pnalu);
int32_t yang_getSpsppseNalu(YangFrame* videoFrame,uint8_t* pnaludata,uint8_t* meta);
int32_t yang_getKeyframeNalu(YangFrame* videoFrame);
bool yang_hasPframe(uint8_t* p);
/**
 * Table 7-1 - NAL unit type codes, syntax element categories, and NAL unit type classes
 * ISO_IEC_14496-10-AVC-2012.pdf, page 83.
 */

std::string yang_avc_nalu2str(YangAvcNaluType nalu_type);

#endif /* INCLUDE_YANGAVUTIL_VIDEO_YANGNALU_H_ */
