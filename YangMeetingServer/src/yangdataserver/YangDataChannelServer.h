/*
 * YangServerAdapter.h
 *
 *  Created on: 2020年09月30日
 *      Author: yang
 */

#ifndef YANGSERVERADAPTER_H_
#define YANGSERVERADAPTER_H_
#include "st.h"
#include "YangMapMeeting.h"
#include "YangThread.h"
#include "../yangsql/include/YangConf.h"
class YangDataChannelServer:public YangThread {
public:
	YangDataChannelServer();
	virtual ~YangDataChannelServer();

	static void* handle_request(void *arg);
	static YangMapMeeting *userData;
	YangConf *m_conf;
	void init(int pport);
	void tcpServer();
	void stopServer();
    int m_isStart;
     void stop();
protected:
   void run();

private:
	st_netfd_t m_fd;
	int m_port;
	int m_loop;
};

#endif /* YANGSERVERADAPTER_H_ */
