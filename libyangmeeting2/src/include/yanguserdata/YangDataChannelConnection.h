/*
 * YangUserDataConnection.h
 *
 *  Created on: 2020年9月1日
 *      Author: yang
 */

#ifndef SRC_YANGUSERDATA_INCLUDE_YANGDATACHANNELCONNECTION_H_
#define SRC_YANGUSERDATA_INCLUDE_YANGDATACHANNELCONNECTION_H_

#ifdef _WIN32
#include <winsock2.h>

#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include "yangutil/sys/YangThread.h"
#include "YangMessageHandleI.h"


class YangDataChannelConnection :public YangThread{
public:
	YangDataChannelConnection();
	virtual ~YangDataChannelConnection();
	virtual void decodeMessage(uint8_t* p,int32_t plen)=0;
	int32_t init(char* serverip,int32_t pport);
	int32_t connectServer();
	int32_t disConnectServer();
	int32_t publish(uint8_t* p,int32_t plen);
    int32_t sendHeartbeat();

    int32_t m_isStart;
    void stop();

protected:
   void run();
   void startLoop();
   void stopLoop();
private:
   int32_t m_loop;
   uint8_t m_heartBuf[3];
 #ifdef _WIN32
   SOCKET m_socket;
   SOCKADDR_IN m_serverAddress;
 #else
   int32_t m_socket;
    struct sockaddr_in m_serverAddress;
  #endif

};

#endif /* SRC_YANGUSERDATA_INCLUDE_YANGDATACHANNELCONNECTION_H_ */
