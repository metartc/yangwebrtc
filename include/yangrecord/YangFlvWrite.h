#ifndef __YangFlvWriter__
#define __YangFlvWriter__
#include "stdlib.h"
#include "stdio.h"
#include <stdint.h>
#include "yangutil/yangtype.h"

//#include "flv_types.h"
//#include "as_objects.h"

#define		FLV_CODEC_NONE		-1
#define		FLV_CODEC_FLV1		2			// Sorenson H.263
#define		FLV_CODEC_FLV4		4			// On2 VP6
#define		FLV_CODEC_H264		7			// H.264

#define		FLV_CODEC_MP3		2			// MP3
#define		FLV_CODEC_AAC		10			// AAC


//typedef long __int64;
typedef unsigned char BYTE;
class YangFlvWriter {
public:

	int video_codec;
	int audio_codec;


	double video_fps;
	int audio_channels;
	int audio_samplerate;

	// time helpers
	bool is_first;
	long long time_first_ms;				// timestamp of the first packet
	int time_last_ms;				// timestamp of the last packet
	int duration_ms;		// calculated duration (for the onMetaData tag)
	int video_raw_size;				// size of all video packets
	int video_frames;				// total video frame count
	int file_size;

	int last_tag_size;				// helper for writing FLV file
	long metadatapos;				// byte position in the file



public:
	YangFlvWriter(char* fileName, YangAudioInfo *paudio,YangVideoInfo *pvideo);
	virtual ~YangFlvWriter();
	void setAudioAac();
	// stream config
	FILE *file;
	int Reset();

	int Start();
	int Stop();
	void Close();
	//void ConfigAll();
	int WriteVideoPacket(unsigned char *packet, int len, int keyframe,
			 int p_timestamp);
	int WriteAudioPacket(unsigned char *packet, int len);
	void WriteVideoInfo(unsigned char* buf1, int buflen);
	double framerate, i_bitrate, i_level_idc;
	unsigned int vtime,pre_vt;
	unsigned int atime,pre_at;

	//int tick, tickinterval, tickinterval1;
	//int tick;

	double atime1;
	double perSt;
private:
	YangAudioInfo *m_audio;
	YangVideoInfo *m_video;
	BYTE prev[4];
	BYTE tag_hdr[20];
	// write the data
	int m_Video_Bit_Count, m_Video_Width, m_Video_Height;

	short vcount;
	int vtcou;
	int WriteMetaData();

	static int MakeAVCc(char* data, int size, char *output_data,
			int output_size);

	char * put_amf_string(char *c, const char *str);
	char * put_amf_double(char *c, double d);
	char * put_byte(char *output, unsigned char nVal);
	char * put_be16(char *output, unsigned short nVal);
	char * put_be24(char *output, unsigned int nVal);
	char * put_be32(char *output, unsigned int nVal);
	char * put_be64(char *output, uint64_t nval);
	//int WriteExtradata(GUID type);
	//AM_MEDIA_TYPE *amt;
	//AM_MEDIA_TYPE *vmt;
	//int ConfigStream(int isVideo);
};

#endif
