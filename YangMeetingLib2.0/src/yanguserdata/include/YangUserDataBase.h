/*
 * YangDataChannel.h
 *
 *  Created on: 2019年3月31日
 *      Author: yang
 */

#ifndef YANGDATACHANNEL_INCLUDE_YANGDATACHANNEL_H_
#define YANGDATACHANNEL_INCLUDE_YANGDATACHANNEL_H_
//#include <yangutil/sys/YangSysMessageI.h>
#include <yangutil/yangavinfotype.h>
#include <string>
#include <vector>
#include "yangmeeting/yangmeetingtype.h"
//#include "YangUserDataI.h"
using namespace std;

class YangUserDataBase{
public:
	YangUserDataBase();
	virtual ~YangUserDataBase();
	vector<YangUser*> *m_userList;
	YangUser *m_user;
	YangMeeting *m_meeting;
	int32_t addUser(uint8_t *puserstr);
	string getLoginstring();

	int32_t logout(int32_t puid);
	int32_t playVideo(int32_t puid);
	int32_t unPlayVideo(int32_t puid);
	int32_t playAudio(int32_t puid);
	int32_t unPlayAudio(int32_t puid);

	void leaveRoom();
	void clearData();
	void handleVideoStream(int32_t puserId,int32_t isAdd);
	void handleAudioStream(int32_t puserId,int32_t isAdd);
	YangMeetingContext *m_ini;

protected:
	int32_t getUserObjIndex(int32_t puserId);
	int32_t getIndex(int32_t puid,vector<int> *pvs);
	void delUser(int32_t puserId);
	void SplitString(string s, string c);

private:
	void loginHandle(int32_t pisLogin,int32_t pisAdd);
	void removeVectorUid(vector<int>*vecs,int32_t pind);
};

#endif /* YANGDATACHANNEL_INCLUDE_YANGDATACHANNEL_H_ */
