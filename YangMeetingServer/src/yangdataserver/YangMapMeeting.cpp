/*
 * YangMeetingMap.cpp
 *
 *  Created on: 2020年8月24日
 *      Author: yang
 */

#include <string.h>
#include <yangdataserver/YangCommandId.h>
#include <yangdataserver/YangMapMeeting.h>
#include <sstream>
#include "yangsrt/include/YangTsMuxer.h"
//#include "yangsrt/include/YangMessageData.h"

void YangMapMeeting::split( string s,vector<string>* sv,const char flag ) {
    sv->clear();
    istringstream iss(s);
    string temp;

    while (getline(iss, temp, flag)) {
        sv->push_back(temp);
    }
    return;
}



YangMapMeeting::YangMapMeeting() {
	//if(m_meetingMap==NULL) m_meetingMap=new map<int,vector<YangUserContext*>*>();
	m_usingSrtDataChannel=0;

}

YangMapMeeting::~YangMapMeeting() {

}
//vector<YangUserContext*> *YangMapMeeting::m_meetingMap=NULL;

void YangMapMeeting::clearMap(){

	for(int i=0;i<(int)m_meetingList.size();i++){
		delete m_meetingList.at(i);
		m_meetingList.at(i)=NULL;
	}
	m_meetingList.clear();

}
YangServerMeeting *YangMapMeeting::getMeeting(int pmeetingId){
	for(int i=0;i<(int)m_meetingList.size();i++){
		if(m_meetingList.at(i)->m_meetingId==pmeetingId) return m_meetingList.at(i);
	}
	return NULL;
}
vector<YangServerMeeting*> YangMapMeeting::m_meetingList;
YangUser* YangMapMeeting::getUser(int pmeetingId,int puserid){
	YangServerMeeting *meeting=getMeeting(pmeetingId);
	if(!meeting){
		meeting=NULL;
		return NULL;
	}
	return &meeting->getUser(puserid)->user;

}

int YangMapMeeting::removeUser(int pmeetingId,int puserid){
	YangServerMeeting *meeting=getMeeting(pmeetingId);
	if(meeting) {
		meeting->removeUser(puserid);
		if(meeting->getUserCount()==0){
			for(int i=0;i<(int)m_meetingList.size();i++){
				if(m_meetingList.at(i)->m_meetingId==pmeetingId){
					delete m_meetingList.at(i);
					m_meetingList.at(i)=NULL;
					m_meetingList.erase(m_meetingList.begin()+i);
					return 0;
				}
			}
		}
		return 0;
	}
	return 1;
}

int YangMapMeeting::putUser(int pmeetingId,int puserid,char *para){
	if(!getMeeting(pmeetingId)){
		m_meetingList.push_back(new YangServerMeeting());
		m_meetingList.back()->m_meetingId=pmeetingId;
		m_meetingList.back()->m_meeting.meetingId=pmeetingId;
	}

	return getMeeting(pmeetingId)->putUser(puserid,para,m_fd);
}


/**void YangMapMeeting::commandHandle(int pmeetingId,unsigned char* p,int plen){
	YangMessage mess;
	memset(&mess,0,sizeof(mess));
	m_message.getMessage(p,plen,&mess);
	if(mess.messageType==Yang_Message_Command){
		string s((char*)mess.data,mess.len);
		vector<string> sb;
		split(s, &sb, ':');
		if (sb.size() < 4)		return;
		int t_userId = atoi(sb.at(1).c_str());
		//int t_AimUserId = atoi(sb.at(2).c_str());
		int commandIdint = atoi(sb.at(3).c_str());
		YangUser* u=NULL;
		if(commandIdint==Yang_Command_PubCam||commandIdint==Yang_Command_PubCamClose){
			u=getUser(pmeetingId,t_userId);
			if(u) u->camFlag=commandIdint==Yang_Command_PubCam?2:1;
		}
		if(commandIdint==Yang_Command_PubMic||commandIdint==Yang_Command_PubMicClose){
				u=getUser(pmeetingId,t_userId);
				if(u) u->micFlag=commandIdint==Yang_Command_PubMic?2:1;
			}
		u=NULL;
	}
}**/

