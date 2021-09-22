

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
	int32_t width;
	int32_t heigth;
	int32_t pixelByteCount;
	int32_t size;
	uint8_t *data;
};
class YangTurboJpeg {
public:
	YangTurboJpeg();
	virtual ~YangTurboJpeg();
	int32_t readJpegFile(char* fileName,YangJpegData *img);
	void write_JPEG_file(char *filename, YangJpegData *img,int32_t quality);
	void rgb2bgr(YangJpegData *rgb,YangJpegData *bgr);
	void bgr2rgb(YangJpegData *bgr,YangJpegData *rgb);
	void freeJpeg(YangJpegData *jpg);
private:
	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
        //jpeg_CreateCompress
	tjhandle (*yang_tjInitDecompress)(void);
	int32_t (*yang_tjDecompressHeader3)(tjhandle handle,
	                                  const uint8_t *jpegBuf,
	                                  unsigned long jpegSize, int32_t *width,
	                                  int32_t *height, int32_t *jpegSubsamp,
	                                  int32_t *jpegColorspace);
	 int32_t (*yang_tjDecompress2)(tjhandle handle, const uint8_t *jpegBuf,
	                            unsigned long jpegSize, uint8_t *dstBuf,
	                            int32_t width, int32_t pitch, int32_t height, int32_t pixelFormat,
	                            int32_t flags);
	 int32_t (*yang_tjDestroy)(tjhandle handle);

};
#endif
//#endif /* YangTurboJpeg_H_ */
