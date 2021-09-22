/*
 *
 *
 *  Created on: 2020年8月8日
 *      Author: yang
 */
#include <string.h>
#include <malloc.h>
#include <yangutil/buffer/YangMediaBuffer.h>
#include <yangutil/yangavinfotype.h>

YangMediaBuffer::YangMediaBuffer() {
	resetIndex();
	m_cache_num = 5;
	m_mediaType = 1;
	m_uid = -1;
	m_frames = NULL;
	m_bufferManager = NULL;
	m_lock = PTHREAD_MUTEX_INITIALIZER;
}

YangMediaBuffer::~YangMediaBuffer() {
	if (m_frames) {
		for (int32_t i = 0; i < m_cache_num; i++) {
			yang_delete(m_frames[i]);
		}
		yang_deleteA(m_frames);
	}
	yang_delete(m_bufferManager);
}

void YangMediaBuffer::resetIndex() {
	m_putIndex = 0;
	m_getIndex = 0;
	m_size = 0;
}
void YangMediaBuffer::setSize(int32_t padd) {
	pthread_mutex_lock(&m_lock);
	if (padd)
		m_size++;
	else
		m_size--;
	if(m_size>m_cache_num) m_size=m_cache_num;
	if(m_size<0) m_size=0;
	pthread_mutex_unlock(&m_lock);
}

void YangMediaBuffer::initFrames(int32_t pnum, int unitsize) {
	if (m_bufferManager == NULL)		m_bufferManager = new YangBufferManager(pnum, unitsize);
	if (m_frames == NULL) {
		m_frames = new YangFrame*[pnum];
		for (int32_t i = 0; i < pnum; i++) {
			m_frames[i] = new YangFrame();
			m_frames[i]->payload = m_bufferManager->getBuffer();
		}
	}
}
void YangMediaBuffer::putFrame(YangFrame *pframe) {
	if (!pframe)		return;
	if (m_putIndex >= m_cache_num||m_putIndex<0)		m_putIndex = 0;
	yang_frame_copy_buffer(pframe, m_frames[m_putIndex++]);
	setSize(1);
}
void YangMediaBuffer::getFrame(YangFrame *pframe) {
	if (!pframe||!m_size)		return;
	if (m_getIndex >= m_cache_num||m_getIndex<0)		m_getIndex = 0;
	yang_frame_copy_buffer(m_frames[m_getIndex++], pframe);

	setSize(0);
}
uint8_t* YangMediaBuffer::getFrameRef(YangFrame *pframe) {
	if (!m_size||!pframe)		return NULL;
	if (m_getIndex >= m_cache_num||m_getIndex<0)		m_getIndex = 0;
	yang_frame_copy_nobuffer(m_frames[m_getIndex], pframe);
	uint8_t *p = m_frames[m_getIndex]->payload;
	m_getIndex++;
	setSize(0);
	return p;

}
YangFrame* YangMediaBuffer::getCurFrameRef() {
	if(!m_size) return NULL;
	return m_frames[m_getIndex];
}

