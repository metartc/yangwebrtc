#pragma once
#ifndef __YangAudioEncoderrHandle__
#define __YangAudioEncoderrHandle__
#include "stdint.h"

#include <vector>

#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/sys/YangThread.h"
#include "yangencoder/YangAudioEncoder.h"
using namespace std;
class YangAudioEncoderHandle:public YangThread,public YangEncoderCallback
{
public:
	YangAudioEncoderHandle(YangAudioInfo *pini);
	~YangAudioEncoderHandle(void);

private:
	int m_isInit;

public:
	void init();
	void stop();
	void setInAudioBuffer(YangAudioBuffer *pbuf);
	void setOutAudioBuffer(YangAudioEncoderBuffer *pbuf);
	//void setRoomState(int pst);
	void onVideoData(unsigned char* p,int plen,long long timestamp,int pframetype,int puid);
	void onAudioData(unsigned char* p,int plen,int puid);
	int m_isStart;



protected:
	void run();
	void stopLoop();
	void startLoop();

void saveFile(char *fileName, unsigned char *pBuffer, long BufferLen);

private:

	YangAudioEncoder *m_enc;
	int m_isConvert;
	YangAudioBuffer *m_in_audioBuffer;
	YangAudioEncoderBuffer *m_out_audioBuffer;
	YangAudioInfo *m_ini;
	void Encoder(int isIframe,unsigned char*src,int  p_buflen,unsigned char *dest,int *p_destLen);

};
#endif
