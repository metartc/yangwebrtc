
#include "../include/YangUserManager.h"
#include <string.h>
#include <string>
#include <vector>
using namespace std;
YangUserManager::YangUserManager() {

}

YangUserManager::~YangUserManager() {

}

void YangUserManager::init() {
	//initMysql("127.0.0.1", "root", "rf123456", "meeting", 3306);

}

/**
 string YangUserManager::getXml(vector<vector<string>> *data,string* str,int plen){
 string sb;
 int i=0;
 char s[128];
 for (vector<vector<string>>::iterator it = data->begin(); it != data->end(); it++)
 {
 vector<string> tmp=*it;
 i=0;
 for(int j=0;j<plen;j++){
 //memset(s,0,128);
 sprintf(s,"<%s>%s</%s>",str[j].c_str(),tmp.at(i++).c_str(),str[j].c_str());
 sb.append(s);
 }
 }
 return sb;
 }
 **/
string YangUserManager::getXml(vector<vector<string>> *data, string tag,
		string *str, int plen) {
	string sb;
	int i = 0;
	char s[128];
	for (vector<vector<string>>::iterator it = data->begin(); it != data->end();
			it++) {
		vector<string> tmp = *it;
		i = 0;
		sb.append("<" + tag + ">");
		for (int j = 0; j < plen; j++) {
			//memset(s,0,128);
			sprintf(s, "<%s>%s</%s>", str[j].c_str(), tmp.at(i++).c_str(),
					str[j].c_str());
			sb.append(s);
		}
		sb.append("</" + tag + ">");
	}
	return sb;
}

string YangUserManager::getUserXml(char *userId, char *passwd) {
	if (strchr(userId, '\'') || strchr(passwd, '\''))
		return string(
				"<?xml version=\"1.0\" encoding=\"utf-8\"?><user loginState=\"fail\"><tmMemberId>000</tmMemberId></user>");
	char sql[512] = { 0 };
	//sprintf("select user_id, login_name,userStatus,user_name,mobile,email, depart_id,tel,sip_flag,remark from mt_code_user login where login_name = '%s' and user_password='%s'",userId,passwd);

	vector<vector<string>> data;
	sprintf(sql,
			"select user_id, login_name,user_status,user_name from yang_user login where login_name = '%s'",
			userId);
	data = querySql(sql);
	//printf("\nsize====%d\n",data.size());
	if (data.size() != 1)
		return string(
				"<?xml version=\"1.0\" encoding=\"utf-8\"?><user loginState=\"fail\"><tmMemberId>000</tmMemberId></user>");
	string ret("");
	for (vector<vector<string>>::iterator it = data.begin(); it != data.end();
			it++) {
		vector<string> tmp = *it;
		ret =
				"<?xml version=\"1.0\" encoding=\"utf-8\"?><user loginState=\"success\"><userId>"
						+ tmp.at(0) + "</userId><loginName>" + tmp.at(1)
						+ "</loginName><adminId>" + tmp.at(2)
						+ "</adminId><userName>"
						+ tmp.at(3)
						+ "</userName></user>";
	}
	return ret;

}

//select a.user_id, a.user_name, '' user_password, a.email, a.mobile, b.purview depart_id,b.purview userStatus,a.login_name,a.sip_flag,a.tel,a.remark
  // from mt_code_user a ,mt_join_meeting_people b where a.user_id=b.user_id
string YangUserManager::getMeetingUserCommonXml1(string meetingId) {
	char sql[1024] = { 0 };
	sprintf(sql,"select a.user_id, a.user_name, a.login_name from yang_user a,  mt_join_meeting_user b \
				where	 a.user_id=b.user_id and b.mt_id='%s' and b.mt_role='3'", meetingId.c_str());
	vector<vector<string>> data;
	data = querySql(sql);
	string sb;
	int i = 0;
	//char s1[512] = { 0 };
	for (vector<vector<string>>::iterator it = data.begin(); it != data.end();
			it++) {
		vector<string> tmp = *it;
		//i = 0;
		if(i==0)
			sb.append(tmp.at(1));
		else
			sb.append(","+tmp.at(1));
		i++;
		}
	return sb;
}

