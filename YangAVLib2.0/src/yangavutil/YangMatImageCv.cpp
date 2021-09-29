/*
 * YangMatImage.cpp
 *
 *  Created on: 2020年10月29日
 *      Author: yang
 */

#include "yangavutil/vr/YangMatImageCv.h"
#include "yangavutil/video/YangYuvConvert.h"
#include "yangavutil/video/YangYuvUtil.h"


YangMatImageFactory::YangMatImageFactory() {

}
YangMatImageFactory::~YangMatImageFactory() {

}

YangMatImage* YangMatImageFactory::getYangMatImage() {
	return new YangMatImageCv();
}

void YangMatImageCv::loadLib() {
    yang_cvCreateImage =(IplImage* (*)(CvSize size, int32_t depth, int32_t channels)) m_lib.loadFunction("cvCreateImage");
	yang_cvCreateImageHeader = (IplImage* (*)(CvSize size, int32_t depth,
			int32_t channels)) m_lib.loadFunction("cvCreateImageHeader");
	yang_cvReleaseImageHeader = (void (*)(IplImage **image)) m_lib.loadFunction("cvReleaseImageHeader");
	yang_cvReleaseImage = (void (*)(IplImage **image)) m_lib.loadFunction("cvReleaseImage");
	yang_cvInRangeS = (void (*)(const CvArr *src, CvScalar lower,
			CvScalar upper, CvArr *dst)) m_lib.loadFunction("cvInRangeS");

	yang_cvCvtColor =(void (*)(const CvArr *src, CvArr *dst, int32_t code)) m_lib1.loadFunction("cvCvtColor");

	yang_cvCreateStructuringElementEx =(IplConvKernel* (*)(int32_t cols, int32_t rows, int32_t anchor_x, int32_t anchor_y,
					int32_t shape, int32_t *values)) m_lib1.loadFunction(
					"cvCreateStructuringElementEx");
	yang_cvMorphologyEx =(void (*)(const CvArr *src, CvArr *dst, CvArr *temp,
					IplConvKernel *element, int32_t operation, int32_t iterations)) m_lib1.loadFunction(
					"cvMorphologyEx");
	yang_cvErode = (void (*)(const CvArr *src, CvArr *dst,
			IplConvKernel *element, int32_t iterations)) m_lib1.loadFunction("cvErode");
	yang_cvSmooth =(void (*)(const CvArr *src, CvArr *dst, int32_t smoothtype, int32_t size1,
					int32_t size2, double sigma1, double sigma2)) m_lib1.loadFunction("cvSmooth");


}

void YangMatImageCv::unloadLib() {

	yang_cvCreateImageHeader = NULL;
	yang_cvReleaseImageHeader = NULL;
	yang_cvReleaseImage = NULL;
	yang_cvInRangeS = NULL;
	yang_cvCvtColor = NULL;
	yang_cvCreateStructuringElementEx = NULL;
	yang_cvMorphologyEx = NULL;
	yang_cvErode = NULL;
	yang_cvSmooth = NULL;
//#endif

}
YangMatImageCv::YangMatImageCv() {
	m_imgLen = 0;
	m_width = 0, m_height = 0;
	m_bkData = NULL;

	//background_02=NULL;
	m_frame = NULL;
	m_hsv = NULL;
	m_mask = NULL;
	memset(&m_jpgData, 0, sizeof(m_jpgData));

	unloadLib();
	m_data = NULL;
}

