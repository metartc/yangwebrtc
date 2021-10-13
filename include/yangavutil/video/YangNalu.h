#ifndef INCLUDE_YANGAVUTIL_VIDEO_YANGNALU_H_
#define INCLUDE_YANGAVUTIL_VIDEO_YANGNALU_H_
#include <stdint.h>
#include <yangutil/yangavtype.h>
struct YangNaluData{
	int spsppsPos;
	int keyframePos;
};
int yang_parseNalue(YangFrame* videoFrame,YangNaluData* pnalu);
int32_t yang_getSpsppseNalu(YangFrame* videoFrame,uint8_t* pnaludata,uint8_t* meta);
int yang_getKeyframeNalu(YangFrame* videoFrame);
class YangNalu {
public:
	YangNalu();
	virtual ~YangNalu();
};

#endif /* INCLUDE_YANGAVUTIL_VIDEO_YANGNALU_H_ */
