/*
 * YangRecord.h
 *
 *  Created on: 2020年10月11日
 *      Author: yang
 */

#ifndef YANGRECORD_INCLUDE_YANGRECORD_H_
#define YANGRECORD_INCLUDE_YANGRECORD_H_

#include "yangutil/sys/YangTime.h"
#include "YangFlvWrite.h"

#include "YangMp4File.h"
struct YangMp4Para {
	YangVideoMeta *vmd;
	unsigned char asc[10];
	char *fileName;
	int ascLen;
};
class YangRecord {
public:
	YangRecord(YangAudioInfo *paudio, YangVideoInfo *pvideo,
			YangVideoEncInfo *penc);
	~YangRecord(void);

	int isMp4;
	void initPara(YangVideoMeta *p_vmd, char *filename, int p_isMp4);
	void setFileTimeLen(int ptlen_min);
	void pauseRec();
	void resumeRec();
	void writeVideoData(unsigned char *srcVideoSource, int videoBufLen,
			long long videoTimestamp, int frameType);
	void writeAudioData(unsigned char *srcAudioSource, int audioBufLen);
	void closeRec();
	YangMp4File *mp4;
	YangFlvWriter *flv;
	YangAudioInfo *m_audio;
	YangVideoInfo *m_video;
	YangVideoEncInfo *m_enc;

	long long oldalltick, alltick;
	int curVideoTimestamp;
	long videoDestLen;
	long long preVideotimestamp, basestamp, minusStamp;
	void put_be32(unsigned char *output, int32_t nVal);
private:
	long long m_prePauseTime, m_afterPauseTime;
	long long m_pauseTime;
	long long m_alltime1;
	int m_alltime;
	int m_fileTimeLen;
	int m_isCreateNewFile;
	YangTime m_time;
	YangMp4Para m_mp4Para;
	void createNewfile();
	void createFile(char *filename);
	void initRecPara();
	int m_fileId;
};

#endif /* YANGRECORD_INCLUDE_YANGRECORD_H_ */
