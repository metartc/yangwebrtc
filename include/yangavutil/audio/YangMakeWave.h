#ifndef  _YangMakeWave_H_
#define  _YangMakeWave_H_

#include "stdint.h"
#include "stdio.h"

typedef  short Int16;
typedef  int Int32;
typedef struct  {
	char fileID[4];
	Int32 fileleth;
	char wavTag[4];
	char FmtHdrID[4];
	Int32  FmtHdrLeth;
	Int16 FormatTag;
	Int16 Channels;
	Int32 SamplesPerSec;
	Int32 AvgBytesPerSec;
	Int16 BlockAlign;
	Int16 BitsPerSample;
	char DataHdrID[4];
	Int32  DataHdrLeth;
} WaveHdr;
class YangMakeWave{
public:
	YangMakeWave();
	FILE *waveFile;
	void init();
	void start(int pisMono,char * filename);
	void write(unsigned char *data,int len);
	void stop();
void writeHeader(int isMono,FILE *WavFile,unsigned long len);
int isMp3(char* p);
//void updateLameTagFrame(lame_global_flags* gfp, FILE* fpStream);
int PCMSize;
int m_isMono;
};
#endif
