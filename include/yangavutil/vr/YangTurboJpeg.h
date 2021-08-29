

#ifndef YangTurboJpeg_H_
#define YangTurboJpeg_H_

#include "stdio.h"
//#include "stdlib.h"
#ifdef _WIN32
#include <Windows.h>
#define HAVE_BOOLEAN
#endif
#include "libjpeg/turbojpeg.h"
#include "yangutil/sys/YangLoadLib.h"
//#pragma pack(1)
struct YangJpegData{
	int width;
	int heigth;
	int pixelByteCount;
	int size;
	unsigned char *data;
};
class YangTurboJpeg {
public:
	YangTurboJpeg();
	virtual ~YangTurboJpeg();
	int readJpegFile(char* fileName,YangJpegData *img);
	void write_JPEG_file(char *filename, YangJpegData *img,int quality);
	void rgb2bgr(YangJpegData *rgb,YangJpegData *bgr);
	void bgr2rgb(YangJpegData *bgr,YangJpegData *rgb);
	void freeJpeg(YangJpegData *jpg);
private:
	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
        //jpeg_CreateCompress
	tjhandle (*yang_tjInitDecompress)(void);
	int (*yang_tjDecompressHeader3)(tjhandle handle,
	                                  const unsigned char *jpegBuf,
	                                  unsigned long jpegSize, int *width,
	                                  int *height, int *jpegSubsamp,
	                                  int *jpegColorspace);
	 int (*yang_tjDecompress2)(tjhandle handle, const unsigned char *jpegBuf,
	                            unsigned long jpegSize, unsigned char *dstBuf,
	                            int width, int pitch, int height, int pixelFormat,
	                            int flags);
	 int (*yang_tjDestroy)(tjhandle handle);

};
#endif
//#endif /* YangTurboJpeg_H_ */
