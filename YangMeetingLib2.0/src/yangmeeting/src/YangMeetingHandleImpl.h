#ifndef SRC_YANGMEETING_SRC_YANGMEETINGHANDLEIMPL_H_
#define SRC_YANGMEETING_SRC_YANGMEETINGHANDLEIMPL_H_
#include <yangmeeting/include/YangMeetingPlay.h>
#include <yangmeeting/include/YangMeetingPublish.h>
#include <yangmeeting/YangMeetingHandle.h>
#include "../include/YangMeetingNet.h"
#include <yangutil/sys/YangSsl.h>
class YangMeetingHandleImpl: public YangMeetingHandle{
public:
	YangMeetingHandleImpl(YangMeetingContext *p_ini);
	virtual ~YangMeetingHandleImpl();
public:
	int32_t init();
	int32_t connectServer();
	int32_t disconnectServer();
	int32_t reconnectServer();
	int32_t startAudioCapture();
	int32_t startVideoCapture();
	void startAudioDecoder();
	void startVideoDecoder();

	void sendKeyframe();

	vector<YangUser*>* getUserList();
	YangVideoBuffer* getPreviewVideo();
	YangVideoBuffer* getVideoBuffer(int32_t uid);
	vector<YangVideoBuffer*>* getDecoderVideoBuffers();
	void change(int32_t st);
	void notifyMediaSource(int32_t puid);
    int32_t sendHeartbeat();
	void addVr();
	void delVr();
	int32_t getConectState();
	void stopAll();
	void setMeetingMessage(YangMeetingMessage* pms);
	void setPlayvideoHandle(YangVideoPlayHandle* playvideo);
	void delMessage();

public:
	void loginServer(int32_t loginState);
	void login(int32_t puid);
	void logout(int32_t puid);
	void startAudioCaptureState();
	void startVideoCaptureState();
	void stopAudioCaptureState();
	void stopVideoCaptureState();

	int32_t pushAudio();
	int32_t pushVideo();
	int32_t unPushAudio();
	int32_t unPushVideo();

	int32_t playAudio(int32_t puid);
	int32_t playVideo(int32_t puid);
	int32_t unPlayAudio(int32_t puid);
	int32_t unPlayVideo(int32_t puid);

	int32_t connectPushServer(int32_t puid);
	int32_t connectPlayServer(int32_t puid);

	YangCertificate* m_cer;
	YangMeetingMessage* m_message;
	YangVideoPlayHandle* m_playvideo;
private:

	void start_audioEncoder();
	void start_videoEncoder();

	void start_audioDecoder();
	void start_videoDecoder();
	void initAec(YangMeetingContext *p_ini);

private:
	YangMeetingPlay *m_playMain;
	YangMeetingPublish *m_pubMain;
	YangMeetingNet *m_netMain;
	YangAecBase *m_aec;
	int32_t m_hasAec;

};
#endif /* SRC_YANGMEETING_SRC_YANGMEETINGHANDLEIMPL_H_ */
