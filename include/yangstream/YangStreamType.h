
#ifndef YANGWEBRTC_YangStreamConfig_H_
#define YANGWEBRTC_YangStreamConfig_H_
#include <string>
#include <yangutil/yangtype.h>
enum YangStreamOptType{
	Yang_Stream_Play,
	Yang_Stream_Publish,
	Yang_Stream_Both
};
struct YangAudioParam{
	YangAudioEncDecType encode;
	int sample;
	int channel;
    int clock;
};

struct YangVideoParam{
	YangVideoEncDecType encode;
	int clock;

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
	int localPort;
	int serverPort;
	int uid;
	YangStreamOptType streamOptType;
};
class YangReceiveCallback{
public:
	YangReceiveCallback(){};
	virtual ~YangReceiveCallback(){};
	virtual void receiveAudio(int puid,unsigned char* p,int plen)=0;
	virtual void receiveVideo(int puid,unsigned char* p,int plen,long long ptimestamp,int keyframe)=0;
};
#endif /* YANGWEBRTC_YANGSDPTYPE_H_ */
