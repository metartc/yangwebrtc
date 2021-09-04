#ifndef YANGDECODER_INCLUDE_YANGDECODER_H_
#define YANGDECODER_INCLUDE_YANGDECODER_H_

class YangDecoderCallback{
public:
	YangDecoderCallback(){};
	virtual ~YangDecoderCallback(){};
	virtual void onAudioData(YangAudioFrame* pframe)=0;
	virtual void onVideoData(YangVideoFrame* pframe)=0;
};



#endif /* YANGDECODER_INCLUDE_YANGDECODER_H_ */
