/*
 * YangDataChannel.cpp
 *
 *  Created on: 2019年3月31日
 *      Author: yang
 */

#include <yanguserdata/YangCommandId.h>
#include <yanguserdata/YangUserDataBase.h>
YangUserDataBase::YangUserDataBase() {
	m_user = NULL;
	m_meeting = NULL;
	m_userList = NULL;
	m_context = NULL;
}

YangUserDataBase::~YangUserDataBase() {
	clearData();
	m_user = NULL;
	m_meeting = NULL;
	m_userList = NULL;
	m_context = NULL;
}

void YangUserDataBase::clearData() {
	if (m_userList) {
		for (int32_t i = 0; i < (int) m_userList->size(); i++) {
			delete m_userList->at(i);
		}
		m_userList->clear();
	}
	if (m_context) {
		m_context->audiouids.clear();
		m_context->videouids.clear();
		//m_context->playvideouids.clear();
		//m_context->playaudiouids.clear();
	}
}
void YangUserDataBase::loginHandle(int32_t pind, int32_t pisAdd) {
	if (pind < 0)
		return;

	YangUser *yu = m_userList->at(pind);	//->userId == puserId)
	if (!pisAdd) {
		removeVectorUid(&m_context->audiouids, yu->userId);
		removeVectorUid(&m_context->videouids, yu->userId);
		//removeVectorUid(&m_context->playaudiouids, yu->userId);
		//removeVectorUid(&m_context->playvideouids, yu->userId);
	}

	char strs[120];
	memset(strs, 0, 120);
	sprintf(strs, "用户%s已%s教室%s", yu->userName, pisAdd == 1 ? "进入" : "离开",
			m_meeting->meetingName);

	if (m_context->receive)
		m_context->receive->receiveMessage(strs);				//(perrCode);
	if (!pisAdd) {
		delete m_userList->at(pind);
		m_userList->erase(m_userList->begin() + pind);
	}
	yu = NULL;
}

void YangUserDataBase::removeVectorUid(vector<int> *vecs, int32_t puid) {
	for (size_t i = 0; i < vecs->size(); i++) {
		if (vecs->at(i) == puid)
			vecs->erase(vecs->begin() + i);
	}
}
int32_t YangUserDataBase::addUser(uint8_t *puserstr) {
	int32_t ret = -1;
	int32_t tcou = 0;
	int32_t strsize = strlen((char*) puserstr) + 1;
    char* tuserstr=new char[strsize];
    YangAutoFreeA(char,tuserstr);
	memset(tuserstr, 0, strsize);
	memcpy(tuserstr, puserstr, strsize - 1);
	YangUser *yu = new YangUser();
	char *p = strtok(tuserstr, ",");
	while (p) {
		if (tcou == 0)
			yu->userId = atoi(p);//atoi(p);//memcpy(yu->userId, p, strlen(p));
		if (tcou == 1) {
			memcpy(yu->userName, p, strlen(p));
		}

		if (tcou == 2)	yu->myId = atoi(p);
		if (tcou == 3)
			yu->roomAdmin = atoi(p);
		if (tcou == 4)
			yu->micFlag = atoi(p);
		if (tcou == 5)
			yu->camFlag = atoi(p);

		tcou++;
		p = strtok(NULL, ",");
	}
	//printf("\nuserId=%d,micFlag==%d",yu->userId,yu->micFlag);
	if (getUserObjIndex(yu->userId) == -1) {
		m_userList->push_back(yu);
		ret = m_userList->size() - 1;
	}
	if (yu->micFlag == 2)
		handleAudioStream(yu->userId, 1);
	if (yu->camFlag == 2)
		handleVideoStream(yu->userId, 1);
	//yu = NULL;
	p = NULL;
	return ret;
}

int32_t YangUserDataBase::getUserObjIndex(int32_t puserId) {
	for (size_t i = 0; i < m_userList->size(); i++) {
		if (m_userList->at(i)->userId == puserId)
			return i;
	}
	return -1;
}

