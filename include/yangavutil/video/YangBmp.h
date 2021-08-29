#ifndef YangBmp_H_
#define YangBmp_H_

#pragma pack(1)
struct Yang_BITMAPFILEHEADER {
        unsigned short bfType;
        unsigned int bfSize;
        unsigned short bfReserved1;
        unsigned short bfReserved2;
        unsigned int bfOffBits;

};

struct Yang_BITMAPINFOHEADER
{
        unsigned int biSize;
        unsigned int biWidth;
        unsigned int biHeight;
        unsigned short biPlanes;
        unsigned short biBitCount;
        unsigned int biCompression;
        unsigned int biSizeImage;
        unsigned int biXPelsPerMeter;
        unsigned int biYPelsPerMeter;
        unsigned int biClrUsed;
        unsigned int biClrImportant;
};
#pragma pack()

class YangBmp {
public:
        YangBmp();
        virtual ~YangBmp();
public:
        void create_bmpheader(int p_width, int p_height);
        void save_bmp(char *p_filename, char *p_addr, int p_len);
protected:

private:
        struct Yang_BITMAPFILEHEADER bfh;
        struct Yang_BITMAPINFOHEADER bih;
};

#endif
