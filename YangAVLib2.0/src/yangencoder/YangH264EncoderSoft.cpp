#include "malloc.h"
#include <unistd.h>
#include <yangutil/yangavinfotype.h>
#include "YangH264EncoderSoft.h"
#include "stdlib.h"

#define HEX2BIN(a)      (((a)&0x40)?((a)&0xf)+9:((a)&0xf))

int32_t YangH264EncoderSoft::hex2bin(char *str, char **hex) {
	char *ptr;
	int32_t i, l = strlen(str);
	if (l & 1)
		return 0;
	*hex = (char*) malloc(l / 2);
	ptr = *hex;
	if (!ptr)
		return 0;
	for (i = 0; i < l; i += 2)
		*ptr++ = (HEX2BIN(str[i]) << 4) | HEX2BIN(str[i + 1]);
	return l / 2;
}

void YangH264EncoderSoft::loadLib() {
	yang_x264_param_default = (void (*)(x264_param_t*)) m_lib.loadFunction(
			"x264_param_default");
	yang_x264_param_default_preset = (int32_t (*)(x264_param_t*, const char *preset,
			const char *tune)) m_lib.loadFunction("x264_param_default_preset");
	yang_x264_param_apply_profile =
			(int32_t (*)(x264_param_t*, const char *profile)) m_lib.loadFunction(
					"x264_param_apply_profile");
	char s[30];
	memset(s, 0, 30);
	sprintf(s, "x264_encoder_open_%d", X264_BUILD);
	yang_x264_encoder_open = (x264_t* (*)(x264_param_t*)) m_lib.loadFunction(s);
	yang_x264_picture_alloc = (int32_t (*)(x264_picture_t *pic, int32_t i_csp,
			int32_t i_width, int32_t i_height)) m_lib.loadFunction(
			"x264_picture_alloc");
	yang_x264_encoder_encode =
			(int32_t (*)(x264_t*, x264_nal_t **pp_nal, int32_t *pi_nal,
					x264_picture_t *pic_in, x264_picture_t *pic_out)) m_lib.loadFunction(
					"x264_encoder_encode");
	yang_x264_picture_clean =
			(void (*)(x264_picture_t *pic)) m_lib.loadFunction(
					"x264_picture_clean");
	yang_x264_encoder_close = (void (*)(x264_t*)) m_lib.loadFunction(
			"x264_encoder_close");
}
void YangH264EncoderSoft::unloadLib() {
	yang_x264_param_default_preset = NULL;
	yang_x264_param_default = NULL;
	yang_x264_param_apply_profile = NULL;
	yang_x264_picture_alloc = NULL;
	yang_x264_encoder_encode = NULL;
	yang_x264_picture_clean = NULL;
	yang_x264_encoder_close = NULL;
	yang_x264_encoder_open = NULL;
}
YangH264EncoderSoft::YangH264EncoderSoft() {
	m_nal = NULL;
	m_264Nal = NULL;
	m_sendKeyframe=0;
	m_264Pic = NULL;
	m_264Handle = NULL;
	nal_len = 0;
	destLength = 0;
	m_i264Nal = 0;
	nal_len = 0;
	unloadLib();
}

YangH264EncoderSoft::~YangH264EncoderSoft(void) {
	yang_x264_picture_clean(m_264Pic);
	yang_x264_encoder_close(m_264Handle);
	m_264Pic = NULL;
	m_264Handle = NULL;
	m_264Nal = NULL;

	m_nal = NULL;
	unloadLib();
	m_lib.unloadObject();
//	m_lib1.unloadObject();

}
void YangH264EncoderSoft::sendKeyFrame(){
	m_sendKeyframe=1;

}

