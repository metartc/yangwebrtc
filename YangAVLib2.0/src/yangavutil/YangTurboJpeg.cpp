
#include <stdio.h>
#include <stdlib.h>
#include <yangavutil/vr/YangTurboJpeg.h>


#include "memory.h"


void YangTurboJpeg::loadLib(){
	//yang_opus_encoder_create=(OpusEncoder *(*)(opus_int32_t Fs, int32_t channels, int32_t application,  int32_t *error))m_lib.loadFunction("");
	yang_tjInitDecompress=(tjhandle (*)(void))m_lib.loadFunction("tjInitDecompress");
	yang_tjDecompressHeader3=(int32_t (*)(tjhandle handle,
	                                  const uint8_t *jpegBuf,
	                                  unsigned long jpegSize, int32_t *width,
	                                  int32_t *height, int32_t *jpegSubsamp,
	                                  int32_t *jpegColorspace))m_lib.loadFunction("tjDecompressHeader3");
	 yang_tjDecompress2=(int32_t (*)(tjhandle handle, const uint8_t *jpegBuf,
	                            unsigned long jpegSize, uint8_t *dstBuf,
	                            int32_t width, int32_t pitch, int32_t height, int32_t pixelFormat,
	                            int32_t flags))m_lib.loadFunction("tjDecompress2");
	 yang_tjDestroy=(int32_t (*)(tjhandle handle))m_lib.loadFunction("tjDestroy");

}

void YangTurboJpeg::unloadLib(){
	yang_tjInitDecompress=NULL;
	yang_tjDecompressHeader3=NULL;
	yang_tjDecompress2=NULL;
	yang_tjDestroy=NULL;


}
YangTurboJpeg::YangTurboJpeg() {

	unloadLib();
	m_lib.loadObject("libturbojpeg");
	loadLib();
}

YangTurboJpeg::~YangTurboJpeg() {
	unloadLib();
	m_lib.unloadObject();
}





 int32_t YangTurboJpeg::readJpegFile(char* filename,YangJpegData *img) {
	 	 FILE *infile;
		   if ((infile = fopen(filename, "rb")) == NULL) {
		     fprintf(stderr, "can't open %s\n", filename);
		     return 0;
		   }
		   fseek(infile, 0, SEEK_END);
		   	unsigned long srcSize = ftell(infile);
		   	uint8_t *srcBuf=new uint8_t[srcSize];
		   	fseek(infile, 0, SEEK_SET);
		   	fread(srcBuf, srcSize, 1, infile);
		   	fclose(infile);

		tjhandle handle = yang_tjInitDecompress();
		int32_t subsamp, cs;
		img->pixelByteCount=3;
		int32_t ret = yang_tjDecompressHeader3(handle, srcBuf, srcSize, &img->width, &img->heigth, &subsamp, &cs);
		if (cs == TJCS_GRAY) img->pixelByteCount = 1;
		int32_t pf = TJCS_RGB;
		int32_t ps = tjPixelSize[pf];

		img->size=img->width*img->heigth*img->pixelByteCount;
		if(img->data==NULL) img->data=new uint8_t[img->size];
		ret=yang_tjDecompress2(handle, (const uint8_t*)srcBuf, srcSize, img->data, img->width, img->width * img->pixelByteCount, img->heigth, TJPF_RGB, TJFLAG_NOREALLOC);

		yang_tjDestroy(handle);


	   return 1;
}


void YangTurboJpeg::write_JPEG_file(char *filename, YangJpegData *img,int32_t quality)
 {
	/**
   struct jpeg_compress_struct cinfo;
   struct jpeg_error_mgr jerr;
   FILE *outfile;
   JSAMPROW row_pointer[1];
   int32_t row_stride;
   cinfo.err = yang_jpeg_std_error(&jerr);

   yang_jpeg_CreateCompress(&cinfo,JPEG_LIB_VERSION, \
           (size_t)sizeof(struct jpeg_compress_struct));

   if ((outfile = fopen(filename, "wb")) == NULL) {
     fprintf(stderr, "can't open %s\n", filename);
     exit(1);
   }
   yang_jpeg_stdio_dest(&cinfo, outfile);

   cinfo.image_width = img->width;
   cinfo.image_height = img->heigth;
   cinfo.input_components = 3;
   cinfo.in_color_space = JCS_RGB;

   yang_jpeg_set_defaults(&cinfo);

   yang_jpeg_set_quality(&cinfo, quality, TRUE );

   yang_jpeg_start_compress(&cinfo, TRUE);


   row_stride = img->width * 3;

   while (cinfo.next_scanline < cinfo.image_height) {
     row_pointer[0] = &img->data[cinfo.next_scanline * row_stride];
     (void)yang_jpeg_write_scanlines(&cinfo, row_pointer, 1);
   }

   yang_jpeg_finish_compress(&cinfo);

   fclose(outfile);

   yang_jpeg_destroy_compress(&cinfo);
   **/

 }
void YangTurboJpeg::rgb2bgr(YangJpegData *rgb,YangJpegData *bgr){
	int32_t dataIsNull=(bgr->data==NULL);
	memcpy(bgr,rgb,sizeof(YangJpegData));
	if(dataIsNull) bgr->data=new uint8_t[bgr->size];
	memcpy(bgr->data,rgb->data,rgb->size);
	for(int32_t i=0;i<bgr->size;i+=3){
		bgr->data[i]=rgb->data[i+2];
		bgr->data[i+2]=rgb->data[i];
	}

}
void YangTurboJpeg::bgr2rgb(YangJpegData *bgr,YangJpegData *rgb){
	int32_t dataIsNull=(rgb->data==NULL);
	memcpy(rgb,bgr,sizeof(YangJpegData));
	if(dataIsNull) rgb->data=new uint8_t[rgb->size];
	memcpy(rgb->data,bgr->data,rgb->size);
	for(int32_t i=0;i<bgr->size;i+=3){
		rgb->data[i]=bgr->data[i+2];
		rgb->data[i+2]=bgr->data[i];
	}
}
void YangTurboJpeg::freeJpeg(YangJpegData *jpg){
	if(jpg->data){
		delete[] jpg->data;
		jpg->data=NULL;
	}
}


