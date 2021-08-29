#ifndef __YangMp4File____
#define __YangMp4File____
#include "faac.h"
#include "yangutil/sys/YangLoadLib.h"
#include "yangutil/yangtype.h"
#include "mp4v2/mp4v2.h"


class YangMp4File{

public :
	YangMp4File( char* fileName,YangVideoInfo *pvideo);
	~YangMp4File();
	MP4FileHandle m_MP4hFile ;
   MP4TrackId  m_mp4Audiotrack ;
   MP4TrackId  m_mp4Videotrack ;

    unsigned int m_ntracks, m_trackno ;
    unsigned int m_ndiscs , m_discno ;
    //u_int8_t compilation ;
    const char *m_artist ;//, *title , *album, *year ,
      //*genre , *comment , *writer ;
	double m_newtick,m_oldtick,m_newalltick,m_oldalltick,m_tmptick;
	int m_tick,m_interval,m_interval1;
   // unsigned char *art ;
   // unsigned int artSize;
    unsigned int m_total_samples;
    unsigned int m_encoded_samples;
    unsigned int m_delay_samples;
    unsigned int m_frameSize;
	int m_cou;



	void init(unsigned char *p_spsBuf, int p_spsLen);
	//void write(unsigned char *bitbuf,int bytesWritten);
	void closeMp4();

	int WriteVideoPacket(unsigned char *packet, int len,int keyframe, long long timestamp);
	int WriteAudioPacket(unsigned char *packet, int len);
	void WriteVideoInfo(unsigned char *p_vpsBuf,int p_vpsLen,unsigned char *p_spsBuf, int p_spsLen,unsigned char *p_ppsBuf, int p_ppsLen);
	void WriteAudioInfo(unsigned char *pasc,unsigned long pasclen,unsigned char* buf1,int buflen);
	double m_framerate,m_bitrate, m_level_idc;
private:
	YangVideoInfo *m_ini;
	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
	const MP4Tags* (*yang_MP4TagsAlloc)( void );
    void (*yang_MP4TagsFree)( const MP4Tags* tags );
	MP4FileHandle (*yang_MP4Create)(const char* fileName,uint32_t flags);
	bool (*yang_MP4TagsFetch)( const MP4Tags* tags, MP4FileHandle hFile );
	bool (*yang_MP4TagsSetSortArtist)      ( const MP4Tags*, const char* );
	bool (*yang_MP4TagsStore)( const MP4Tags* tags, MP4FileHandle hFile );
	bool (*yang_MP4SetTimeScale)( MP4FileHandle hFile, uint32_t value );
	void (*yang_MP4SetVideoProfileLevel)( MP4FileHandle hFile, uint8_t value );
	void (*yang_MP4SetAudioProfileLevel)( MP4FileHandle hFile, uint8_t value );
	MP4TrackId (*yang_MP4AddAudioTrack)( MP4FileHandle hFile, uint32_t      timeScale,
	    MP4Duration   sampleDuration,  uint8_t       audioType );
	MP4TrackId (*yang_MP4AddH264VideoTrack)(
	    MP4FileHandle hFile, uint32_t timeScale,MP4Duration sampleDuration,uint16_t width,uint16_t  height,	uint8_t   AVCProfileIndication
		,uint8_t profile_compat,  uint8_t  AVCLevelIndication,  uint8_t  sampleLenFieldSizeMinusOne );
	bool (*yang_MP4SetTrackESConfiguration)(
	    MP4FileHandle  hFile,
	    MP4TrackId     trackId,
	    const uint8_t* pConfig,
	    uint32_t       configSize );
	void (*yang_MP4AddH264SequenceParameterSet)(
	    MP4FileHandle  hFile,
	    MP4TrackId     trackId,
	    const uint8_t* pSequence,
	    uint16_t       sequenceLen );
	void (*yang_MP4AddH264PictureParameterSet)(
	    MP4FileHandle  hFile,
	    MP4TrackId     trackId,
	    const uint8_t* pPict,
	    uint16_t       pictLen );
	bool (*yang_MP4WriteSample)(
	    MP4FileHandle  hFile,
	    MP4TrackId     trackId,
	    const uint8_t* pBytes,
	    uint32_t       numBytes,
	    MP4Duration    duration,
	    MP4Duration    renderingOffset,
	    bool           isSyncSample);
	void (*yang_MP4Close)(
	    MP4FileHandle hFile,
	    uint32_t    flags);

	MP4TrackId (*yang_MP4AddH265VideoTrack)(
	    MP4FileHandle hFile,
	    uint32_t      timeScale,
	    MP4Duration   sampleDuration,
	    uint16_t      width,
	    uint16_t      height,
	    uint8_t       isIso);
	void (*yang_MP4AddH265SequenceParameterSet)(
	    MP4FileHandle  hFile,
	    MP4TrackId     trackId,
	    const uint8_t* pSequence,
	    uint16_t       sequenceLen );
	void (*yang_MP4AddH265PictureParameterSet)(
	    MP4FileHandle  hFile,
	    MP4TrackId     trackId,
	    const uint8_t* pPict,
	    uint16_t       pictLen );
	void (*yang_MP4AddH265VideoParameterSet) (MP4FileHandle hFile,
										 MP4TrackId trackId,
										 const uint8_t *pSequence,
										 uint16_t sequenceLen);
	uint32_t (*yang_MP4GetTrackTimeScale)(
	    MP4FileHandle hFile,
	    MP4TrackId    trackId );
	bool (*yang_MP4SetTrackTimeScale)(
	    MP4FileHandle hFile,
	    MP4TrackId    trackId,
	    uint32_t      value );
};
#endif
