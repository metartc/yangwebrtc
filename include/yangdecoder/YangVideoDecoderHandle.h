#ifndef __YangVideoDecoderHandle__
#define __YangVideoDecoderHandle__
#include "stdint.h"

#include <vector>

#include "yangutil/buffer/YangVideoDecoderBuffer.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/sys/YangThread.h"
#include "YangVideoDecoder.h"
using namespace std;
class YangVideoDecoderHandle:public YangThread,public YangDecoderCallback
{
public:
	YangVideoDecoderHandle(YangVideoInfo *pini);
	~YangVideoDecoderHandle(void);

private:
	int isInit;

public:
	int m_isStart;
	YangVideoDecoder *getDecoder(int puid);
	YangVideoBuffer* getOutVideoBuffer();
	//int getDecoderIndex(int puid);
	void parseVideoHeader(uint8_t *p,int pind);
	void addVideoStream(unsigned char *ps,int pind,int pisAdd);
	//void removeAllStream();
	//int fx;
	void init();
	void stop();
	void setInVideoBuffer(YangVideoDecoderBuffer *pbuf);
	void setOutVideoBuffer(YangVideoBuffer* pbuf);

	void onMediaData(unsigned char* p,int plen,long long timestamp,int puid);




protected:
	void run();
	void stopLoop();
	void startLoop();


void saveFile(char *fileName, unsigned char *pBuffer, long BufferLen);

private:
	YangVideoDecoderBuffer *m_in_videoBuffer;
	YangVideoBuffer *m_out_videoBuffer;
	YangVideoDecoder *m_decs;
	//vector<int> m_removeList;
	int m_isConvert;
	YangVideoInfo *m_ini;

	//void removeStream();
	void decode(int isIframe,unsigned char*src,int  p_buflen,unsigned char *dest,int *p_destLen);


};
#endif
