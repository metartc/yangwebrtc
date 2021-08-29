
#ifndef __YangAudioDecoderHandles__
#define __YangAudioDecoderHandles__

#include "stdint.h"
#include <vector>
#include <map>
#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/sys/YangThread.h"

#include "yangdecoder/YangAudioDecoder.h"
using namespace std;
class YangAudioDecoderHandles:public YangThread,public YangDecoderCallback
{
public:
	YangAudioDecoderHandles(YangAudioInfo *pini);
	~YangAudioDecoderHandles(void);

private:
	int m_isInit;

public:
	YangAudioDecoder *getDecoder(int puid);
	YangAudioPlayBuffer* getAudioBuffer(int puid);
	int getDecoderIndex(int puid);
	void init();
	void stop();
	void setInAudioBuffer(YangAudioEncoderBuffer *pbuf);
	void setOutAudioBuffer(vector<YangAudioPlayBuffer*>* pbuf);
	//void setRoomState(int pst);
	void removeAudioStream(int puid);
	void removeAllStream();
	int m_isStart;
	void onMediaData(unsigned char* p,int plen,long long timestamp,int puid);
	void setRemoteParam(int puid,YangAudioParam* para);

protected:
	void run();
	void stopLoop();
	void startLoop();

void saveFile(char *fileName, unsigned char *pBuffer, long BufferLen);

private:
	std::map<int,YangAudioParam*> m_paramMap;
	vector<YangAudioPlayBuffer*> *m_out_audioBuffer;
	vector<YangAudioDecoder*> *m_decs;
	int m_isConvert;
	YangAudioEncoderBuffer *m_in_audioBuffer;
	YangAudioInfo *m_ini;

	int m_frameSize,m_channel;
	void decode(int isIframe,unsigned char*src,int  p_buflen,unsigned char *dest,int *p_destLen);

};
#endif
