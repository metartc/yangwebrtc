/*
 * YangMessage.h
 *
 *  Created on: 2020年9月1日
 *      Author: yang
 */

#ifndef YANGSRT_INCLUDE_YANGMESSAGEDATA_H_
#define YANGSRT_INCLUDE_YANGMESSAGEDATA_H_
#include "yangsrt/YangTsMuxer.h"
enum YangMessageType{
	Yang_Message_Command,
	Yang_Message_RoomInfoUsers,
	Yang_Message_ChatText,
	Yang_Message_Whiteboard,
	Yang_Message_Other
};
struct YangUserDataType{
	YangMessageType messageType;
	uint32_t  len;
	uint8_t *data;
};
class YangUserData {
public:
	YangUserData();
	virtual ~YangUserData();
	void putMessage(YangMessageType ptype,uint8_t *p,int32_t plen,vector<YangTsBuffer> *psb);
	void putMessage(YangUserDataType *p,vector<YangTsBuffer> *psb);
	void getMessage(uint8_t *p,int32_t plen,YangUserDataType *mes);
private:
	YangTsMuxer m_mux;


};

#endif /* YANGSRT_INCLUDE_YANGMESSAGEDATA_H_ */
