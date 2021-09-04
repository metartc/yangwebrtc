
#ifndef YANGWEBRTC_YangStreamConfig_H_
#define YANGWEBRTC_YangStreamConfig_H_
#include <yangutil/yangavinfotype.h>
#include <string>
enum YangStreamOptType{
	Yang_Stream_Play,
	Yang_Stream_Publish,
	Yang_Stream_Both
};
struct YangAudioParam{
	YangAudioEncDecType encode;
	int32_t sample;
	int32_t channel;
    int32_t clock;
};

struct YangVideoParam{
	YangVideoEncDecType encode;
	int32_t clock;

};

struct SrsSdpResponseType {
	std::string retcode;
	std::string serverIp;
	std::string sdp;
	std::string sessionid;
};

struct YangStreamConfig{
	std::string serverIp;
	std::string localIp;
	std::string app;
	std::string stream;
	std::string url;
	int32_t localPort;
	int32_t serverPort;
	int32_t uid;
	YangStreamOptType streamOptType;
};
class YangReceiveCallback{
public:
	YangReceiveCallback(){};
	virtual ~YangReceiveCallback(){};
	virtual void receiveAudio(YangAudioFrame* audioFrame)=0;
	virtual void receiveVideo(YangVideoFrame* videoFrame)=0;
};
#endif /* YANGWEBRTC_YANGSDPTYPE_H_ */
