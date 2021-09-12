/*
 * YangBaseAllocator.h
 *
 *  Created on: 2020年8月8日
 *      Author: yang
 */

#ifndef YANGUTIL_BUFFER_YANGBUFFER_H_
#define YANGUTIL_BUFFER_YANGBUFFER_H_
#include <pthread.h>
#include <stdint.h>
#include <yangutil/yangavinfotype.h>
#define yang_reindex(p)  if(p!=NULL) p->resetIndex();
class YangMediaBuffer {
public:
	YangMediaBuffer();
	virtual ~YangMediaBuffer();
public:
	int32_t m_mediaType;
	int32_t m_uid;
	int32_t m_size;
	void resetIndex();
protected:
	int32_t m_index;
	int32_t m_newIndex;
	//int32_t m_curIndex;
	int32_t m_cache_num;
	void setSize(int32_t padd);

	YangFrame** m_videoFrames;
	YangBufferManager* m_bufs;
protected:
	void initFrames(int pnum,int unitsize);

	void putFrame(YangFrame* pframe);
	void getFrame(YangFrame* pframe);
	YangFrame* getCurFrameRef();
	uint8_t* getFrameRef(YangFrame* pframe);
private:
	pthread_mutex_t m_lock;
};

#endif /* YANGUTIL_BUFFER_YANGBUFFER_H_ */
