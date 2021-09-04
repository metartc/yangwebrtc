/*
 * YangUserDataHandle.cpp
 *
 *  Created on: 2020年10月1日
 *      Author: yang
 */

#include "YangUserDataHandle.h"
#include "YangCommandId.h"
#include <string.h>
//#include "common.hpp"
int64_t sls_gettime(void)//rturn micro-second
{
#if HAVE_GETTIMEOFDAY
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
#elif HAVE_GETSYSTEMTIMEASFILETIME
    FILETIME ft;
    int64_t t;
    GetSystemTimeAsFileTime(&ft);
    t = (int64_t)ft.dwHighDateTime << 32 | ft.dwLowDateTime;
    return t / 10 - 11644473600000000; /* Jan 1, 1601 */
#else
    return -1;
#endif
}

YangUserDataHandle::YangUserDataHandle(st_netfd_t pfd) {
	m_userId = 0;
	m_meetingId = 0;
	m_fd = pfd;
	m_loop=0;
	notLogin=1;
	hasLoginClient=0;
}

YangUserDataHandle::~YangUserDataHandle() {

}

void YangUserDataHandle::on_data_callback(SRT_DATA_MSG_PTR data_ptr,
		unsigned int media_type, uint64_t dts, uint64_t pts) {
	if (media_type == Yang_PRIVATE_PID) {
		userCommandHandle(data_ptr->get_data(), data_ptr->data_len());
	}
}
void YangUserDataHandle::sendUsersStr(int pmeetingId) {
	YangServerMeeting *meeting=getMeeting(pmeetingId);
	if(!meeting) return;
	vector<YangTsBuffer> sb;

	string s=meeting->getUsersStr();
	m_message.putMessageWithPmt(Yang_Message_RoomInfoUsers, (unsigned char*) s.c_str(),
			s.length(), &sb);
	for (int i = 0; i < (int) sb.size(); i++) {
		st_write(m_fd, sb.at(i).data(), 188, ST_UTIME_NO_TIMEOUT);
	}
	sb.clear();
}
void YangUserDataHandle::sendCommand(int pmeetingId,vector<YangTsBuffer> *sb){
	YangServerMeeting *meeting=getMeeting(pmeetingId);
		if(!meeting) return;
		if(meeting->m_users.size()==0) return;
		for (size_t j = 0; j < meeting->m_users.size(); j++) {
			if(meeting->m_users.at(j)->fd==m_fd) continue;
			for (int i = 0; i < (int) sb->size(); i++) {
					st_write(meeting->m_users.at(j)->fd, sb->at(i).data(), 188, ST_UTIME_NO_TIMEOUT);
				}
		}
}

void YangUserDataHandle::userCommandHandle(unsigned char *p, int plen) {
	YangMessage mess;
	memset(&mess, 0, sizeof(mess));
	m_message.getMessage(p, plen, &mess);

	if (mess.messageType == Yang_Message_Command) {
		string s((char*)mess.data, mess.len);
		vector<string> sb;
		split(s, &sb, ':');
		if (sb.size() < 4)
			return;

		//int t_AimUserId = atoi(sb.at(2).c_str());
		int commandIdint = atoi(sb.at(3).c_str());

		YangUser *u = NULL;
		if (commandIdint == Yang_Command_Login) {

			int t_meetingId = atoi(sb.at(0).c_str());
			int t_userId = atoi(sb.at(1).c_str());
			if(getUser(t_meetingId,t_userId)){
				hasLoginClient=1;
				sendUserCommond(Yang_Command_HasLogin);
				sendUserCommond(Yang_Command_Login_Failure);
				//u=NULL;
				//return;
			}else{
				m_meetingId = t_meetingId;
				m_userId = t_userId;
				putUser(m_meetingId, m_userId, (char*) sb.at(4).c_str());
				notLogin=0;
				sendUserCommond(Yang_Command_Login_Success);
			}
		} else if (commandIdint == Yang_Command_LogOut) {
			removeUser(m_meetingId, m_userId);
			m_loop=0;
		} else if (commandIdint == Yang_Command_PubCam
				|| commandIdint == Yang_Command_PubCamClose) {
			u = getUser(m_meetingId, m_userId);
			if (u){
				u->camFlag = commandIdint == Yang_Command_PubCam ? 2 : 1;
				printf("\nuserid===%d.....cam pub=%d..",u->userId,u->camFlag);
			}
		} else if (commandIdint == Yang_Command_PubMic
				|| commandIdint == Yang_Command_PubMicClose) {
			u = getUser(m_meetingId, m_userId);
			if (u){
				u->micFlag = commandIdint == Yang_Command_PubMic ? 2 : 1;
				printf("\nuserid===%d.....mic pub=%d..",u->userId,u->micFlag);
			}
		}
		if (commandIdint == Yang_Command_RoomInfoUsers) {
			sendUsersStr(m_meetingId);
		}else{
			YangMessageData md;
			vector<YangTsBuffer> sb;
			md.putMessage(&mess,&sb);
			sendCommand(m_meetingId,&sb);
			sb.clear();
		}

		printf("\n*********************commandid==%d,\n",commandIdint);
		u = NULL;
	}
}

