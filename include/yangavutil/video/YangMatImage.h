/*
 * YangMatImage.h
 *
 *  Created on: 2020年10月29日
 *      Author: yang
 */

#ifndef YANGUTIL_VIDEO_YANGMATIMAGE_H_
#define YANGUTIL_VIDEO_YANGMATIMAGE_H_
#include <stdint.h>

class YangMatImage {
public:
	YangMatImage();
	virtual ~YangMatImage();
	virtual void initImg(char *bgfile,int32_t width,int32_t height,int32_t typeLen=3)=0;
	virtual void matImage(uint8_t* psrc,uint8_t *pdst)=0;
};
class YangMatImageFactory{
public:
	YangMatImageFactory();
	~YangMatImageFactory();
	YangMatImage *getYangMatImage();
};
#endif /* YANGUTIL_VIDEO_YANGMATIMAGE_H_ */
