/*
 * YangCaptureBase.cpp
 *
 *  Created on: 2019年10月11日
 *      Author: yang
 */
#include <unistd.h>
#include <yangmeeting/src/YangMeetingCaptureImpl.h>
#include "yangavutil/video/YangYuvConvert.h"
#include "yangavutil/video/YangYuvUtil.h"
#include "yangavutil/video/YangMatImage.h"
#include "yangavutil/vr/YangMatImageCv.h"
YangMeetingCapture::YangMeetingCapture() {
	m_out_audioBuffer = NULL;
	m_ini=NULL;
	m_audioCapture=NULL;
}

YangMeetingCapture::~YangMeetingCapture() {
	if (m_audioCapture) {
		m_audioCapture->stop();
		while (m_audioCapture->m_isStart) {
			usleep(1000);
		}
	}
	yang_delete(m_audioCapture);
	yang_delete(m_out_audioBuffer);
	m_ini=NULL;
}

void YangMeetingCapture::startAudioCaptureState() {
	m_audioCapture->setCatureStart();
}
void YangMeetingCapture::stopAudioCaptureState() {
	m_audioCapture->setCatureStop();
}
void YangMeetingCapture::setAec(YangAecBase *paec) {
	if (m_audioCapture) {
		m_audioCapture->setAec(paec);
	}
}
void YangMeetingCapture::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf){
	if(m_audioCapture!=NULL) m_audioCapture->setInAudioBuffer(pbuf);
}
void YangMeetingCapture::startAudioCapture() {
	if (m_audioCapture && !m_audioCapture->m_isStart)
		m_audioCapture->start();
}
YangAudioBuffer* YangMeetingCapture::getOutAudioBuffer() {
	return m_out_audioBuffer;
}
int32_t YangMeetingCapture::initAudio(YangPreProcess *pp) {
	if (m_out_audioBuffer == NULL) {
		if (m_ini->audio.usingMono)
			m_out_audioBuffer = new YangAudioBuffer(m_ini->audio.audioCacheNum);
		else
			m_out_audioBuffer = new YangAudioBuffer(m_ini->audio.audioCacheNum);
	}
	if (m_audioCapture == NULL) {
		YangCaptureFactory m_capture;
		m_audioCapture = m_capture.createAudioCapture(&m_ini->audio); //new YangAudioCapture(m_ini);
		int32_t ret=m_audioCapture->init();
		if(ret){
			if(ret==ERROR_SYS_NoAudioDevice||ret==ERROR_SYS_NoAudioCaptureDevice) {
				yang_error("ERROR_SYS_NoAudioDevice");
				m_ini->user.micFlag=0;
				return ret;
			}
			m_ini->user.micFlag=1;
		}
		m_audioCapture->setPreProcess(pp);
		m_audioCapture->setOutAudioBuffer(m_out_audioBuffer);

		m_audioCapture->setCatureStop();
	}
	stopAudioCaptureState();
	return Yang_Ok;
}


YangMeetingCaptureImpl::YangMeetingCaptureImpl(YangMeetingContext *pini) {
	m_ini=pini;
	m_out_videoBuffer=NULL;
	m_out_vr_pre_videoBuffer=NULL;
	m_pre_videoBuffer=new YangVideoBuffer(pini->video.bitDepth==8?1:2);
	m_pre_videoBuffer->isPreview=1;
	m_pre_videoBuffer->m_frames=pini->video.frame;
	m_isConvert=0;
	m_isStart=0;
}

YangMeetingCaptureImpl::~YangMeetingCaptureImpl() {
	stopAll();
	if(m_isStart){
		while(m_isStart) usleep(1000);
	}
	yang_stop_thread(m_videoCapture);
	yang_stop_thread(m_audioCapture);

	yang_delete(m_videoCapture);
	yang_delete(m_pre_videoBuffer);
	yang_delete(m_out_videoBuffer);
	yang_delete(m_out_vr_pre_videoBuffer);

}

void YangMeetingCaptureImpl::stopAll(){
	stop();
	if(m_audioCapture) 	m_audioCapture->stop();
	if(m_videoCapture) 	m_videoCapture->stop();

}


void YangMeetingCaptureImpl::startVideoCaptureState() {
	m_videoCapture->initstamp();
	m_videoCapture->setVideoCaptureStart();


}

void YangMeetingCaptureImpl::stopVideoCaptureState() {
	m_videoCapture->setVideoCaptureStop();

}
void YangMeetingCaptureImpl::change(int32_t st) {

}