void YangUserDataHandle::sendLogout(int pmeetingId){
	 char ss[128] = { 0 };
	sprintf(ss, "%d:%d:%d:%d: ", pmeetingId, m_userId,
					m_userId, Yang_Command_LogOut);
	vector<YangTsBuffer> sb;
	YangMessageData message;
	message.putMessage(Yang_Message_Command,(unsigned char*)ss,strlen(ss),&sb);
	sendCommand(pmeetingId,&sb);

}
void YangUserDataHandle::sendUserCommond(int commandId){
	 char ss[128] = { 0 };
	sprintf(ss, "%d:%d:%d:%d: ", 0, 0,
					m_userId, commandId);
	vector<YangTsBuffer> sb;
	YangMessageData message;
	message.putMessage(Yang_Message_Command,(unsigned char*)ss,strlen(ss),&sb);
	for (int i = 0; i < (int) sb.size(); i++) {
		st_write(m_fd, sb.at(i).data(), 188, ST_UTIME_NO_TIMEOUT);
	}
}
#define IOBUFSIZE (16*1024)
void YangUserDataHandle::startLoop() {
	struct pollfd pds[1];
	pds[0].fd = st_netfd_fileno(m_fd);
	pds[0].events = POLLIN; //|POLLERR;
	int nr = 0;
	unsigned char buf[IOBUFSIZE];
	YangTsdemux demux;
	printf("\n*******************connect connect............\n");
	m_loop=1;
	int64_t connTimeBase=sls_gettime()/1000;
	notLogin=1;
	st_utime_t tm=2000*1000;
	st_utime_t heartbeatTm=1000*1000*60;
	char beatbuffer[3]={0x00,0x00,0x00};
	while (m_loop) {
		if(notLogin){
			if(hasLoginClient){
				int64_t connTime=sls_gettime()/1000;
				if((connTime-connTimeBase)>1500){
					break;
				}
			}
		}
		pds[0].revents = 0;
		//printf("\nnotLogin====%d",notLogin);
		if (st_poll(pds, 1, notLogin?tm:heartbeatTm) <= 0) {
			if(notLogin)
				printf("not login timeout \n");
			else
				printf("socket connect timeout.........\n");
			break;
		}

		if (pds[0].revents & POLLIN) {

			nr = (int) st_read(m_fd, buf, IOBUFSIZE, ST_UTIME_NO_TIMEOUT);
			if (nr > 0) {
				if(nr==3&&memcmp(buf,beatbuffer,3)==0) {
					//printf("heartbeat read....%d\n",nr);
					continue;
				}
				auto input_ptr = std::make_shared<SRT_DATA_MSG>(buf, nr);
				demux.decode(input_ptr, this);

			} else if (nr == 0)
				break;
			//printf("\n.......%s...",buf);
		}
		//if (pds[0].revents & POLLERR){
		//	printf("\n........error.............");
		//}
	}
	removeUser(m_meetingId,m_userId);
	st_netfd_close(m_fd);
	sendLogout(m_meetingId);
}
