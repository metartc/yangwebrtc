#ifndef YANGBMP_H
#define YANGBMP_H
#include "stdint.h"
#define yang_long unsigned int
#pragma pack(1)
 struct Yang_BITMAPFILEHEADER
{
  unsigned short bfType;//位图文件的类型,
  unsigned int bfSize;//位图文件的大小，以字节为单位
  unsigned short bfReserved1;//位图文件保留字，必须为0
  unsigned short bfReserved2;//同上
  unsigned int bfOffBits;//位图阵列的起始位置，以相对于位图文件   或者说是头的偏移量表示，以字节为单位
  // short bfReserved3;//同上
} ;



 struct BITMAPINFOHEADER//位图信息头类型的数据结构，用于说明位图的尺寸
{
 yang_long biSize;//位图信息头的长度，以字节为单位
 yang_long biWidth;//位图的宽度，以像素为单位
 yang_long biHeight;//位图的高度，以像素为单位
  unsigned short biPlanes;//目标设备的级别,必须为1
  unsigned short biBitCount;//每个像素所需的位数，必须是1(单色),4(16色),8(256色)或24(2^24色)之一
 yang_long biCompression;//位图的压缩类型，必须是0-不压缩，1-BI_RLE8压缩类型或2-BI_RLE4压缩类型之一
 yang_long biSizeImage;//位图大小，以字节为单位
 yang_long biXPelsPerMeter;//位图目标设备水平分辨率，以每米像素数为单位
 yang_long biYPelsPerMeter;//位图目标设备垂直分辨率，以每米像素数为单位
 yang_long biClrUsed;//位图实际使用的颜色表中的颜色变址数
 yang_long biClrImportant;//位图显示过程中被认为重要颜色的变址数
} ;
#pragma pack()
class YangBmp
{
    public:
        YangBmp();
        virtual ~YangBmp();
    public:
        void create_bmpheader(int p_width,int p_height);
        void save_bmp(char* p_filename,char *p_addr,int p_len);
    protected:

    private:
        struct Yang_BITMAPFILEHEADER bfh;
        struct BITMAPINFOHEADER bih;
};

#endif // YANGBMP_H
