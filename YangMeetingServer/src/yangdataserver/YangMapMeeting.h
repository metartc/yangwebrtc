/*
 * YangMeetingMap.h
 *
 *  Created on: 2020年8月24日
 *      Author: yang
 */

#ifndef YANGDATASERVER_YANGMAPMEETING_H_
#define YANGDATASERVER_YANGMAPMEETING_H_
#include <yangdataserver/YangUserContext.h>
#include <yangdataserver/YangMessageData.h>
//#include <map>
//#include <list>
#include <vector>
#include "YangServerMeeting.h"
//#include "SLSSrt.hpp"
using namespace std;
#define Yang_Using_SrtDataServer 0
class YangMapMeeting {
public:
	YangMapMeeting();
	virtual ~YangMapMeeting();
	void split( string s,vector<string>* sv,const char flag );

	int putUser(int pmeetingId,int puserid,char *para);
	int removeUser(int pmeetingId,int puserid);
	void getUserStr(int pmeetingId,char** pstr);
	//void sendUserSrt(int pmeetingId,CSLSSrt *psrt);
	vector<YangTsBuffer>* getUsersStr(int pmeetingId);
	//void commandHandle(int pmeetingId,unsigned char* p,int plen);
	YangMessageData m_message;

	int m_usingSrtDataChannel;

	static void clearMap();
	st_netfd_t m_fd;
protected:
	static vector<YangServerMeeting*> m_meetingList;
	YangUser* getUser(int pmeetingId,int puserid);
	YangServerMeeting *getMeeting(int pmeetingId);



};

#endif /* YANGDATASERVER_YANGMAPMEETING_H_ */
