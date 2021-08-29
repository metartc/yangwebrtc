#ifndef YangPicUtilFfmpeg___
#define YangPicUtilFfmpeg___
#include "yangutil/sys/YangLoadLib.h"
#include "../../Yang_Config.h"
#if HavePicUtilFfmpeg
//extern "C" {
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"
//}
class YangPicUtilFfmpeg
{
public:
	YangPicUtilFfmpeg(void);
    ~YangPicUtilFfmpeg(void);
    int inWidth,inHeight,outWidth,outHeight;
    void resize();
    void init(int pinWidth,int pinHeight,int poutWidth,int poutHeight,AVPixelFormat src,AVPixelFormat dst);
    void init(int pinWidth,int pinHeight,int poutWidth,int poutHeight);
    void initYuy2(int pinWidth,int pinHeight,int poutWidth,int poutHeight);
    void initNv12(int pinWidth,int pinHeight,int poutWidth,int poutHeight);
    void init420P(int pinWidth,int pinHeight,int poutWidth,int poutHeight);
    void initYuy2_Bgr24(int pinWidth,int pinHeight,int poutWidth,int poutHeight);
    void initBgr24_Yuy2(int pinWidth,int pinHeight,int poutWidth,int poutHeight);
    void getAddr(unsigned char **p_in,unsigned char **p_out);
    void closeAll();

    struct SwsContext *img_convert_ctx;
    AVPicture in_pic, out_pic;
private:

    YangLoadLib m_lib,m_lib1;
    	void loadLib();
    	void unloadLib();
    	struct SwsContext *(*yang_sws_getContext)(int srcW, int srcH, enum AVPixelFormat srcFormat,
    		                                  int dstW, int dstH, enum AVPixelFormat dstFormat,
    		                                  int flags, SwsFilter *srcFilter,
    		                                  SwsFilter *dstFilter, const double *param);

    		int (*yang_sws_scale)(struct SwsContext *c, const uint8_t *const srcSlice[],
    		              const int srcStride[], int srcSliceY, int srcSliceH,
    		              uint8_t *const dst[], const int dstStride[]);
    		void (*yang_sws_freeContext)(struct SwsContext *swsContext);




    		int (*yang_avpicture_alloc)(AVPicture *picture, enum AVPixelFormat pix_fmt, int width, int height);
    		void (*yang_avpicture_free)(AVPicture *picture);
};
#endif
#endif