string YangUserManager::getUserMeetingXml(char *userId) {
	char sql[1024] = { 0 };
	sprintf(sql,
			"SELECT a.mt_id,a.mt_name,a.mt_type,a.mt_STATUS status,(SELECT user_name FROM yang_user WHERE a.mt_director=user_id) AS director ,\
	 a.mt_desc  FROM yang_meeting a,(SELECT DISTINCT mt_id FROM mt_join_meeting_user WHERE user_id ='%s') b  WHERE a.mt_id=b.mt_id",
			userId);
	//8
	vector<vector<string>> data;
	data = querySql(sql);
	string sb;
	sb.append("<?xml version=\"1.0\" encoding=\"utf-8\"?><result>");
	//int i=0;
	string s[] = { "meetingId", "meetingName", "meetingType",  "meetingDesc" };
	//;
	int i = 0;
	char s1[2048] = { 0 };
	for (vector<vector<string>>::iterator it = data.begin(); it != data.end();
			it++) {
		vector<string> tmp = *it;
		i = 0;
		sb.append("<meeting>");
		for (int j = 0; j < 4; j++) {
			//memset(s,0,128);
			sprintf(s1, "<%s>%s</%s>", s[j].c_str(), tmp.at(i++).c_str(),
					s[j].c_str());
			sb.append(s1);
		}
		sb.append("<adminName>"+getMeetingUserCommonXml1(tmp.at(0))+"</adminName>");
		sb.append("</meeting>");
	}

	//sb.append(getXml(&data,s,23));

	sb.append("</result>");
	return sb;
}


string YangUserManager::getMeetingXml(string meetingId) {
	char sql[200] = { 0 };
	string sb("");
	sprintf(sql,
			"SELECT mt_id,mt_name,mt_type,mt_desc  from yang_meeting WHERE mt_id='%s'",
			meetingId.c_str());
	//8
	vector<vector<string>> data;
	data = querySql(sql);
	if (data.size() == 0)
		return sb;
	sb.append("<?xml version=\"1.0\" encoding=\"utf-8\"?><result>");
	//int i=0;
	string s[] = { "meetingId", "meetingName", "meetingType", "meetingDesc" };
	vector<string> *tmp = &data.at(0);
	int i = 0;
	char s1[1024] = { 0 };
	sb.append("<meeting>");
	for (int j = 0; j < 4; j++) {
		//memset(s,0,128);
		sprintf(s1, "<%s>%s</%s>", s[j].c_str(), tmp->at(i++).c_str(),
				s[j].c_str());
		sb.append(s1);
	}
	sb.append("<members>");
	sb.append(getMeetingUserCommonXml((char*) meetingId.c_str(), "member"));
	sb.append("</members>");
	sb.append("</meeting>");
	tmp = NULL;
	return sb;
}
/**
 string  YangUserManager::getMeetingUser(char* meetingId){
 char sql[1024]={0};
 sprintf(sql," select * from yang_meeting   where mt_id='%s'",meetingId);
 //8
 vector<vector<string>> data;
 data =querySql(sql);
 string sb;
 sb.append("<?xml version=\"1.0\" encoding=\"utf-8\"?><result>");
 //int i=0;
 string s[]={"memberId","memberName","purview","mobile","email","tel"};
 //;
 sb.append("<meeting>");
 sb.append(getXml(&data,s,6));
 string tag("members");
 getMeetingUserCommonXml(meetingId,tag);
 sb.append("</meeting>");

 sb.append("</result>");
 return sb;
 }**/
//}
string YangUserManager::getMeetingUserCommonXml(char *meetingId, string tag) {
	char sql[1024] = { 0 };
	sprintf(sql,
			"select a.user_id, a.user_name, a.login_name, ifnull(b.mt_role,0) mtRole\			
			from yang_user a left outer join (SELECT * FROM mt_join_meeting_user WHERE mt_id='%s') b \
					on a.user_id=b.user_id", meetingId);
	//8
	vector<vector<string>> data;
	data = querySql(sql);
	string sb;

	string s[] = { "memberId", "memberName", "loginName", "mtRole" };
	sb.append(getXml(&data, tag, s, 4));

	return sb;
}

