
#ifndef __YangAudioDecoderHandle__
#define __YangAudioDecoderHandle__

#include "stdint.h"
#include <vector>

#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/sys/YangThread.h"

#include "yangdecoder/YangAudioDecoder.h"
using namespace std;
class YangAudioDecoderHandle:public YangThread,public YangDecoderCallback
{
public:
	YangAudioDecoderHandle(YangAudioInfo *pini);
	~YangAudioDecoderHandle(void);

	int m_isStart;
private:
	int m_isInit;

public:

	YangAudioPlayBuffer* getOutAudioBuffer();
	void setRemoteParam(YangAudioParam* para);
	void init();
	void stop();
	void setInAudioBuffer(YangAudioEncoderBuffer *pbuf);
	void setOutAudioBuffer(YangAudioPlayBuffer* pbuf);
	//void setRoomState(int pst);
	void removeAudioStream(int puid);
	void removeAllStream();

	void onMediaData(unsigned char* p,int plen,long long timestamp,int puid);


protected:
	void run();
	void stopLoop();
	void startLoop();

void saveFile(char *fileName, unsigned char *pBuffer, long BufferLen);

private:
	YangAudioPlayBuffer* m_out_audioBuffer;
	YangAudioDecoder *m_decs;
	int m_isConvert;
	YangAudioEncoderBuffer *m_in_audioBuffer;
	YangAudioInfo *m_ini;
	YangAudioParam* m_param;

	int m_frameSize,m_channel;
	void decode(int isIframe,unsigned char*src,int  p_buflen,unsigned char *dest,int *p_destLen);

};
#endif
