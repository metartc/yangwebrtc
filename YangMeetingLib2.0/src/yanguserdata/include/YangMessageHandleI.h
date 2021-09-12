/*
 * YangMessageTcpI.h
 *
 *  Created on: 2020年9月1日
 *      Author: yang
 */

#ifndef SRC_YANGDATACHANNEL_INCLUDE_YANGMESSAGETCPI_H_
#define SRC_YANGDATACHANNEL_INCLUDE_YANGMESSAGETCPI_H_

class YangMessageHandleI{
public:
	YangMessageHandleI(){}
	virtual ~YangMessageHandleI(){}
	virtual void decodeMessage(uint8_t* p,int32_t plen)=0;
};



#endif /* SRC_YANGDATACHANNEL_INCLUDE_YANGMESSAGETCPI_H_ */
