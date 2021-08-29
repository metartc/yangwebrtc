/*
 * YangAudioMix.h
 *
 *  Created on: 2020年9月25日
 *      Author: yang
 */

#ifndef YANGUTIL_AUDIO_YANGAUDIOMIX_H_
#define YANGUTIL_AUDIO_YANGAUDIOMIX_H_
#define SIZE_AUDIO_FRAME 4096
class YangAudioMix {
public:
	YangAudioMix();
	virtual ~YangAudioMix();
	void yangMix4(unsigned char *dst, unsigned char *src, int len,int pvolume);
	void yangMix3(unsigned char *dst, unsigned char *src, int len,int pvolume);
	//void yangMix(unsigned char *sourseFile[SIZE_AUDIO_FRAME], int number,	unsigned char *objectFile);
	//void yangMixs1(unsigned char *dst, unsigned char *src, int len, int volume);
	void yangMix1(unsigned char *dst, unsigned char *src, int len, int volume);
	void yangMix1(short *dst, short *src, int len, int volume);
	void yangMix2(unsigned char *pin, unsigned char *pout,int len,int pvolume);
private:
	int  m_yangAudioMAX;
	int  m_yangAudioMIN ;
};

#endif /* YANGUTIL_AUDIO_YANGAUDIOMIX_H_ */
