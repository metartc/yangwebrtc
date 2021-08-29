/*
 * YangYuvUtil.h
 *
 *  Created on: 2020年10月8日
 *      Author: yang
 */

#ifndef YangYuvConvert_H_
#define YangYuvConvert_H_
//#include "yangutil/yangtype.h"
#include "stdint.h"
#include "yangutil/sys/YangLoadLib.h"
#include "libyuv.h"
using namespace libyuv;
class YangYuvConvert {
public:
	YangYuvConvert();
	virtual ~YangYuvConvert();
	//void rgbtonv12();
	int yuy2tonv12(uint8_t* src,uint8_t *dst,int srcWidth,int srcHeiht);
	int yuy2toi420(uint8_t* src,uint8_t *dst,int srcWidth,int srcHeiht);
	int i420tonv12(uint8_t* src,uint8_t *dst,int srcWidth,int srcHeiht);
	int i420tonv21(uint8_t* src,uint8_t *dst,int srcWidth,int srcHeiht);
	int bgr24toyuy2(uint8_t* src,uint8_t *dst,int srcWidth,int srcHeiht);
	int rgb24toI420(uint8_t* src_rgb24,uint8_t *dst,int width,int height);

	//I420ToRGB24
	int I420torgb24(uint8_t* src_rgb24,uint8_t *dst,int width,int height);
	int nv12torgb24(uint8_t* src_rgb24,uint8_t *dst,int width,int height);
	int nv21torgb24(uint8_t* src_rgb24,uint8_t *dst,int width,int height);
	int argbtorgb24(uint8_t* src_argb, uint8_t *dst,int width,int height);
	int rgb24toargb(uint8_t *src_rgb24, uint8_t *dst, int width,int height);
	int scaleNv12(uint8_t* src,uint8_t *dst,int srcWidth,int srcHeight,int dstWidth,int dstHeight,int mode=2);
	int scaleI420(uint8_t* src,uint8_t *dst,int srcWidth,int srcHeight,int dstWidth,int dstHeight,int mode=2);
	int scaleYuy2(uint8_t* src,uint8_t *dst,int srcWidth,int srcHeight,int dstWidth,int dstHeight,int mode=2);
	int scaleRgb(uint8_t* src,uint8_t *dst,int srcWidth,int srcHeight,int dstWidth,int dstHeight,int mode=2);
	int scaleArgb(uint8_t* src,uint8_t *dst,int srcWidth,int srcHeight,int dstWidth,int dstHeight,int mode=2);


private:
	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
	//I420ToNV12
	int (*yang_YUY2ToNV12)(const uint8_t* src_yuy2,
	               int src_stride_yuy2,
	               uint8_t* dst_y,
	               int dst_stride_y,
	               uint8_t* dst_uv,
	               int dst_stride_uv,
	               int width,
	               int height);
	int (*yang_YUY2ToI420)(const uint8_t* src_yuy2,
	               int src_stride_yuy2,
	               uint8_t* dst_y,
	               int dst_stride_y,
	               uint8_t* dst_u,
	               int dst_stride_u,
	               uint8_t* dst_v,
	               int dst_stride_v,
	               int width,
	               int height);

	int (*yang_I420ToNV12)(const uint8_t* src_y,
	               int src_stride_y,
	               const uint8_t* src_u,
	               int src_stride_u,
	               const uint8_t* src_v,
	               int src_stride_v,
	               uint8_t* dst_y,
	               int dst_stride_y,
	               uint8_t* dst_uv,
	               int dst_stride_uv,
	               int width,
	               int height);

	int (*yang_I420ToNV21)(const uint8_t* src_y,
	               int src_stride_y,
	               const uint8_t* src_u,
	               int src_stride_u,
	               const uint8_t* src_v,
	               int src_stride_v,
	               uint8_t* dst_y,
	               int dst_stride_y,
	               uint8_t* dst_vu,
	               int dst_stride_vu,
	               int width,
	               int height);

	int (*yang_I420ToRGB24)(const uint8_t* src_y,
	                int src_stride_y,
	                const uint8_t* src_u,
	                int src_stride_u,
	                const uint8_t* src_v,
	                int src_stride_v,
	                uint8_t* dst_rgb24,
	                int dst_stride_rgb24,
	                int width,
	                int height);
	int (*yang_NV12ToRGB24)(const uint8_t* src_y,
	                int src_stride_y,
	                const uint8_t* src_uv,
	                int src_stride_uv,
	                uint8_t* dst_rgb24,
	                int dst_stride_rgb24,
	                int width,
	                int height);
	int (*yang_NV21ToRGB24)(const uint8_t* src_y,
	                int src_stride_y,
	                const uint8_t* src_vu,
	                int src_stride_vu,
	                uint8_t* dst_rgb24,
	                int dst_stride_rgb24,
	                int width,
	                int height);
	int (*yang_ARGBToRGB24)(const uint8_t* src_argb,
	                int src_stride_argb,
	                uint8_t* dst_rgb24,
	                int dst_stride_rgb24,
	                int width,
	                int height);
	int (*yang_RGB24ToARGB)(const uint8_t* src_rgb24,
	                int src_stride_rgb24,
	                uint8_t* dst_argb,
	                int dst_stride_argb,
	                int width,
	                int height);
	int (*yang_RAWToARGB)(const uint8_t* src_raw,
	              int src_stride_raw,
	              uint8_t* dst_argb,
	              int dst_stride_argb,
	              int width,
	              int height);
	int (*yang_RGB24ToI420)(const uint8_t* src_rgb24,
	                int src_stride_rgb24,
	                uint8_t* dst_y,
	                int dst_stride_y,
	                uint8_t* dst_u,
	                int dst_stride_u,
	                uint8_t* dst_v,
	                int dst_stride_v,
	                int width,
	                int height);
	int (*yang_NV12Scale)(const uint8_t* src_y,
	              int src_stride_y,
	              const uint8_t* src_uv,
	              int src_stride_uv,
	              int src_width,
	              int src_height,
	              uint8_t* dst_y,
	              int dst_stride_y,
	              uint8_t* dst_uv,
	              int dst_stride_uv,
	              int dst_width,
	              int dst_height,
	              enum FilterMode filtering);
	int (*yang_I420Scale)(const uint8_t* src_y,
	              int src_stride_y,
	              const uint8_t* src_u,
	              int src_stride_u,
	              const uint8_t* src_v,
	              int src_stride_v,
	              int src_width,
	              int src_height,
	              uint8_t* dst_y,
	              int dst_stride_y,
	              uint8_t* dst_u,
	              int dst_stride_u,
	              uint8_t* dst_v,
	              int dst_stride_v,
	              int dst_width,
	              int dst_height,
	              enum FilterMode filtering);
	void (*yang_ScalePlane)(const uint8_t* src,
	                int src_stride,
	                int src_width,
	                int src_height,
	                uint8_t* dst,
	                int dst_stride,
	                int dst_width,
	                int dst_height,
	                enum FilterMode filtering);
	int (*yang_ARGBScale)(const uint8_t* src_argb,
	              int src_stride_argb,
	              int src_width,
	              int src_height,
	              uint8_t* dst_argb,
	              int dst_stride_argb,
	              int dst_width,
	              int dst_height,
	              enum FilterMode filtering);
};

#endif /* YANGYUVUTIL_H_ */
