#ifndef __YangVideoDecoderHandles__
#define __YangVideoDecoderHandles__
#include "stdint.h"

#include <vector>

#include "yangutil/buffer/YangVideoDecoderBuffer.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/sys/YangThread.h"
#include "YangVideoDecoder.h"
using namespace std;
class YangVideoDecoderHandles:public YangThread,public YangDecoderCallback
{
public:
	YangVideoDecoderHandles(YangVideoInfo *pini);
	~YangVideoDecoderHandles(void);

private:
	int isInit;

public:
	int m_isStart;
	YangVideoDecoder *getDecoder(int puid);
	YangVideoBuffer* getVideoBuffer(int puid);
	int getDecoderIndex(int puid);
	void parseVideoHeader(uint8_t *p,int pind);
	void addVideoStream(unsigned char *ps,int pind,int pisAdd);
	void removeAllStream();
	//int fx;
	void init();
	void stop();
	void setInVideoBuffer(YangVideoDecoderBuffer *pbuf);
	void setOutVideoBuffer(vector<YangVideoBuffer*>* pbuf);

	void onMediaData(unsigned char* p,int plen,long long timestamp,int puid);




protected:
	void run();
	void stopLoop();
	void startLoop();


void saveFile(char *fileName, unsigned char *pBuffer, long BufferLen);

private:
	YangVideoDecoderBuffer *m_in_videoBuffer;
	vector<YangVideoBuffer*> *m_out_videoBuffer;
	vector<YangVideoDecoder*> *m_decs;
	vector<int> m_removeList;
	int m_isConvert;
	//int m_roomState;
	YangVideoInfo *m_ini;

	void removeStream();
	void decode(int isIframe,unsigned char*src,int  p_buflen,unsigned char *dest,int *p_destLen);


};
#endif
