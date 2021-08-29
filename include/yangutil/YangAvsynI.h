
#ifndef INCLUDE_YANGUTIL_YANGAVSYNI_H_
#define INCLUDE_YANGUTIL_YANGAVSYNI_H_

class YangAvsynI{
public:
	YangAvsynI(){};
	virtual ~YangAvsynI(){};
	virtual int playAudioFrame(long long pts)=0;
	virtual int playVideoFrame(long long pts)=0;
};



#endif /* INCLUDE_YANGUTIL_YANGAVSYNI_H_ */
