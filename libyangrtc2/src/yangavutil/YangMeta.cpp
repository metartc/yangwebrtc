#include <yangavutil/video/YangMeta.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>
#include <memory.h>
void yang_createH264Meta(YangVideoMeta* pvmd,YangFrame *videoFrame){
	memset(pvmd,0,sizeof(YangVideoMeta));
	uint32_t spsLen=yang_get_be32(videoFrame->payload);
	uint32_t ppslen=yang_get_be32(videoFrame->payload+spsLen+4);
	pvmd->mp4Meta.spsLen=spsLen;
	pvmd->mp4Meta.ppsLen=ppslen;
	memcpy(pvmd->mp4Meta.sps,videoFrame->payload+4,spsLen);
	memcpy(pvmd->mp4Meta.pps,videoFrame->payload+4+spsLen+4,ppslen);

}
void yang_createH265Meta(YangVideoMeta* pvmd,YangFrame *videoFrame){
	memset(pvmd,0,sizeof(YangVideoMeta));
	uint32_t spsLen=yang_get_be32(videoFrame->payload);
	uint32_t ppslen=yang_get_be32(videoFrame->payload+spsLen+4);
	pvmd->mp4Meta.spsLen=spsLen;
	pvmd->mp4Meta.ppsLen=ppslen;
	memcpy(pvmd->mp4Meta.sps,videoFrame->payload+4,spsLen);
	memcpy(pvmd->mp4Meta.pps,videoFrame->payload+4+spsLen+4,ppslen);

}