void YangH264EncoderSoft::setVideoMetaData(YangVideoMeta *pvmd) {

}
void YangH264EncoderSoft::initX264Param(YangVideoInfo *pvp,
		YangVideoEncInfo *penc, x264_param_t *param) {
	param->i_width = pvp->outWidth;   //set frame width
	param->i_height = pvp->outHeight;  //set frame height
	param->i_bframe = 0;
	param->b_cabac = 0;
	param->i_bitdepth = pvp->bitDepth;
	//param->analyse.i_weighted_pred = X264_WEIGHTP_NONE;
	//	param->analyse.b_weighted_bipred = 0;

	param->b_interlaced = 0;
	//param->rc.i_rc_method = X264_RC_ABR; //X264_RC_CQP ;////X264_RC_CQP  X264_RC_CRF
	param->i_level_idc = penc->level_idc;
	//param->rc.i_bitrate = pvp->rate;//512;
	//printf("\npreset====%d...................\n",penc->preset);
	if (penc->preset > 4) {
		param->rc.i_qp_min = 10;
		param->rc.i_qp_max = 30;
		param->rc.i_qp_constant = 26;
	}

	param->i_fps_num = pvp->frame;  //30;
	param->i_fps_den = 1;
	param->i_keyint_max = pvp->frame * 3;
	param->i_keyint_min = pvp->frame;
	param->i_log_level = X264_LOG_ERROR; //X264_LOG_NONE;//X264_LOG_DEBUG;//X264_LOG_NONE;
	param->i_threads=penc->enc_threads;
	//printf("\n..................ecnThreads==%d",param->i_threads);
	//param->analyse.i_me_method=X264_ME_DIA;
	param->rc.b_mb_tree = 0;

}
void YangH264EncoderSoft::init(YangVideoInfo *pvp, YangVideoEncInfo *penc) {
	if (m_isInit == 1)
		return;
	m_lib.loadObject("libx264");
	loadLib();
	setVideoPara(pvp, penc);
	x264_param_t *param = new x264_param_t();
	if (penc->preset < 5)
		yang_x264_param_default_preset(param, x264_preset_names[penc->preset],
				"zerolatency");
	else
		yang_x264_param_default(param);
	//if (penc->preset < 5)
	//	yang_x264_param_default_preset(param, x264_preset_names[penc->preset],	x264_tune_names[7]);
	initX264Param(pvp, penc, param);
	//param->b_repeat_headers=0;
	yang_x264_param_apply_profile(param, x264_profile_names[0]);
	m_264Pic = new x264_picture_t();
	memset(m_264Pic, 0, sizeof(x264_picture_t));
	//set default param
	param->b_sliced_threads = 0;
	//param->i_threads = penc->enc_threads;
	if ((m_264Handle = yang_x264_encoder_open(param)) == NULL) {
		printf("RE init x264_encoder_open failed\n");
		exit(1);
	}
	int32_t x264Format = pvp->bitDepth == 8 ? X264_CSP_I420 : X264_CSP_HIGH_DEPTH;
	//x264_picture_alloc
	yang_x264_picture_alloc(m_264Pic, x264Format,param->i_width, param->i_height);
	m_264Pic->i_type = X264_TYPE_AUTO;

	m_isInit = 1;
	delete param;
	param = NULL;

}

int32_t YangH264EncoderSoft::encode(YangFrame* pframe, YangEncoderCallback* pcallback) {
	memcpy(m_264Pic->img.plane[0], pframe->payload,pframe->nb);
	m_nal = m_vbuffer;
		destLength = 0;
		int32_t frametype = 0;
		if(m_sendKeyframe==1) {
			m_sendKeyframe=2;
			m_264Pic->i_type = X264_TYPE_IDR;//X264_TYPE_AUTO;
			printf("\n1************************sendkey.......................\n");
		}
		if (yang_x264_encoder_encode(m_264Handle, &m_264Nal, &m_i264Nal, m_264Pic,
				&pic_out) < 0) {
			fprintf( stderr, "x264_encoder_encode failed/n");
		}

		for (int32_t i = 0; i < m_i264Nal; i++) {
			m_nal = m_264Nal[i].p_payload;
			if (m_nal[3] == 0x65) {
				nal_len = 3;
			} else {
				if ((m_nal[4] & 0x60) == 0 || m_nal[4] == 0x67 || m_nal[4] == 0x68)
					continue;
				nal_len = 4;
			}

			if (m_264Nal[i].i_type == NAL_SLICE_IDR)	frametype = 1;
			memcpy(m_vbuffer + destLength, m_nal + nal_len,
					m_264Nal[i].i_payload - nal_len);
			destLength += (m_264Nal[i].i_payload - nal_len);

		}
		pframe->payload=m_vbuffer;
			pframe->frametype=frametype;
			pframe->nb=destLength;
		if(pcallback) pcallback->onVideoData(pframe);
		if(m_sendKeyframe==2) {
			m_264Pic->i_type = X264_TYPE_AUTO;//X264_TYPE_AUTO;
			m_sendKeyframe=0;
			printf("\n2************************sendkey.......................frametype==%d\n",frametype);
		}

	return 1;
}


