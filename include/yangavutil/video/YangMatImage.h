/*
 * YangMatImage.h
 *
 *  Created on: 2020年10月29日
 *      Author: yang
 */

#ifndef YANGUTIL_VIDEO_YANGMATIMAGE_H_
#define YANGUTIL_VIDEO_YANGMATIMAGE_H_

class YangMatImage {
public:
	YangMatImage();
	virtual ~YangMatImage();
	virtual void initImg(char *bgfile,int width,int height,int typeLen=3)=0;
	virtual void matImage(unsigned char* psrc,unsigned char *pdst)=0;
};
class YangMatImageFactory{
public:
	YangMatImageFactory();
	~YangMatImageFactory();
	YangMatImage *getYangMatImage();
};
#endif /* YANGUTIL_VIDEO_YANGMATIMAGE_H_ */
