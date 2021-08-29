/*
 * YangYuvUtil.h
 *
 *  Created on: 2019年8月25日
 *      Author: yang
 */

#ifndef INCLUDE_YANGAVUTIL_VIDEO_YANGYUVUTIL_H_
#define INCLUDE_YANGAVUTIL_VIDEO_YANGYUVUTIL_H_
#include "stdint.h"
	void yang_plusNV12(uint8_t* src,uint8_t * dest,int model,int srcWidth,int srcHeight,int destWidth,int destHeight);
	void yang_plusI420(uint8_t* src,uint8_t * dest,int model,int srcWidth,int srcHeight,int destWidth,int destHeight);
	void yang_plusYuy2(uint8_t* src,uint8_t * dest,int model,int srcWidth,int srcHeight,int destWidth,int destHeight);
	void yang_rgbtobgr(uint8_t *rgb,uint8_t *bgr,int srcWidth,int srcHeight);
	void yang_zoom4(uint8_t* src,uint8_t* dst,int wid,int hei);


#endif /* INCLUDE_YANGAVUTIL_VIDEO_YANGYUVUTIL_H_ */