void YangMeetingCaptureImpl::initVideo(){
	if(m_out_videoBuffer==NULL) m_out_videoBuffer = new YangVideoBuffer(m_ini->video.bitDepth==8?1:2);
	if (m_videoCapture == NULL) {
		YangCaptureFactory cf;

		m_videoCapture = cf.createVideoCapture(&m_ini->video);//new YangVideoCapture(m_ini);

		int32_t ret=Yang_Ok;

		ret=m_videoCapture->init();
       //  if(1) return;
		if(ret){

		}
		m_out_videoBuffer->init(m_ini->video.width,m_ini->video.height,12);
		m_pre_videoBuffer->init(m_ini->video.width,m_ini->video.height,12);
		m_videoCapture->setOutVideoBuffer(m_out_videoBuffer);
		m_videoCapture->setPreVideoBuffer(m_pre_videoBuffer);
		m_videoCapture->setVideoCaptureStart();
	}
	stopVideoCaptureState();
}

void YangMeetingCaptureImpl::startVideoCapture(){
		if(m_videoCapture&&!m_videoCapture->m_isStart) m_videoCapture->start();
}

YangVideoBuffer * YangMeetingCaptureImpl::getOutVideoBuffer(){
	return m_out_videoBuffer;
}

YangVideoBuffer * YangMeetingCaptureImpl::getPreVideoBuffer(){

	return m_pre_videoBuffer;
}
void YangMeetingCaptureImpl::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangMeetingCaptureImpl::stop() {
	stopLoop();
}
void YangMeetingCaptureImpl::stopLoop() {
	m_isConvert = 0;
}
void YangMeetingCaptureImpl::startLoop() {
	m_isConvert = 1;
	int32_t inWidth = m_ini->video.width;
	int32_t inHeight = m_ini->video.height;
	int64_t  prestamp = 0;
	long az = inWidth * inHeight * 2;
    uint8_t *srcData=new uint8_t[az];// { 0 };
    //if (is12bits)
    az = inWidth * inHeight * 3 / 2;

        //mf.getYangMatImage();

    uint8_t *matDst=new uint8_t[inWidth * inHeight * 2];
    uint8_t *matSrcRgb=new uint8_t [inWidth * inHeight * 3];
    uint8_t *matSrcBgr=new uint8_t [inWidth * inHeight * 3];
    YangYuvConvert yuv;


    YangMatImageCv *mat = new YangMatImageCv();
    mat->initImg(m_ini->bgFilename, m_ini->video.width, m_ini->video.height, 3);
   /** if(1){
       // yang_delete(mat);
        yang_deleteA(srcData);
        yang_deleteA(matDst);
        yang_deleteA(matSrcRgb);
        yang_deleteA(matSrcBgr);
        return;

    }**/
    YangFrame videoFrame;

	while (m_isConvert == 1) {

		if (m_out_vr_pre_videoBuffer->m_size == 0) {
			usleep(1000);
			continue;
		}
		videoFrame.payload=srcData;
		videoFrame.nb=az;
		m_out_vr_pre_videoBuffer->getVideo(&videoFrame);
        yuv.I420torgb24(srcData, matSrcRgb, inWidth, inHeight);
		yang_rgbtobgr(matSrcRgb, matSrcBgr, inWidth, inHeight);
         mat->matImage(matSrcBgr, matDst);

		if (videoFrame.timestamp - prestamp <= 0) {
			prestamp = videoFrame.timestamp;
			continue;
		}
		prestamp = videoFrame.timestamp;
		videoFrame.payload=matDst;
		videoFrame.nb=az;
		if (m_videoCapture->getVideoCaptureState())
			m_out_videoBuffer->putVideo(&videoFrame);
        m_pre_videoBuffer->putVideo(&videoFrame);

    }

	yang_delete(mat);
    yang_deleteA(srcData);
    yang_deleteA(matDst);
    yang_deleteA(matSrcRgb);
    yang_deleteA(matSrcBgr);

}
void YangMeetingCaptureImpl::addVr(){
	if (m_out_vr_pre_videoBuffer == NULL)
		m_out_vr_pre_videoBuffer = new YangVideoBuffer(m_ini->video.width,
				m_ini->video.height, 12, m_ini->video.bitDepth == 8 ? 1 : 2);

	m_out_vr_pre_videoBuffer->resetIndex();

	m_pre_videoBuffer->resetIndex();
	m_out_videoBuffer->resetIndex();
	m_videoCapture->setPreVideoBuffer(m_out_vr_pre_videoBuffer);
	m_videoCapture->setOutVideoBuffer(NULL);//(m_out_vr_pre_videoBuffer);
	start();
}
void YangMeetingCaptureImpl::delVr(){
	stop();
	while(m_isStart){
		usleep(1000);
	}
	m_pre_videoBuffer->resetIndex();
		m_out_videoBuffer->resetIndex();
	m_videoCapture->setOutVideoBuffer(m_out_videoBuffer);
		m_videoCapture->setPreVideoBuffer(m_pre_videoBuffer);
}


