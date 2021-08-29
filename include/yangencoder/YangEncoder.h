#ifndef YANGENCODER_INCLUDE_YANGENCODER_H_
#define YANGENCODER_INCLUDE_YANGENCODER_H_

class YangEncoderCallback{
public:
	YangEncoderCallback(){};
	virtual ~YangEncoderCallback(){};
	virtual void onVideoData(unsigned char* p,int plen,long long timestamp,int pframetype,int puid)=0;
	virtual void onAudioData(unsigned char* p,int plen,int puid)=0;
};




#endif /* YANGENCODER_INCLUDE_YANGENCODER_H_ */
