/*
 * YangAudioUtil.h
 *
 *  Created on: 2020年9月28日
 *      Author: yang
 */

#ifndef SRC_YANGUTIL_YANGAUDIOUTIL_H_
#define SRC_YANGUTIL_YANGAUDIOUTIL_H_
#include <stdint.h>
#include <stddef.h>
int32_t single2Double(short *pData, short *dData, int32_t nSize);
uint64_t Resample_s16(const int16_t *input, int16_t *output, int32_t inSampleRate, int32_t outSampleRate, uint64_t inputSize,
                      uint32_t channels);
short FloatS16ToS16(float v);
int16_t maxAbsValueW16C(const int16_t* vector, size_t length);
void ComputeLevel(const int16_t* data, size_t length);

#endif /* SRC_YANGUTIL_YANGAUDIOUTIL_H_ */
