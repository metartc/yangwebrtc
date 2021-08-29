/*
 * YangResample.h
 *
 *  Created on: 2020年9月10日
 *      Author: yang
 */

#ifndef YANGAUDIOPROC_YANGRESAMPLE_H_
#define YANGAUDIOPROC_YANGRESAMPLE_H_
#include "speex/speex_resampler.h"
#include "yangutil/sys/YangLoadLib.h"
class YangResample {
public:
	YangResample();
	virtual ~YangResample();
	void resample(const short *pin,unsigned int *pinLen,short* pout,unsigned int *poutLen);
	void init(int pchannel,int pinsample,int poutsample);
	int m_in_sample,m_out_sample;
	int m_init;
	int m_channel;
	SpeexResamplerState *m_state;
private:
	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
	SpeexResamplerState *(*yang_speex_resampler_init)(spx_uint32_t nb_channels,spx_uint32_t in_rate, spx_uint32_t out_rate,int quality,int *err);
	int (*yang_speex_resampler_process_int)(SpeexResamplerState *st,spx_uint32_t channel_index,const spx_int16_t *in,
	                                 spx_uint32_t *in_len,spx_int16_t *out,spx_uint32_t *out_len);
	int (*yang_speex_resampler_process_interleaved_int)(SpeexResamplerState *st, const spx_int16_t *in,spx_uint32_t *in_len,
	                                             spx_int16_t *out,spx_uint32_t *out_len);
	void (*yang_speex_resampler_destroy)(SpeexResamplerState *st);
};

#endif /* YANGAUDIOPROC_YANGRESAMPLE_H_ */
