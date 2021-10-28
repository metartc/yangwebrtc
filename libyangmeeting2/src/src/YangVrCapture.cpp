/*
 * YangCapture.cpp
 *
 *  Created on: 2020年10月3日
 *      Author: yang
 */

#include <yangcapture/YangCaptureFactory.h>
#include <yangmeeting/YangVrCapture.h>
#include "yangavutil/video/YangYuvConvert.h"
#include "yangavutil/video/YangYuvUtil.h"
#if HavingVr
#include "yangavutil/video/YangMatImage.h"
#include "yangavutil/vr/YangMatImageCv.h"
#endif
YangVrCapture::YangVrCapture(YangMeetingContext *pcontext) {

	m_context = pcontext;
	m_cameraCount = m_context->sys.cameraCount;
	if (!pcontext->sys.isMultCamera)
		m_cameraCount = 1;
	m_pre_st = 1;
	m_pre_videoBuffer = NULL; //new YangVideoBuffer(pcontext->video.bitDepth==8?1:2);
	m_out_videoBuffer = NULL;
	m_out_vr_pre_videoBuffer = NULL;
	m_isStart = 0;
	m_isConvert = 0;
//	m_hasVr=pcontext->sys.hasVr;

}

YangVrCapture::~YangVrCapture() {
	stopAll();
	int32_t ret = 0;
	while (1) {
		ret = 0;
		for (int32_t i = 0; i < m_cameraCount; i++) {
			if (m_vcs[i]->m_isStart)
				ret = 1;
		}
		if (!ret)
			break;
		yang_usleep(1000);
	}
	while (m_isStart) {
		yang_usleep(1000);
	}

	for (int32_t i = 0; i < m_cameraCount; i++) {
		yang_delete(m_vcs[i]);
	}
	m_vcs.clear();
	//yang_delete(m_out_vr_videoBuffer);
	yang_delete(m_out_vr_pre_videoBuffer);

	yang_delete(m_out_videoBuffer);
	yang_delete(m_pre_videoBuffer);

}

void YangVrCapture::addVr() {
	if (m_out_vr_pre_videoBuffer == NULL)
		m_out_vr_pre_videoBuffer = new YangVideoBuffer(m_context->video.width,
				m_context->video.height, 12, m_context->video.bitDepth == 8 ? 1 : 2);

	m_out_vr_pre_videoBuffer->resetIndex();

	m_pre_videoBuffer->resetIndex();
	m_out_videoBuffer->resetIndex();
	for (int32_t i = 0; i < m_cameraCount; i++) {
		m_vcs[i]->setFilmOutVideoBuffer(m_out_vr_pre_videoBuffer);
		m_vcs[i]->setOutVideoBuffer(NULL);
	}
	if (!m_isStart)
		start();
}
void YangVrCapture::delVr() {
	stop();
	while (m_isStart) {
		yang_usleep(1000);
	}
	m_pre_videoBuffer->resetIndex();
	m_out_videoBuffer->resetIndex();
	for (int32_t i = 0; i < m_cameraCount; i++) {
		m_vcs[i]->setOutVideoBuffer(m_out_videoBuffer);
		m_vcs[i]->setFilmOutVideoBuffer(m_pre_videoBuffer);
	}

}
void YangVrCapture::initVideo() {
	//int32_t cindexs[7] = { m_context->camera.cameraIndex1, m_context->camera.cameraIndex2,
	//		m_context->camera.cameraIndex3, m_context->camera.cameraIndex4, m_context->camera.cameraIndex5,
	//		m_context->camera.cameraIndex6, m_context->camera.cameraIndex7 };

	vector<int> cindexs;
	if (m_pre_videoBuffer == NULL) {
		m_pre_videoBuffer = new YangVideoBuffer(
				m_context->video.bitDepth == 8 ? 1 : 2);
		m_pre_videoBuffer->isPreview = 1;
		m_pre_videoBuffer->m_frames = m_context->video.frame;
	}
	if (m_out_videoBuffer == NULL)
		m_out_videoBuffer = new YangVideoBuffer(
				m_context->video.bitDepth == 8 ? 1 : 2);
	int32_t hasInitVideoBuffer = 0;

	if (m_cameraCount > 1) {
		yang_get_camera_indexs(&cindexs, string(m_context->sys.cameraIndexs));
		if (cindexs.size() < (size_t) m_cameraCount) {
			cindexs.clear();
			for (int32_t k = 0; k < m_cameraCount; k++) {
				cindexs.push_back(k);
			}
		}
	}
	for (int32_t i = 0; i < m_cameraCount; i++) {
		YangCaptureFactory cf;
		m_vcs[i] = cf.createRecordVideoCapture(&m_context->video);
		m_vcs[i]->cameraIndex = cindexs[i];
		//printf("\n**************************vindex=====%d", cindexs[i]);
		m_vcs[i]->init();
		m_vcs[i]->setVideoCaptureStop();
		m_vcs[i]->setFilmVideoCaptureStop();
		if (!hasInitVideoBuffer) {
			m_out_videoBuffer->init(m_context->video.width, m_context->video.height,
					12);
			m_pre_videoBuffer->init(m_context->video.width, m_context->video.height,
					12);
			hasInitVideoBuffer = 1;
		}
		m_vcs[i]->setOutVideoBuffer(m_out_videoBuffer);
		m_vcs[i]->setFilmOutVideoBuffer(m_pre_videoBuffer);
	}

	stopVideoCaptureState();
	m_vcs[0]->setFilmVideoCaptureStart();
}

