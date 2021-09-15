/*
 * YangUserManager.h
 *
 *  Created on: 2021年3月6日
 *      Author: yang
 */

#ifndef YANGSQL_INCLUDE_YANGUSERMANAGER_H_
#define YANGSQL_INCLUDE_YANGUSERMANAGER_H_

#include <vector>
#include <string>

#include "YangMysqlBase.h"
using namespace std;

class YangUserManager: public YangMysqlBase {
public:
	YangUserManager();
	virtual ~YangUserManager();
	void init();

	string getUserXml(char* userId,char* passwd);
	string getUserMeetingXml(char* userId);
	string getMeetingXml(string meetingId);



public:
//	string getXml(vector<vector<string>> *data,string* str,int plen);
	string getXml(vector<vector<string>> *data,string tag,string* str,int plen);
	string getMeetingUserCommonXml(char* meetingId,string tag);
	string getMeetingUserCommonXml1(string meetingId);
};

#endif /* YANGSQL_INCLUDE_YANGUSERMANAGER_H_ */
