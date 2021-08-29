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
	void initImg(char *bgfile,int width,int height,int typeLen=3);
	void matImage(unsigned char* psrc,unsigned char *pdst);
	YangJpegData m_jpgData;
	unsigned char* m_bkData;

private:
	IplImage *m_frame;
	IplImage *  m_hsv;
	IplImage * m_mask;

	int m_imgLen;
	int m_width,m_height;
	void replace_and_blend(unsigned char *pdst);


private:
	unsigned char* m_data;
	YangLoadLib m_lib,m_lib1;
	void loadLib();
	void unloadLib();

	void loadJpeg(char* filename,YangJpegData *jpg);
        YangTurboJpeg jpg;
	YangYuvConvert yuv;
	IplImage*  (*yang_cvCreateImage)( CvSize size, int depth, int channels );
	IplImage*  (*yang_cvCreateImageHeader)( CvSize size, int depth, int channels );
	void  (*yang_cvCvtColor)( const CvArr* src, CvArr* dst, int code );
	void (*yang_cvInRangeS)( const CvArr* src, CvScalar lower,
	                       CvScalar upper, CvArr* dst );
	 IplConvKernel*  (*yang_cvCreateStructuringElementEx)(
	            int cols, int  rows, int  anchor_x, int  anchor_y,
	            int shape, int* values);
	 void  (*yang_cvMorphologyEx)( const CvArr* src, CvArr* dst,
	                              CvArr* temp, IplConvKernel* element,
	                              int operation, int iterations  );
	 void  (*yang_cvErode)( const CvArr* src, CvArr* dst,
	                       IplConvKernel* element ,
	                       int iterations);
	 void (*yang_cvSmooth)( const CvArr* src, CvArr* dst,
	                       int smoothtype ,
	                       int size1 ,
	                       int size2 ,
	                       double sigma1 ,
	                       double sigma2);
	 void  (*yang_cvReleaseImageHeader)( IplImage** image );
         void  (*yang_cvReleaseImage)( IplImage** image );

};

#endif /* YANGRECLIVING_INCLUDE_YANGMATIMAGECV_H_ */
