/*
 * YangCommon.h
 *
 *  Created on: 2021年2月24日
 *      Author: yang
 */

#ifndef YANGDATASERVER_YANGCOMMON_H_
#define YANGDATASERVER_YANGCOMMON_H_

struct YangUser {
	int userId; //char userId[15];
	char userName[30];
	char loginName[30];
	int myId;
	int roomAdmin;
	int micFlag;
	int camFlag;
};
struct YangMeeting {
	int meetingId;
	char meetingName[35];
	int departId;
	int meetingTime = 0;
	int roomAdmin = 0;
};



#endif /* YANGDATASERVER_YANGCOMMON_H_ */