void YangUserDataBase::delUser(int32_t puserId) {
	for (size_t i = 0; i < m_userList->size(); i++) {
		if (m_userList->at(i)->userId == puserId) {
			handleAudioStream(puserId, 0);
			handleVideoStream(puserId, 0);
			delete m_userList->at(i);
			m_userList->erase(m_userList->begin() + i);
		}
	}
}

void YangUserDataBase::handleVideoStream(int32_t puserId, int32_t isAdd) {
	if (puserId == m_user->userId)
		return;
	int32_t index = getUserObjIndex(puserId);
	if (index < 0)
		return;
	YangUser *yu = m_userList->at(index);
	int32_t ind = getIndex(yu->userId, &m_context->videouids);
	if (isAdd) {
		if (ind == -1)
			m_context->videouids.push_back(yu->userId);
		yu->camFlag = 2;
		//yang_post_message(YangM_Sys_Playvideo,yu->userId,NULL);
	} else {
		yu->camFlag = 1;
		if (ind > -1)
			m_context->videouids.erase(m_context->videouids.begin() + ind);
		//yang_post_message(YangM_Sys_UnPlayvideo,yu->userId,NULL);
	}
	yu = NULL;
}
void YangUserDataBase::handleAudioStream(int32_t puserId, int32_t isAdd) {
	if (puserId == m_user->userId)
		return;
	int32_t index = getUserObjIndex(puserId);
	if (index < 0)
		return;
	YangUser *yu = m_userList->at(index);
	int32_t ind = getIndex(yu->userId, &m_context->audiouids);
	if (isAdd) {
		yu->micFlag = 2;
		if (ind == -1) {
			m_context->audiouids.push_back(yu->userId);
			//yang_post_message(YangM_Sys_Playaudio, yu->userId, NULL);
		}
	} else {
		if (ind > -1) {
			m_context->audiouids.erase(m_context->audiouids.begin() + ind);
			//yang_post_message(YangM_Sys_UnPlayaudio, yu->userId, NULL);
		}
		yu->micFlag = 1;

	}
	yu = NULL;
}

void YangUserDataBase::SplitString(string s, string c) {
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2) {
		//v.push_back(s.substr(pos1, pos2-pos1));
		addUser((uint8_t*) s.substr(pos1, pos2 - pos1).data());
		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		addUser((uint8_t*) s.substr(pos1).data());
}



int32_t YangUserDataBase::getIndex(int32_t puid, vector<int> *pvs) {
	for (int32_t i = 0; i < (int) pvs->size(); i++) {
		if (pvs->at(i) == puid)
			return i;
	}
	return -1;
}

void YangUserDataBase::leaveRoom() {
	for (int32_t i = 0; i < (int) m_userList->size(); i++) {
		delete m_userList->at(i);
	}
	m_userList->clear();
}
int32_t YangUserDataBase::logout(int32_t puid) {
	int32_t ind=getUserObjIndex(puid);
	if(puid>-1){
		delete m_userList->at(ind);
		m_userList->erase(m_userList->begin() + ind);
	}
	removeVectorUid(&m_context->audiouids, puid);
	removeVectorUid(&m_context->videouids, puid);
	return Yang_Ok;
}
string YangUserDataBase::getLoginstring() {
	char ss[128] = { 0 };

	sprintf(ss, "%d,%s,%d,%d,%d,%d", m_user->userId, m_user->userName,
			m_user->myId, m_user->roomAdmin, m_user->micFlag, m_user->camFlag);
	string s(ss);

	return s;
}


int32_t YangUserDataBase::playVideo(int32_t puid){
	handleVideoStream(puid,1);
	return Yang_Ok;
}
int32_t YangUserDataBase::unPlayVideo(int32_t puid){
	handleVideoStream(puid,0);
	return Yang_Ok;
}
int32_t YangUserDataBase::playAudio(int32_t puid){
	handleAudioStream(puid,1);
	return Yang_Ok;
}
int32_t YangUserDataBase::unPlayAudio(int32_t puid){
	handleAudioStream(puid,0);
	return Yang_Ok;
}
