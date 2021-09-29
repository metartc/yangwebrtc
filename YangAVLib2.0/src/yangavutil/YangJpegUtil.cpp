/*
 * YangJpegUtil.cpp
 *
 *  Created on: 2021年1月21日
 *      Author: yang
 */

#include <stdio.h>
#include <stdlib.h>
#include <yangavutil/vr/YangJpegUtil.h>


#include "memory.h"
#include <setjmp.h>

struct my_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */
  jmp_buf setjmp_buffer;        /* for return to caller */
};


typedef struct my_error_mgr *my_error_ptr;
void YangJpegUtil::loadLib(){
	//yang_opus_encoder_create=(OpusEncoder *(*)(opus_int32_t Fs, int32_t channels, int32_t application,  int32_t *error))m_lib.loadFunction("");
	yang_jpeg_CreateCompress=(void (*)(j_compress_ptr cinfo, int32_t version,
	                                 size_t structsize))m_lib.loadFunction("jpeg_CreateCompress");
	yang_jpeg_CreateDecompress=(void (*)(j_decompress_ptr cinfo, int32_t version,
	            size_t structsize))m_lib.loadFunction("jpeg_CreateDecompress");
	yang_jpeg_stdio_src=(void (*)(j_decompress_ptr cinfo, FILE *infile))m_lib.loadFunction("jpeg_stdio_src");
	yang_jpeg_read_header=(int32_t (*)(j_decompress_ptr cinfo, boolean require_image))m_lib.loadFunction("jpeg_read_header");
	yang_jpeg_start_decompress=(boolean (*)(j_decompress_ptr cinfo))m_lib.loadFunction("jpeg_start_decompress");
	yang_jpeg_read_scanlines=(JDIMENSION (*)(j_decompress_ptr cinfo,
		                                       JSAMPARRAY scanlines,
		                                       JDIMENSION max_lines))m_lib.loadFunction("jpeg_read_scanlines");
	yang_jpeg_finish_decompress=(boolean (*)(j_decompress_ptr cinfo))m_lib.loadFunction("jpeg_finish_decompress");
	yang_jpeg_destroy_decompress=(void (*)(j_decompress_ptr cinfo))m_lib.loadFunction("jpeg_destroy_decompress");
	yang_jpeg_stdio_dest=(void (*)(j_compress_ptr cinfo, FILE *outfile))m_lib.loadFunction("jpeg_stdio_dest");
	yang_jpeg_set_defaults=(void (*)(j_compress_ptr cinfo))m_lib.loadFunction("jpeg_set_defaults");
	yang_jpeg_set_quality=(void (*)(j_compress_ptr cinfo, int32_t quality,
	                              boolean force_baseline))m_lib.loadFunction("jpeg_set_quality");
	yang_jpeg_start_compress=(void (*)(j_compress_ptr cinfo,
	                                 boolean write_all_tables))m_lib.loadFunction("jpeg_start_compress");
	yang_jpeg_write_scanlines=(JDIMENSION (*)(j_compress_ptr cinfo,
	                                        JSAMPARRAY scanlines,
	                                        JDIMENSION num_lines))m_lib.loadFunction("jpeg_write_scanlines");
	yang_jpeg_finish_compress=(void (*)(j_compress_ptr cinfo))m_lib.loadFunction("jpeg_finish_compress");
	yang_jpeg_destroy_compress=(void (*)(j_compress_ptr cinfo))m_lib.loadFunction("jpeg_destroy_compress");
	yang_jpeg_std_error=(struct jpeg_error_mgr * (*)(struct jpeg_error_mgr *err))m_lib.loadFunction("jpeg_std_error");
}

void YangJpegUtil::unloadLib(){
	yang_jpeg_CreateCompress=NULL;
	yang_jpeg_CreateDecompress=NULL;
	yang_jpeg_stdio_src=NULL;
	yang_jpeg_read_header=NULL;
	yang_jpeg_start_decompress=NULL;
	yang_jpeg_read_scanlines=NULL;
	yang_jpeg_finish_decompress=NULL;
	yang_jpeg_destroy_decompress=NULL;
	yang_jpeg_stdio_dest=NULL;
	yang_jpeg_set_defaults=NULL;
	yang_jpeg_set_quality=NULL;
	yang_jpeg_start_compress=NULL;
	yang_jpeg_write_scanlines=NULL;
	yang_jpeg_finish_compress=NULL;
	yang_jpeg_destroy_compress=NULL;
	yang_jpeg_std_error=NULL;

}
YangJpegUtil::YangJpegUtil() {

	unloadLib();
	m_lib.loadObject("libjpeg");
	loadLib();
}

YangJpegUtil::~YangJpegUtil() {
	unloadLib();
	m_lib.unloadObject();
}


