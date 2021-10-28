/*
 * YangServerMeeting.cpp
 *
 *  Created on: 2021年4月13日
 *      Author: yang
 */

#include "YangServerMeeting.h"
#include <stdio.h>
#include <string>
#include <string.h>
#include <sstream>
YangServerMeeting::YangServerMeeting() {
	m_meetingId=0;

}

YangServerMeeting::~YangServerMeeting() {
	for(int i=0;i<m_users.size();i++){
		delete m_users.at(i);
		m_users.at(i)=NULL;
	}
	m_users.clear();
}
void YangServerMeeting::split( string s,vector<string>* sv,const char flag ) {
    sv->clear();
    istringstream iss(s);
    string temp;

    while (getline(iss, temp, flag)) {
        sv->push_back(temp);
    }
    return;
}
int YangServerMeeting::getUserCount(){
	return m_users.size();
}
string YangServerMeeting::getUsersStr(){
	string s;
	for (size_t j = 0; j < m_users.size(); j++) {
			char ss[256] = { 0 };
			YangUser *u = &m_users.at(j)->user;
			//printf("\n%d....user==============%d:%s:%d:%d:%d\n",(int)j,u->userId,u->userName,u->roomAdmin,u->micFlag,u->camFlag);
			if (j > 0)
				sprintf(ss, "|%d,%s,%d,%d,%d,%d", u->userId, u->userName,u->myId, u->roomAdmin,
						u->micFlag, u->camFlag);
			else
				sprintf(ss, "%d,%s,%d,%d,%d,%d", u->userId, u->userName, u->myId,u->roomAdmin,
						u->micFlag, u->camFlag);
			s.append(ss);
			u=NULL;
		}
	return s;
}
YangUserContext* YangServerMeeting::getUser(int puserid){
	for(int i=0;i<m_users.size();i++){
	if(m_users.at(i)->userId==puserid) return m_users.at(i);
	}
	return NULL;
}
int YangServerMeeting::removeUser(int puserid){
	for(int i=0;i<m_users.size();i++){
		if(m_users.at(i)->userId==puserid) {
			delete m_users.at(i);
			m_users.at(i)=NULL;
			m_users.erase(m_users.begin()+i);
			return 0;
		}
	}
	return 1;
}
int YangServerMeeting::putUser(int puserid,char *para,st_netfd_t pfd){
	if(getUser(puserid)) return 0;
	string str=para;
	vector<string> sv;
	split(str,&sv,',');
	if(sv.size()<5) return 1;
	m_users.push_back(new YangUserContext());
	m_users.back()->userId=puserid;
	m_users.back()->fd=pfd;
	YangUser *u=&m_users.back()->user;

	u->userId=atoi(sv.at(0).c_str());
	strcpy(u->userName,sv.at(1).c_str());
	u->myId=atoi(sv.at(2).c_str());
	u->roomAdmin=atoi(sv.at(3).c_str());
	u->micFlag=atoi(sv.at(4).c_str());
	u->camFlag=atoi(sv.at(5).c_str());
	printf("\nAdd user userId=%d,username=%s,roomadmin=%d,micflag=%d,camflag=%d",u->userId,u->userName,u->roomAdmin,u->micFlag,u->camFlag);
	u=NULL;
	return 0;
}
