#include <yangstream/YangStreamFactory.h>
#include "YangStreamRtmp.h"
#include "YangStreamSrt.h"
#include "YangStreamRtc.h"
YangStreamFactory::YangStreamFactory() {


}

YangStreamFactory::~YangStreamFactory() {

}
YangStreamHandle* YangStreamFactory::createStreamHandle(int32_t transType,int32_t puid){
	if(transType==Yang_Rtmp)		return new YangStreamRtmp(puid);
	if(transType==Yang_Srt)		return new YangStreamSrt(puid);

		return new YangStreamRtc(puid);

}