void my_error_exit(j_common_ptr cinfo)
{
  my_error_ptr myerr = (my_error_ptr)cinfo->err;
 (*cinfo->err->output_message) (cinfo);
  /* Return control to the setjmp point32_t */
  longjmp(myerr->setjmp_buffer, 1);
}



 int32_t YangJpegUtil::readJpegFile(char* filename,YangJpegData *img) {
	  jpeg_decompress_struct t_cinfo;
	  jpeg_decompress_struct *cinfo=&t_cinfo;
	  struct my_error_mgr jerr;
	   FILE *infile;
	   JSAMPARRAY buffer;
	   int32_t row_stride;
	   if ((infile = fopen(filename, "rb")) == NULL) {
	     fprintf(stderr, "can't open %s\n", filename);
	     return 0;
	   }

	   cinfo->err = yang_jpeg_std_error(&jerr.pub);
	   jerr.pub.error_exit = my_error_exit;

	   if (setjmp(jerr.setjmp_buffer)) {
		   yang_jpeg_destroy_decompress(cinfo);
	     fclose(infile);
	     return 0;
	   }

	   yang_jpeg_CreateDecompress(cinfo, JPEG_LIB_VERSION, \
	                           (size_t)sizeof(struct jpeg_decompress_struct));
	   yang_jpeg_stdio_src(cinfo, infile);
	   (void)yang_jpeg_read_header(cinfo, TRUE);
	   (void)yang_jpeg_start_decompress(cinfo);
	   row_stride = cinfo->output_width * cinfo->output_components;
       //Make a one-row-high sample array that will go away when done with image
	   buffer = (*cinfo->mem->alloc_sarray)
	                 ((j_common_ptr)cinfo, JPOOL_IMAGE, row_stride, 1);
	  img->width=cinfo->output_width;
	  img->heigth=cinfo->output_height;
	  img->pixelByteCount=cinfo->output_components;//24;
	  img->size=img->width*img->heigth*img->pixelByteCount;
	  //printf("width==%d,height=%d,byteCount=%d,size=%d",img->width,img->heigth,img->pixelByteCount,img->size);
	  if(img->data==NULL) img->data=new uint8_t[img->size];
	  int32_t t_row=0;
	   while (cinfo->output_scanline < cinfo->output_height) {
		     (void)yang_jpeg_read_scanlines(cinfo, buffer, 1);
		     memcpy(img->data+row_stride*t_row,buffer[0],row_stride);
	  	   //  memcpy(img->data+row_stride*(img->heigth-1-t_row),buffer[0],row_stride);
	  	     t_row++;
	   }
	   (void)yang_jpeg_finish_decompress(cinfo);
	   yang_jpeg_destroy_decompress(cinfo);
	   fclose(infile);
	   cinfo=NULL;
	   return 1;
}


void YangJpegUtil::write_JPEG_file(char *filename, YangJpegData *img,int32_t quality)
 {
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

   yang_jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

   yang_jpeg_start_compress(&cinfo, TRUE);


   row_stride = img->width * 3;

   while (cinfo.next_scanline < cinfo.image_height) {
     row_pointer[0] = &img->data[cinfo.next_scanline * row_stride];
     (void)yang_jpeg_write_scanlines(&cinfo, row_pointer, 1);
   }

   yang_jpeg_finish_compress(&cinfo);

   fclose(outfile);

   yang_jpeg_destroy_compress(&cinfo);

 }
void YangJpegUtil::rgb2bgr(YangJpegData *rgb,YangJpegData *bgr){
	int32_t dataIsNull=(bgr->data==NULL);
	memcpy(bgr,rgb,sizeof(YangJpegData));
	if(dataIsNull) bgr->data=new uint8_t[bgr->size];
	memcpy(bgr->data,rgb->data,rgb->size);
	for(int32_t i=0;i<bgr->size;i+=3){
		bgr->data[i]=rgb->data[i+2];
		bgr->data[i+2]=rgb->data[i];
	}

}
void YangJpegUtil::bgr2rgb(YangJpegData *bgr,YangJpegData *rgb){
	int32_t dataIsNull=(rgb->data==NULL);
	memcpy(rgb,bgr,sizeof(YangJpegData));
	if(dataIsNull) rgb->data=new uint8_t[rgb->size];
	memcpy(rgb->data,bgr->data,rgb->size);
	for(int32_t i=0;i<bgr->size;i+=3){
		rgb->data[i]=bgr->data[i+2];
		rgb->data[i+2]=bgr->data[i];
	}
}
void YangJpegUtil::freeJpeg(YangJpegData *jpg){
	if(jpg->data){
		delete[] jpg->data;
		jpg->data=NULL;
	}
}


