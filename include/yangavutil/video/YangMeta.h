#ifndef INCLUDE_YANGAVUTIL_VIDEO_YANGMETA_H_
#define INCLUDE_YANGAVUTIL_VIDEO_YANGMETA_H_
#include <stdint.h>
#include <string>
#include <yangutil/yangavtype.h>
void yang_createH264Meta(YangVideoMeta* pvmd,YangFrame *videoFrame);
void yang_createH265Meta(YangVideoMeta* pvmd,YangFrame *videoFrame);


#endif /* INCLUDE_YANGAVUTIL_VIDEO_YANGMETA_H_ */
