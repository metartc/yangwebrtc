/*
 * YangMessage.cpp
 *
 *  Created on: 2020年9月1日
 *      Author: yang
 */
#include <string.h>
#include <yangdataserver/YangMessageData.h>

YangMessageData::YangMessageData() {


}

YangMessageData::~YangMessageData() {

}
unsigned char* YangMessageData::yang_put_be32(unsigned char *output, unsigned int nVal) {
	output[3] = nVal & 0xff;
	output[2] = nVal >> 8;
	output[1] = nVal >> 16;
	output[0] = nVal >> 24;
	return output + 4;
}
unsigned int YangMessageData::yang_get_be32(unsigned char *output) {
	//int nVal=0;
	return output[3]|output[2]<<8|output[1]<<16|output[0]<<24;

	//return nVal;
}

void YangMessageData::putMessage(YangMessageType ptype,unsigned char *p,int plen,vector<YangTsBuffer> *psb){
	unsigned char s[plen+20]={0};
	s[0]=ptype;
	yang_put_be32(s+1,plen);
	memcpy(s+5,p,plen);
	m_mux.encode((unsigned char*)s,plen+5,0,0,TS_PRIVATE,psb);
}
void YangMessageData::putMessageWithPmt(YangMessageType ptype,unsigned char *p,int plen,vector<YangTsBuffer> *psb){
	unsigned char s[plen+20]={0};
	s[0]=ptype;
	yang_put_be32(s+1,plen);
	memcpy(s+5,p,plen);
	m_mux.encodeWithPmt((unsigned char*)s,plen+5,0,0,TS_PRIVATE,psb);
}
void YangMessageData::putMessage(YangMessage *p,vector<YangTsBuffer> *psb){
	unsigned char s[p->len+20]={0};
	s[0]=p->messageType;
	yang_put_be32(s+1,p->len);
	memcpy(s+5,p->data,p->len);
	m_mux.encode((unsigned char*)s,p->len+5,0,0,TS_PRIVATE,psb);
}
void YangMessageData::getMessage(unsigned char *p,int plen,YangMessage *mes){
	mes->messageType=(YangMessageType)(*p);
	mes->len=yang_get_be32(p+1);
	mes->data=p+5;
}
