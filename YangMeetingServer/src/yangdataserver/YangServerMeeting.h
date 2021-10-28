
#ifndef YANGDATASERVER_YANGSERVERMEETING_H_
#define YANGDATASERVER_YANGSERVERMEETING_H_
#include "YangUserContext.h"
#include <vector>
#include <string>
using namespace std;
class YangServerMeeting {
public:
	YangServerMeeting();
	virtual ~YangServerMeeting();
	int m_meetingId;
	YangMeeting m_meeting;
	vector<YangUserContext*> m_users;


public:
	YangUserContext* getUser(int puserid);
	int getUserCount();
	string getUsersStr();
	int putUser(int puserid,char *para,st_netfd_t pfd);
	int removeUser(int puserid);
	void split( string s,vector<string>* sv,const char flag );

};

#endif /* YANGDATASERVER_YANGSERVERMEETING_H_ */