YangMatImageCv::~YangMatImageCv() {

//	m_lib2.unloadObject();
	//yang_cvReleaseImageHeader(&background_02);
	if (m_data) {
		delete[] m_data;
		m_data = NULL;
	}

	yang_cvReleaseImage(&m_frame);
	//yang_cvReleaseImageHeader(&m_hsv);
	yang_cvReleaseImage(&m_hsv);
	//yang_cvReleaseImageHeader(&m_mask);
	yang_cvReleaseImage(&m_mask);
	//background_02=NULL;
	if (m_bkData)
		delete[] m_bkData;
	m_bkData = NULL;
	m_frame = NULL;
	m_hsv = NULL;
	m_mask = NULL;

	if (m_jpgData.data)
		jpg.freeJpeg(&m_jpgData);
	unloadLib();
	m_lib.unloadObject();
	m_lib1.unloadObject();
}
void YangMatImageCv::initImg(char *bgfile, int32_t pwidth, int32_t pheight,
		int32_t ptypeLen) {

    m_lib.loadObject("libopencv_core");
	m_lib1.loadObject("libopencv_imgproc");

	loadLib();


	if (m_data == NULL)
		m_data = new uint8_t[pwidth * pheight * 3];

	m_width = pwidth;
	m_height = pheight;
	m_imgLen = pwidth * pheight * ptypeLen;
	//m_frame = Mat::zeros(m_height, m_width, ptypeLen == 1 ? CV_8UC1 : CV_8UC3);
	//m_frame=yang_cvCreateImageHeader(cvSize(pwidth,pheight),IPL_DEPTH_8U,ptypeLen == 1 ? 1 : 3);
	m_frame = yang_cvCreateImage(cvSize(pwidth, pheight), IPL_DEPTH_8U,
			ptypeLen == 1 ? 1 : 3);
	memset(m_frame->imageData, 0, pwidth * pheight * ptypeLen == 1 ? 1 : 3);
	m_hsv = yang_cvCreateImage(cvSize(pwidth, pheight), IPL_DEPTH_8U,
			ptypeLen == 1 ? 1 : 3);	//Mat::zeros(m_height,m_width , ptypeLen == 1 ? CV_8UC1 : CV_8UC3);
	memset(m_hsv->imageData, 0, pwidth * pheight * ptypeLen == 1 ? 1 : 3);
	//m_mask= Mat::zeros(m_height,m_width, CV_8UC1);
	m_mask = yang_cvCreateImage(cvSize(pwidth, pheight), IPL_DEPTH_8U, 1);
	memset(m_mask->imageData, 0, pwidth * pheight);
	int32_t imgLen = pwidth * pheight * 3;
	if (m_bkData == NULL)		m_bkData = new uint8_t[imgLen];
	jpg.readJpegFile(bgfile, &m_jpgData);
    printf("\njpg filename====%s",bgfile);
    printf("\n**************************jpg bitCount==%d\n",m_jpgData.pixelByteCount);
	if (pwidth == m_jpgData.width && pheight == m_jpgData.heigth) {
		memcpy(m_bkData, m_jpgData.data, imgLen);
	} else {
		uint8_t *tmp = new uint8_t[m_jpgData.width
				* m_jpgData.heigth * 4];
		uint8_t *tmp1 = new uint8_t[pwidth * pheight * 4];
		YangYuvConvert yuv;
		yuv.rgb24toargb(m_jpgData.data, tmp, m_jpgData.width, m_jpgData.heigth);
		yuv.scaleArgb(tmp, tmp1, m_jpgData.width, m_jpgData.heigth, pwidth,	pheight);
		yuv.argbtorgb24(tmp1, m_bkData, pwidth, pheight);
		delete[] tmp;
		delete[] tmp1;
		tmp = NULL;
		tmp1 = NULL;
	}


}
void YangMatImageCv::matImage(uint8_t *pImg, uint8_t *pdst) {

	memcpy(m_frame->imageData, (char*) pImg, m_imgLen);
	yang_cvCvtColor(m_frame, m_hsv, 40);

	yang_cvInRangeS(m_hsv, cvScalar(35, 43, 46), cvScalar(77, 255, 255),
			m_mask);

	int32_t value[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1 };
	IplConvKernel *kernel = yang_cvCreateStructuringElementEx(3, 3, 1, 1,
			CV_SHAPE_CUSTOM, value);
	yang_cvMorphologyEx(m_mask, m_mask, NULL, kernel, CV_MOP_CLOSE, 1);
	yang_cvErode(m_mask, m_mask, kernel, 1);
	yang_cvSmooth(m_mask, m_mask, CV_GAUSSIAN, 3, 3, 0, 0);	// 腐蚀用的size是3，模糊的时候也要是3，效果才好
	replace_and_blend(pdst);
}

void YangMatImageCv::replace_and_blend(uint8_t *pdst) {


	uint8_t *current = (uint8_t*) m_frame->imageData;
	uint8_t *bgrow = m_bkData;
	uchar *maskrow = (uchar*) m_mask->imageData;
	uint8_t *targetrow = m_data;
	for (int32_t row = 0; row < m_height; row++) {
		for (int32_t col = 0; col < m_width; col++) {

			if (*maskrow++ == 255) { // 背景
				*(targetrow + 2) = *bgrow++;
				*(targetrow + 1) = *bgrow++;
				*targetrow = *bgrow++;
				current += 3;
				targetrow += 3;
			} else { // 前景
				*(targetrow + 2) = *current++;
				*(targetrow + 1) = *current++;
				*targetrow = *current++;
				bgrow += 3;
				targetrow += 3;
			}
		}
	}
	yuv.rgb24toI420(m_data, pdst, m_width, m_height);
	current = NULL;
	bgrow = NULL;
	maskrow = NULL;
	targetrow = NULL;
	//return result;

}
