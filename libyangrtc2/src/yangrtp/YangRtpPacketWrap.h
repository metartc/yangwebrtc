#ifndef SRC_YANGRTP_YANGRTPPACKETWRAP_H_
#define SRC_YANGRTP_YANGRTPPACKETWRAP_H_
#include <yangrtp/YangRtp.h>
#include <yangrtp/YangRtpBuffer.h>
#include <yangstream/YangStreamCapture.h>
class YangRtpPacketWrap {
public:
	YangRtpPacketWrap();
	virtual ~YangRtpPacketWrap();
	void init(YangRtpBuffer* rtpBuffer);
	int32_t on_h264_video(YangStreamCapture* videoFrame,vector<YangRtpPacket*> &pkts);
	int32_t on_audio(YangStreamCapture* audioFrame,YangRtpPacket* pkt);
	int32_t package_single_nalu(char* p,int32_t plen, int64_t timestamp,vector<YangRtpPacket*>& pkts);
	int32_t package_single_nalu(YangStreamCapture* videoFrame, vector<YangRtpPacket*>& pkts);
	int32_t package_fu_a(YangStreamCapture* videoFrame,int32_t fu_payload_size, vector<YangRtpPacket*>& pkts);
	int32_t package_stap_a(YangStreamCapture* videoFrame,YangRtpPacket* pkt);
public:
	uint32_t m_videoSsrc;
	uint32_t m_audioSsrc;
	uint16_t m_videoSeq;
	uint16_t m_audioSeq;
private:
	YangRtpBuffer* m_rtpBuffer;
};
#endif /* SRC_YANGRTP_YANGRTPPACKETWRAP_H_ */
