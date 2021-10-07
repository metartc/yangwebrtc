/*
 * YangBaseAllocator.h
 *
 *  Created on: 2020年8月8日
 *      Author: yang
 */

#ifndef YANGUTIL_BUFFER_YANGBUFFER_H_
#define YANGUTIL_BUFFER_YANGBUFFER_H_
#include <pthread.h>
#include <yangutil/yangtype.h>
#include <yangutil/yangavtype.h>
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
	int32_t m_putIndex;
	int32_t m_getIndex;
	int32_t m_cache_num;

	int32_t m_nextIndex;
	void setSize(int32_t padd);

	YangFrame** m_frames;
	YangBufferManager* m_bufferManager;
protected:
	void initFrames(int pnum,int unitsize);

	void putFrame(YangFrame* pframe);
	void getFrame(YangFrame* pframe);
	YangFrame* getCurFrameRef();
	int64_t getNextFrameTimestamp();
	uint8_t* getFrameRef(YangFrame* pframe);
private:
	pthread_mutex_t m_lock;
};

#endif /* YANGUTIL_BUFFER_YANGBUFFER_H_ */
