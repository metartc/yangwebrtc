#ifndef _YangRecordMp4_H
#define _YangRecordMp4_H
#include "yangutil/sys/YangThread.h"
#include "yangutil/sys/YangTime.h"
#include "yangutil/buffer/YangVideoEncoderBuffer.h"
#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "YangFlvWrite.h"
#include "YangMp4File.h"
struct YangMp4FilePara{
	YangVideoMeta *vmd;
	unsigned char asc[10];
	 char *fileName;
	int ascLen;
};
class YangRecordMp4:public YangThread
{
public:
	YangRecordMp4(YangAudioInfo *paudio,YangVideoInfo *pvideo,YangVideoEncInfo *penc);
	~YangRecordMp4(void);

	int isMp4;
	int m_isStart;
	void setInVideoBuffer(YangVideoEncoderBuffer *pbuf);
	void setInAudioBuffer(YangAudioEncoderBuffer *pbuf);
	void initPara(YangVideoMeta *p_vmd, char *filename,int p_isMp4);
	void setFileTimeLen(int ptlen_min);
	void pauseRec();
	void resumeRec();
	void stop();
protected:
	void run();
private:
	void startLoop();
	void stopLoop();
	void closeRec();

	YangMp4File *mp4;
	YangFlvWriter *flv;
	YangAudioInfo *m_audio;
	YangVideoInfo *m_video;
	YangVideoEncInfo *m_enc;
	YangAudioEncoderBuffer *m_in_audioBuffer;
	YangVideoEncoderBuffer *m_in_videoBuffer;
	long long oldalltick,alltick;
	int curVideoTimestamp;
	long videoDestLen;
	int frameType;
	unsigned char *srcVideoSource;
	unsigned char *srcAudioSource;
	 int audioBufLen;
	 long long m_videoTimestamp,m_preVideotimestamp,m_startStamp,m_mp4Stamp;
	 int videoBufLen;

	int m_isConvert;
	void writeVideoData();
	void writeAudioData();
	void put_be32(unsigned char *output, int32_t nVal) ;
private:
	long long m_prePauseTime,m_afterPauseTime;
	long long m_pauseTime;
	long long m_alltime1;
	int m_alltime;
	int m_fileTimeLen;
	int m_isCreateNewFile;
	YangTime m_time;
	YangMp4FilePara m_mp4Para;
	void createNewfile();
	void createFile(char* filename);
	void initRecPara();
	int m_fileId;
};

#endif

