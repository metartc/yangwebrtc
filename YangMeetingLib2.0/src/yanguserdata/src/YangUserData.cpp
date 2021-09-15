/*
 * YangMessage.cpp
 *
 *  Created on: 2020年9月1日
 *      Author: yang
 */
#include <string.h>
#include <yangutil/YangEndian.h>
#include <yanguserdata/include/YangUserData.h>

YangUserData::YangUserData() {


}

YangUserData::~YangUserData() {

}

void YangUserData::putMessage(YangMessageType ptype,uint8_t *p,int32_t plen,vector<YangTsBuffer> *psb){
	uint8_t s[plen+20]={0};
	s[0]=ptype;
	yang_put_be32((char*)s+1,plen);
	memcpy(s+5,p,plen);
	m_mux.encode((uint8_t*)s,plen+5,0,0,TS_PRIVATE,psb);
}
void YangUserData::putMessage(YangUserDataType *p,vector<YangTsBuffer> *psb){
	uint8_t s[p->len+20]={0};
	s[0]=p->messageType;
	yang_put_be32((char*)s+1,p->len);
	memcpy(s+5,p->data,p->len);
	m_mux.encode((uint8_t*)s,p->len+5,0,0,TS_PRIVATE,psb);
}
void YangUserData::getMessage(uint8_t *p,int32_t plen,YangUserDataType *mes){

	mes->messageType=(YangMessageType)(*p);
	mes->len=yang_get_be32(p+1);

	mes->data=p+5;
}
