/*
 * YangMatImage.h
 *
 *  Created on: 2020年10月29日
 *      Author: yang
 */

#ifndef YANGRECLIVING_INCLUDE_YANGMATIMAGECV_H_
#define YANGRECLIVING_INCLUDE_YANGMATIMAGECV_H_

//cv.h cxcore.h highgui.h

//#include  "opencv2/opencv.hpp"
#include "opencv/cv.h"
//using namespace cv;


#include "yangavutil/video/YangMatImage.h"
#include <yangavutil/video/YangYuvConvert.h>
#include "yangutil/sys/YangLoadLib.h"

#include <yangavutil/vr/YangTurboJpeg.h>

using namespace std;
class YangMatImageCv:public YangMatImage {
public:
	YangMatImageCv();
	virtual ~YangMatImageCv();
	void initImg(char *bgfile,int32_t width,int32_t height,int32_t typeLen=3);
	void matImage(uint8_t* psrc,uint8_t *pdst);
	YangJpegData m_jpgData;
	uint8_t* m_bkData;

private:
	IplImage *m_frame;
	IplImage *  m_hsv;
	IplImage * m_mask;

	int32_t m_imgLen;
	int32_t m_width,m_height;
	void replace_and_blend(uint8_t *pdst);


private:
	uint8_t* m_data;
	YangLoadLib m_lib,m_lib1;
	void loadLib();
	void unloadLib();

	void loadJpeg(char* filename,YangJpegData *jpg);
        YangTurboJpeg jpg;
	YangYuvConvert yuv;
	IplImage*  (*yang_cvCreateImage)( CvSize size, int32_t depth, int32_t channels );
	IplImage*  (*yang_cvCreateImageHeader)( CvSize size, int32_t depth, int32_t channels );
	void  (*yang_cvCvtColor)( const CvArr* src, CvArr* dst, int32_t code );
	void (*yang_cvInRangeS)( const CvArr* src, CvScalar lower,
	                       CvScalar upper, CvArr* dst );
	 IplConvKernel*  (*yang_cvCreateStructuringElementEx)(
	            int32_t cols, int32_t  rows, int32_t  anchor_x, int32_t  anchor_y,
	            int32_t shape, int* values);
	 void  (*yang_cvMorphologyEx)( const CvArr* src, CvArr* dst,
	                              CvArr* temp, IplConvKernel* element,
	                              int32_t operation, int32_t iterations  );
	 void  (*yang_cvErode)( const CvArr* src, CvArr* dst,
	                       IplConvKernel* element ,
	                       int32_t iterations);
	 void (*yang_cvSmooth)( const CvArr* src, CvArr* dst,
	                       int32_t smoothtype ,
	                       int32_t size1 ,
	                       int32_t size2 ,
	                       double sigma1 ,
	                       double sigma2);
	 void  (*yang_cvReleaseImageHeader)( IplImage** image );
         void  (*yang_cvReleaseImage)( IplImage** image );

};

#endif /* YANGRECLIVING_INCLUDE_YANGMATIMAGECV_H_ */
