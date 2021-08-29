#ifndef YangPlayReceive_H
#define YangPlayReceive_H
//#include "mytype.h"
#include <yangstream/YangStreamHandle.h>
#include <string>
#include "yangutil/yangtype.h"
#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangVideoDecoderBuffer.h"
#include "yangutil/sys/YangThread.h"
using namespace std;
class YangPlayReceive: public YangThread, public YangReceiveCallback
{
    public:
        YangPlayReceive();
        virtual ~YangPlayReceive();
    	void receiveAudio(int puid, unsigned char *p, int plen);
    	void receiveVideo(int puid, unsigned char *p, int plen,
    			long long ptimestamp, int keyframe);
        int init(int nettype,string server,int pport,string stream);
        void setBuffer(YangAudioEncoderBuffer *al,YangVideoDecoderBuffer *vl);
        void disConnect();
        void play(char* pserverStr,char *streamName);
        YangStreamHandle *m_recv;
       // void start_playVideo(YangMeetingContext *p_ini,char *p_ip, char* p_str, int p_room_id);
       // void start_playAV(YangMeetingContext *p_ini,char *p_ip, char* p_str, int p_room_id);
    	int isReceived; //,isHandled;
    	int isReceiveConvert; //,isHandleAllInvoke;
    	int m_isStart;
    	void stop();
    protected:
    	void run();
    	void startLoop();



    private:
    	int m_headLen;
    	YangAudioEncoderBuffer *m_out_audioBuffer;
    	YangVideoDecoderBuffer *m_out_videoBuffer;
};

#endif // VIDEOMEETING_H