void YangVrCapture::stopVideoCaptureState() {
	for (int32_t i = 0; i < m_cameraCount; i++) {
		m_vcs[i]->setVideoCaptureStop();
		//m_vcs[i]->setFilmVideoCaptureStop();
	}
}

void YangVrCapture::startVideoCaptureState() {

	for (int32_t i = 0; i < m_cameraCount; i++) {
		m_vcs[i]->setVideoCaptureStop();
		m_vcs[i]->initstamp();
	}
	m_vcs[0]->setVideoCaptureStart();

}

void YangVrCapture::startVideoCapture() {
	for (int32_t i = 0; i < m_cameraCount; i++) {
		if (m_vcs[i] && !m_vcs[i]->m_isStart)
			m_vcs[i]->start();
	}
}

YangVideoBuffer* YangVrCapture::getOutVideoBuffer() {
	return m_out_videoBuffer;
}

YangVideoBuffer* YangVrCapture::getPreVideoBuffer() {

	return m_pre_videoBuffer;
}

void YangVrCapture::stopAll() {
	if (m_isStart)
		stop();
	if (m_audioCapture)
		m_audioCapture->stop();
	for (int32_t i = 0; i < m_cameraCount; i++)
		m_vcs[i]->stop();
}

void YangVrCapture::change(int32_t st) {

	if (m_pre_st == st)
		return;
	if (st > this->m_cameraCount)
		return;
	stopVideoCaptureState();
	m_vcs[st - 1]->setVideoCaptureStart();
	for (int32_t i = 0; i < m_cameraCount; i++)
		m_vcs[i]->setFilmVideoCaptureStop();
	m_vcs[st - 1]->setFilmVideoCaptureStart();
	m_pre_st = st;

}
void YangVrCapture::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangVrCapture::stop() {
	stopLoop();
}
void YangVrCapture::stopLoop() {
	m_isConvert = 0;
}
void YangVrCapture::startLoop() {
#if HavingVr
	m_isConvert = 1;
	int32_t inWidth = m_context->video.width;
	int32_t inHeight = m_context->video.height;
	int64_t prestamp = 0;
	long az = inWidth * inHeight * 2;
    uint8_t* srcData=new uint8_t[az];// = { 0 };

	az = inWidth * inHeight * 3 / 2;
	YangMatImageCv *mat = new YangMatImageCv();
    uint8_t* matDst=new uint8_t[m_context->video.width * m_context->video.height * 2];
    uint8_t* matSrcRgb=new uint8_t[m_context->video.width * m_context->video.height * 3];
    uint8_t* matSrcBgr=new uint8_t[m_context->video.width * m_context->video.height * 3];
    YangAutoFreeA(uint8_t,srcData);
    YangAutoFreeA(uint8_t,matDst);
    YangAutoFreeA(uint8_t,matSrcRgb);
    YangAutoFreeA(uint8_t,matSrcBgr);
	YangYuvConvert yuv;
	mat->initImg(m_context->bgFilename, m_context->video.width, m_context->video.height, 3);
	YangFrame videoFrame;
	while (m_isConvert == 1) {

		if (m_out_vr_pre_videoBuffer->size() == 0) {
			yang_usleep(1000);
			continue;
		}
		videoFrame.payload = srcData;
		videoFrame.nb = az;
		m_out_vr_pre_videoBuffer->getVideo(&videoFrame);
		yuv.I420torgb24(srcData, matSrcRgb, inWidth, inHeight);
		yang_rgbtobgr(matSrcRgb, matSrcBgr, inWidth, inHeight);
		mat->matImage(matSrcBgr, matDst);

		if (videoFrame.timestamp - prestamp <= 0) {
			prestamp = videoFrame.timestamp;
			continue;
		}
		prestamp = videoFrame.timestamp;
		videoFrame.payload = matDst;
		videoFrame.nb = az;
		if (m_vcs[m_pre_st]->getVideoCaptureState())
			m_out_videoBuffer->putVideo(&videoFrame);
		m_pre_videoBuffer->putVideo(&videoFrame);

	}
	yang_delete(mat);
#endif
}

