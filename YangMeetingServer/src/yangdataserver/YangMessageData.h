/*
 * YangMessage.h
 *
 *  Created on: 2020年9月1日
 *      Author: yang
 */

#ifndef YANGDATASERVER_YANGMESSAGEDATA_H_
#define YANGDATASERVER_YANGMESSAGEDATA_H_
#include "../yangsrt/include/YangTsMuxer.h"
enum YangMessageType{
	Yang_Message_Command,
	Yang_Message_RoomInfoUsers,
	Yang_Message_ChatText,
	Yang_Message_Whiteboard,
	Yang_Message_Other
};
struct YangMessage{
	YangMessageType messageType;
	unsigned int len;
	unsigned char *data;
};
class YangMessageData {
public:
	YangMessageData();
	virtual ~YangMessageData();
	void putMessage(YangMessageType ptype,unsigned char *p,int plen,vector<YangTsBuffer> *psb);
	void putMessageWithPmt(YangMessageType ptype,unsigned char *p,int plen,vector<YangTsBuffer> *psb);
	void putMessage(YangMessage *p,vector<YangTsBuffer> *psb);
	void getMessage(unsigned char *p,int plen,YangMessage *mes);
private:
	YangTsMuxer m_mux;

	unsigned char* yang_put_be32(unsigned char *output, unsigned int nVal);
	unsigned int yang_get_be32(unsigned char *output);

};

#endif /* YANGDATASERVER_YANGMESSAGEDATA_H_ */
