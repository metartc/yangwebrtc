#ifndef YANGDECODER_INCLUDE_YANGDECODER_H_
#define YANGDECODER_INCLUDE_YANGDECODER_H_

class YangDecoderCallback{
public:
	YangDecoderCallback(){};
	virtual ~YangDecoderCallback(){};
	virtual void onMediaData(unsigned char* p,int plen,long long timestamp,int puid)=0;
};



#endif /* YANGDECODER_INCLUDE_YANGDECODER_H_ */
