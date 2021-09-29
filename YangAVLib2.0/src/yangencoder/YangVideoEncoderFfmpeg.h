#ifndef YANGH264DECODERFFMPEG_H
#define YANGH264DECODERFFMPEG_H
#include <yangutil/yangavinfotype.h>
#include "yangencoder/YangVideoEncoder.h"
#include "yangutil/sys/YangLoadLib.h"

extern "C"{
#include "libavcodec/avcodec.h"
//#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
}
class YangVideoEncoderFfmpeg : public YangVideoEncoder
{
    public:
         YangVideoEncoderFfmpeg(int32_t pencType,int32_t phwtype);
         ~YangVideoEncoderFfmpeg();
    	void init(YangVideoInfo *pvp,YangVideoEncInfo *penc);
    	static void initParam(AVCodecContext *p_codecCtx,YangVideoInfo *pvp,YangVideoEncInfo *penc);
    	static YangVideoHwType g_hwType;
    	//void encode(uint8_t *src,int32_t buflen, uint8_t *dest, int32_t *destLen, int32_t *frametype);
    	//int32_t encode(uint8_t *buf, int32_t buflen, uint8_t *dest, int32_t *destLen,int32_t *frametype);
    	int32_t encode(YangFrame* pframe, YangEncoderCallback* pcallback);
    	void setVideoMetaData(YangVideoMeta *pvmd);
    	void sendKeyFrame();
        void parseRtmpHeader(uint8_t *p,int32_t pLen,int32_t *pwid,int32_t *phei,int32_t *pfps);
    protected:

    private:
        int32_t ret;
        YangVideoEncDecType m_encoderType;
        int32_t m_sendKeyframe;
        AVCodec *m_codec;
        AVCodecContext *m_codecCtx = NULL;
        AVPacket packet;
        AVFrame	*m_frame;
        YangLoadLib m_lib,m_lib1;
        int32_t yLen;
        int32_t uLen;
        int32_t allLen;
    	uint8_t* buffer =NULL;
    	int32_t usingVaapi;
    	AVBufferRef *hw_device_ctx;
        AVFrame *frame_mem_gpu;
    	int32_t set_hwframe_ctx(AVPixelFormat ctxformat,AVPixelFormat swformat,YangVideoInfo *yvp,AVCodecContext *ctx, AVBufferRef *hw_device_ctx,int32_t pwid,int32_t phei);
    private:
    	void encode_close();
    	void loadLib();
    	void unloadLib();
    	AVBufferRef *(*yang_av_hwframe_ctx_alloc)(AVBufferRef *device_ctx);
    	int32_t (*yang_av_hwframe_ctx_init)(AVBufferRef *ref);
    	void (*yang_av_buffer_unref)(AVBufferRef **buf);
    	AVCodec *(*yang_avcodec_find_encoder_by_name)(const char *name);
    	int32_t (*yang_av_hwdevice_ctx_create)(AVBufferRef **device_ctx, enum AVHWDeviceType type,
    	                           const char *device, AVDictionary *opts, int32_t flags);
    	AVFrame *(*yang_av_frame_alloc)(void);
    	int32_t (*yang_avcodec_open2)(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);
    	int32_t (*yang_av_image_get_buffer_size)(enum AVPixelFormat pix_fmt, int32_t width, int32_t height, int32_t align);
    	void *(*yang_av_malloc)(size_t size);
    	int32_t (*yang_av_image_fill_arrays)(uint8_t *dst_data[4], int32_t dst_linesize[4],
    	                         const uint8_t *src,
    	                         enum AVPixelFormat pix_fmt, int32_t width, int32_t height, int32_t align);
    	void (*yang_av_init_packet)(AVPacket *pkt);
    	int32_t (*yang_av_hwframe_get_buffer)(AVBufferRef *hwframe_ctx, AVFrame *frame, int32_t flags);
    	int32_t (*yang_av_hwframe_transfer_data)(AVFrame *dst, const AVFrame *src, int32_t flags);
    	int32_t (*yang_avcodec_send_frame)(AVCodecContext *avctx, const AVFrame *frame);
    	int32_t (*yang_avcodec_receive_packet)(AVCodecContext *avctx, AVPacket *avpkt);
    	void (*yang_av_frame_free)(AVFrame **frame);
    	int32_t (*yang_avcodec_close)(AVCodecContext *avctx);
    	void (*yang_av_free)(void *ptr);
    	AVBufferRef *(*yang_av_buffer_ref)(AVBufferRef *buf);
    	AVCodecContext *(*yang_avcodec_alloc_context3)(const AVCodec *codec);

};

#endif // YANGH264DECODERFFMPEG_H
