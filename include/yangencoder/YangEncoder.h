#ifndef YANGENCODER_INCLUDE_YANGENCODER_H_
#define YANGENCODER_INCLUDE_YANGENCODER_H_
#include <stdint.h>
class YangEncoderCallback{
public:
	YangEncoderCallback(){};
	virtual ~YangEncoderCallback(){};
	virtual void onVideoData(YangVideoFrame* pframe)=0;
	virtual void onAudioData(YangAudioFrame* pframe)=0;
};




#endif /* YANGENCODER_INCLUDE_YANGENCODER_H_ */
