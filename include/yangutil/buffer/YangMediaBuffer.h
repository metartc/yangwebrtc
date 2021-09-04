/*
 * YangBaseAllocator.h
 *
 *  Created on: 2020年8月8日
 *      Author: yang
 */

#ifndef YANGUTIL_BUFFER_YANGBUFFER_H_
#define YANGUTIL_BUFFER_YANGBUFFER_H_
#include <mutex>
#include <stdint.h>
#include <yangutil/yangavinfotype.h>
#define yang_reindex(p)  if(p!=NULL) p->resetIndex();
struct YangMeidaBufferHeader{
	char headerLength;
	int32_t bodyLength;
};
class YangMediaBuffer {
public:
	YangMediaBuffer();
	virtual ~YangMediaBuffer();
public:
    int32_t m_index;
	int32_t m_size;
	int32_t m_mediaType;
	//int32_t m_userId;
	int32_t m_uid;
	void resetIndex();
	void putData(int32_t pHeadLen,uint8_t*pheader,int32_t bodyLen,uint8_t* body);
	int32_t getData(uint8_t*pheader,uint8_t* body);
	uint8_t* getData(uint8_t*pheader);
	int32_t getHeaderData(uint8_t*pheader);

protected:
	int32_t m_newIndex;
	int32_t m_curIndex;
	int32_t m_baseHeaderLen;
	YangMeidaBufferHeader *m_baseIn;
	YangMeidaBufferHeader *m_baseOut;
	int32_t m_alen,m_cache_num;//,m_cache_num_cap;
	uint8_t *m_cache,*m_tmp,*m_tmp1;
	void setSize(int32_t padd);
private:

	std::mutex m_lock;

	//int32_t m_bufHeaderLen;


};

#endif /* YANGUTIL_BUFFER_YANGBUFFER_H_ */
