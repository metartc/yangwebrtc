/*
 * YangCommandHandle.cpp
 *
 *  Created on: 2021年5月3日
 *      Author: yang
 */

#include <yanguserdata/YangCommandId.h>
#include <yanguserdata/YangUserData.h>
#include <string.h>
#include <memory.h>
#include <yangutil/sys/YangSysMessageI.h>
#include <yanguserdata/YangCommandHandle.h>
#include <yangutil/sys/YangLog.h>
#include <sstream>

YangCommandHandle::YangCommandHandle() {
	m_context = NULL;
	m_userData = NULL;
}

YangCommandHandle::~YangCommandHandle() {
	m_context = NULL;
	m_userData = NULL;
}
void YangCommandHandle::split(string s, vector<string> *sv, const char flag) {
	sv->clear();
	istringstream iss(s);
	string temp;

	while (getline(iss, temp, flag)) {
		sv->push_back(temp);
	}
	return;
}
void YangCommandHandle::decodeMessage(uint8_t *p, int32_t plen) {
	auto input_ptr = std::make_shared<SRT_DATA_MSG>((uint8_t*) p, plen);
	demux.decode(input_ptr, this);
}
void YangCommandHandle::on_data_callback(SRT_DATA_MSG_PTR data_ptr,
		uint32_t  media_type, uint64_t dts, uint64_t pts) {
	if (media_type == Yang_PRIVATE_PID) {
		handleMessage(data_ptr->get_data(), data_ptr->data_len());
	}
}
void YangCommandHandle::parseCommand(uint8_t *p, int32_t plen) {
	string s((char*) p, plen);
	vector<string> sb;
	split(s, &sb, ':');
	if (sb.size() < 4)
		return;

	int32_t t_userId = atoi(sb.at(1).c_str());
	int32_t t_AimUserId = atoi(sb.at(2).c_str());
	int32_t commandIdint = atoi(sb.at(3).c_str());
	if (t_userId == m_context->user.userId)
		return;
	handleCommand(t_userId, t_AimUserId, commandIdint,
			(uint8_t*) sb.at(4).c_str());
}
void YangCommandHandle::handleMessage(uint8_t *p, int32_t plen) {
	YangUserData mes;
	YangUserDataType mdata;
	memset(&mdata, 0, sizeof(mdata));
	mes.getMessage(p, plen, &mdata);
	string s((char*) mdata.data, mdata.len);

	if (mdata.messageType == Yang_Message_RoomInfoUsers) {
		getConfUserInfosResult(mdata.data, mdata.len);
	} else if (mdata.messageType == Yang_Message_Command) {
		parseCommand(mdata.data, mdata.len);
	}
}

int32_t YangCommandHandle::sendCommand(int32_t pcommandId) {
	return sendCommand(pcommandId, (uint8_t*) " ");
}
int32_t YangCommandHandle::sendCommand(int32_t pcommandId, uint8_t *p) {

	char ss[128] = { 0 };
	sprintf(ss, "%d:%d:%d:%d:%s", m_context->meeting.meetingId, m_context->user.userId,
			m_context->user.userId, pcommandId, p);

	vector<YangTsBuffer> sb;			// = new vector<YangTsBuffer>();
	YangUserData message;
	message.putMessage(Yang_Message_Command, (uint8_t*) ss, strlen(ss),
			&sb);

	int32_t ret = Yang_Ok;
	for (size_t i = 0; i < sb.size(); i++) {
		ret = publish(sb.at(i).data(), 188);
		if (ret) {
			yang_error("publish command error..%d", i);
			return ret;
		}
	}
	return ret;
}

int32_t YangCommandHandle::sendRoomInfoUser() {
	return sendCommand(Yang_Command_RoomInfoUsers);
}

int32_t YangCommandHandle::login(string loginStr) {
	return sendCommand(Yang_Command_Login, (uint8_t*) loginStr.c_str());
}

int32_t YangCommandHandle::logout() {
	return sendCommand(Yang_Command_LogOut);
}
int32_t YangCommandHandle::pubAudio() {
	return sendCommand(Yang_Command_PubMic);
}
int32_t YangCommandHandle::unPubAudio() {
	return sendCommand(Yang_Command_PubMicClose);
}

int32_t YangCommandHandle::pubVideo() {
	return sendCommand(Yang_Command_PubCam);
}
int32_t YangCommandHandle::unPubVideo() {
	return sendCommand(Yang_Command_PubCamClose);
}

int32_t YangCommandHandle::getConfUserInfosResult(uint8_t *p, int32_t plen) {

	//printf("\nstr============%s",p);
	string result((char*) p, plen);
	vector<string> sb1;
	split(result, &sb1, '|');
	for (size_t i = 0; i < sb1.size(); i++) {
		if (m_userData)
			m_userData->addUser((uint8_t*) sb1.at(i).c_str());
	}
	yang_post_state_message((int)YangM_Sys_GetMeetinginfo, 0, 1, NULL);

	return Yang_Ok;
}

void YangCommandHandle::handleCommand(int32_t puserid, int32_t paimuserid,
		int32_t pcommandid, uint8_t *content) {
	if (puserid == m_context->user.userId)
		return;
	switch (pcommandid) {
	case Yang_Command_Login:
		if (m_userData)
			m_userData->addUser(content);
		yang_post_message(YangM_Sys_Login, puserid, NULL);
		break;
	case Yang_Command_Login_Success:
		yang_post_state_message((int)YangM_Sys_Login, puserid, 0, NULL);
		break;
	case Yang_Command_Login_Failure:
		yang_post_state_message((int)YangM_Sys_Login, puserid, 1, NULL);
		break;
	case Yang_Command_LogOut:
		yang_post_message(YangM_Sys_Logout, puserid, NULL);
		break;
	case Yang_Command_AllowSpeak:
		if (paimuserid == m_context->user.userId)
			yang_post_message(YangM_Sys_Pubaudio, m_context->user.userId, NULL);
		break;
	case Yang_Command_DisableSpeak:
		if (paimuserid == m_context->user.userId)
			yang_post_message(YangM_Sys_UnPubaudio, m_context->user.userId, NULL);
		break;
	case Yang_Command_PubCam:
		if (m_userData)
			m_userData->handleVideoStream(puserid, 1);
		//yang_post_message(YangM_Sys_Pubvideo,puserid,NULL);
		break;
	case Yang_Command_PubCamClose:
		if (m_userData)
			m_userData->handleVideoStream(puserid, 0);
		printf("\n********************************************\n");
		printf("Yang_Command_PubCamClose uid===%d",puserid);
		printf("\n********************************************\n");
		yang_post_message(YangM_Sys_UnPlayvideo, puserid, NULL);
		break;
	case Yang_Command_PubMic:
		if (m_userData)
			m_userData->handleAudioStream(puserid, 1);
		yang_post_message(YangM_Sys_Playaudio, puserid, NULL);
		break;
	case Yang_Command_PubMicClose:
		if (m_userData)
			m_userData->handleAudioStream(puserid, 0);
		yang_post_message(YangM_Sys_UnPlayaudio, puserid, NULL);
		break;
	case Yang_Command_KickOff:

		break;
	default:
		break;
	}
}

