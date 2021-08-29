#ifndef _YangConvert_H__
#define _YangConvert_H__

#include "stdint.h"

//#include "YangResize.h"
#define u8_t unsigned char
class YangConvert{
public:
~YangConvert();
YangConvert();
	//YangResize *mpu;

void zoomIn4(uint8_t *src,uint8_t *dest,int srcWidth,int srcHeight,int destWidth,int destHeigh);
void zoom4Yuy2(uint8_t *src,uint8_t *dest,int srcWidth,int srcHeight,int destWidth,int destHeight);
//void initNv12_resize(int p_srcWidth,int p_srcHeight,int p_dstWidth,int p_dstHeight);
void resize_NV12(uint8_t *src,uint8_t *dest,int srcWidth,int srcHeight,int destWidth,int destHeight);
void resize_Yuy2_NV12(uint8_t *src,uint8_t *dest,int p_srcWidth,int p_srcHeight,int p_destWidth,int p_destHeight);
void nv12_nearest_scale(uint8_t*  src, uint8_t*  dst,int srcWidth,int srcHeight,int dstWidth,int dstHeight) ;
void YUY2toI420(int inWidth, int inHeight, uint8_t *pSrc, uint8_t *pDest);
void YUY2toNV12(int inWidth, int inHeight, uint8_t *pSrc, uint8_t *pDest);
void plusNV12(uint8_t* src,uint8_t * dest,int model,int srcWidth,int srcHeight,int destWidth,int destHeight);
void plusYuy2(uint8_t* src,uint8_t * dest,int model,int srcWidth,int srcHeight,int destWidth,int destHeight);

void RGB24_TO_YV12(unsigned char* yv12,unsigned char* rgb24,int w,int h);
void RGB24_To_I420(  unsigned char *RGBbuf,  unsigned char *YUV,  int width,  int height );
void RGB24_To_NV12(  unsigned char *RGBbuf,  unsigned char *YUV,  int width,  int height );
void YUY2_To_RGB24(unsigned char *YUY2buff,unsigned char *RGBbuff,unsigned long dwSize);
private:
	unsigned char clip255(long v);
};
#endif

