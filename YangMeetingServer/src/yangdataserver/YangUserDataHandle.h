/*
 * YangUserDataHandle.h
 *
 *  Created on: 2020年10月1日
 *      Author: yang
 */

#ifndef YANGDATASERVER_YANGUSERDATAHANDLE_H_
#define YANGDATASERVER_YANGUSERDATAHANDLE_H_
#include "YangMapMeeting.h"
#include "../yangsrt/include/YangTsdemux.h"
#include <st.h>
class YangUserDataHandle :public YangMapMeeting,public ts_media_data_callback_I{
public:
	YangUserDataHandle(st_netfd_t pfd);
	virtual ~YangUserDataHandle();
	void on_data_callback(SRT_DATA_MSG_PTR data_ptr, unsigned int media_type,
					uint64_t dts, uint64_t pts);
	void startLoop();
	void userCommandHandle(unsigned char* p,int plen);
	void sendUsersStr(int pmeetingId);
	void sendCommand(int pmeetingId,vector<YangTsBuffer> *vec);
	void sendLogout(int pmeetingId);
	void sendUserCommond(int commandId);



public:
	int m_meetingId,m_userId;
	int m_loop;

private:
	int notLogin;
	int hasLoginClient;
};

#endif /* YANGDATASERVER_YANGUSERDATAHANDLE_H_ */
