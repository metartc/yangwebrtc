#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "yangavutil/video/YangBmp.h"

YangBmp::YangBmp()
{

}

YangBmp::~YangBmp()
{
    //dtor
}

void YangBmp::create_bmpheader(int32_t p_width,int32_t p_height){
//	printf("\nBITMAPFILEHEADER size=====%d\n",(int)sizeof(Yang_BITMAPFILEHEADER));
  bfh.bfType = (unsigned short)0x4D42;
  bfh.bfSize = (unsigned long)(14 + 40 + p_width * p_height*3);
  bfh.bfReserved1 = 0;
  bfh.bfReserved2 = 0;
  bfh.bfOffBits = (unsigned long)(14 + 40);
  bih.biBitCount = 24;
  bih.biWidth = p_width;
  bih.biHeight = p_height;
  bih.biSizeImage = p_width * p_height * 3;
  bih.biClrImportant = 0;
  bih.biClrUsed = 0;
  bih.biCompression = 0;
  bih.biPlanes = 1;
  bih.biSize = 40;//sizeof(BITMAPINFOHEADER);//40;//sizeof(bih);
  bih.biXPelsPerMeter = 0;//0x00000ec4;
  bih.biYPelsPerMeter = 0;//0x00000ec4;
}
void YangBmp::save_bmp(char* p_filename,char *p_addr,int32_t p_len)
{
    FILE *fp1 = fopen(p_filename, "wb");
    if (fp1 < 0) {
        printf("open frame data file failed\n");
        return;
    }
    fwrite(&bfh,sizeof(bfh),1,fp1);
    fwrite(&bih,sizeof(bih),1,fp1);
    fwrite(p_addr, 1, p_len, fp1);
    fclose(fp1);
   // printf("Change one frame saved ..bfh==%d,bih==%d\n", sizeof(bfh),sizeof(bih));
    return;
}
