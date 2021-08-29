#ifndef MeetingPlayoBase_H
#define MeetingPlayoBase_H
#include <yangplayer/YangPlayerDecoder.h>
#include <yangplayer/YangPlayerPlay.h>
#include <vector>

#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/buffer/YangVideoDecoderBuffer.h"

//#include "../../yangmeeting/include/MeetingNetBase.h"
//#include "yangutil/YangVideoContext.h"

using namespace std;
class YangPlayerBase
{
    public:
        YangPlayerBase();
        virtual ~YangPlayerBase();
        YangPlayerDecoder *m_ydb;
        YangPlayerPlay *m_ypb;

    void startAudioDecoder(YangAudioEncoderBuffer *prr);
    void startVideoDecoder(YangVideoDecoderBuffer *prr);
    void init(YangAudioInfo* audio);
	//void initPlay(MeetingNetBase *prr);
	void startAudioPlay(YangAudioInfo* paudio);
	//void startVideoPlay();
	//void setAddSdl(YangSdlAdd *psa);

	//void setRtmpBuffer(MeetingNetBase *prr);
	void stopAll();


    protected:

    private:
	//YangMeetingContext *m_ini;
};

#endif // ZBBASE_H
