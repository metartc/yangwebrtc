#include <yangmeeting/YangMeetingPlay.h>

YangMeetingPlay::YangMeetingPlay(YangMeetingContext *pcontext) {
	m_context = pcontext;
	m_ydb = NULL;

}

YangMeetingPlay::~YangMeetingPlay() {
	m_context = NULL;
	yang_delete(m_ydb);
	//yang_delete(m_ypb);
}

void YangMeetingPlay::stopAll() {
	if (m_ydb)
		m_ydb->stopAll();
}
void YangMeetingPlay::notifyMediaSource(int32_t puid,YangAudioParam* audio,YangVideoParam* video){
	if(m_ydb&&audio) m_ydb->m_audioDec->setRemoteParam(puid,audio);
}
void YangMeetingPlay::setNetBuffer(YangMeetingNet *prr) {

}
void YangMeetingPlay::init(YangMeetingNet *prr) {
	if (m_ydb == NULL) {
		m_ydb = new YangMeetingDecoder(m_context);
		m_ydb->initAudioDecoder();
		m_ydb->initVideoDecoder();
		m_ydb->setInAudioBuffer(prr->getOutAudioList());
		m_ydb->setInVideoBuffer(prr->getOutVideoList());

	}
	//if(m_ypb==NULL) {
//		m_ypb=new YangPlayApp(m_context);
//		m_ypb->initAudioPlay();
//		m_ypb->setInAudioList(m_ydb->getOutAudioList());
//		m_ydb->setInVideoList(prr->getOutVideoList());
	//}

}
void YangMeetingPlay::startAudioDecoder(YangMeetingNet *prr) {
	//if(m_ydb) return;
	init(prr);
	m_ydb->startAudioDecoder();
}
void YangMeetingPlay::startVideoDecoder(YangMeetingNet *prr) {
	//if(m_ydb) return;
	init(prr);
	m_ydb->startVideoDecoder();
}
void YangMeetingPlay::unPlayAudio(int32_t puid) {
	if (m_ydb)
		m_ydb->m_audioDec->removeAudioStream(puid);
}
void YangMeetingPlay::unPlayVideo(int32_t puid) {
	if (m_ydb && m_ydb->m_videoDec)
		m_ydb->m_videoDec->addVideoStream(NULL, puid, 0);
}
void YangMeetingPlay::removeAllStream() {
	if (m_ydb && m_ydb->m_audioDec)
		m_ydb->m_audioDec->removeAllStream();
	if (m_ydb && m_ydb->m_videoDec)
		m_ydb->m_videoDec->removeAllStream();
}

