//#include "stdafx.h"
#include "yangutil/buffer/YangVideoDecoderBuffer.h"

#include <stdlib.h>
#include "memory.h"




YangVideoDecoderBuffer::YangVideoDecoderBuffer(int num) {
	resetIndex();
	m_cache_num = num;
	initFrames(m_cache_num,1024 * 512);
}

YangVideoDecoderBuffer::~YangVideoDecoderBuffer(void) {

}

void YangVideoDecoderBuffer::getEVideo(YangFrame* pframe) {
	getFrame(pframe);
}

void YangVideoDecoderBuffer::putEVideo(YangFrame* pframe) {
	putFrame(pframe);
}

