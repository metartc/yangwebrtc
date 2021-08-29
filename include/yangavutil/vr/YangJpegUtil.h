/*
 * YangJpegUtil.h
 *
 *  Created on: 2021年1月21日
 *      Author: yang
 */

#ifndef YANGJPEGUTIL_H_
#define YANGJPEGUTIL_H_

#include "stdio.h"
//#include "stdlib.h"
#ifdef _WIN32
#include <Windows.h>
#define HAVE_BOOLEAN
#endif
#include "libjpeg/jpeglib.h"
#include "yangutil/sys/YangLoadLib.h"
//#pragma pack(1)
struct YangJpegData{
	int width;
	int heigth;
	int pixelByteCount;
	int size;
	unsigned char *data;
};
class YangJpegUtil {
public:
	YangJpegUtil();
	virtual ~YangJpegUtil();
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
	void (*yang_jpeg_CreateCompress)(j_compress_ptr cinfo, int version,
	                                 size_t structsize);
	void (*yang_jpeg_CreateDecompress)(j_decompress_ptr cinfo, int version,
            size_t structsize);
	void (*yang_jpeg_stdio_src)(j_decompress_ptr cinfo, FILE *infile);
	int (*yang_jpeg_read_header)(j_decompress_ptr cinfo, boolean require_image);
	boolean (*yang_jpeg_start_decompress)(j_decompress_ptr cinfo);
	JDIMENSION (*yang_jpeg_read_scanlines)(j_decompress_ptr cinfo,
	                                       JSAMPARRAY scanlines,
	                                       JDIMENSION max_lines);
	boolean (*yang_jpeg_finish_decompress)(j_decompress_ptr cinfo);
	void (*yang_jpeg_destroy_decompress)(j_decompress_ptr cinfo);
	void (*yang_jpeg_stdio_dest)(j_compress_ptr cinfo, FILE *outfile);
	void (*yang_jpeg_set_defaults)(j_compress_ptr cinfo);
	void (*yang_jpeg_set_quality)(j_compress_ptr cinfo, int quality,
	                              boolean force_baseline);
	void (*yang_jpeg_start_compress)(j_compress_ptr cinfo,
	                                 boolean write_all_tables);
	JDIMENSION (*yang_jpeg_write_scanlines)(j_compress_ptr cinfo,
	                                        JSAMPARRAY scanlines,
	                                        JDIMENSION num_lines);
	void (*yang_jpeg_finish_compress)(j_compress_ptr cinfo);
	void (*yang_jpeg_destroy_compress)(j_compress_ptr cinfo);
        struct jpeg_error_mgr * (*yang_jpeg_std_error)(struct jpeg_error_mgr *err);
};
#endif
//#endif /* YANGJPEGUTIL_H_ */
