/*
 * YangServerletManager.cpp
 *
 *  Created on: 2020年10月10日
 *      Author: yang
 */
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "YangServerletManager.h"
using namespace std;
void YangServerletManager::split(const string s,vector<string>* sv,const char flag ) {
    sv->clear();
    istringstream iss(s);
    string temp;

    while (getline(iss, temp, flag)) {
        sv->push_back(temp);
    }
    return;
}
void YangServerletManager::split1(const string s,map<string,string>* sv,const char flag ) {
    //sv->clear();
    istringstream iss(s);
    string key;
    string value;
    getline(iss, key, flag);
    getline(iss, value, flag);
   // printf("\n%s===%s\n",key.c_str(),value.c_str());
    (*sv)[key]=value;
    return;
}
/**
void split2(const string s,map<string,string> sv,const char flag ) {
    //sv->clear();
    istringstream iss(s);
    string key;
    string value;
    getline(iss, key, flag);
    getline(iss, value, flag);
   // printf("\n%s===%s\n",key.c_str(),value.c_str());
    sv[key]=value;
    return;
}**/
YangServerletManager::YangServerletManager() {
	//ym.init();

}

YangServerletManager::~YangServerletManager() {

}
string YangServerletManager::handle(char* p){
	string s("");
	vector<string> sb;
	map<string,string> requestMap;
	string str(p);
	split(str,&sb,'&');
	for(size_t i=0;i<sb.size();i++){
		split1(sb.at(i),&requestMap,'=');
		//split2(sb.at(i),requestMap,'=');
	}
	//printf("\nvector size==%d,map size===%d\n",sb.size(),requestMap.size());
	if(requestMap["opt"]=="loginverify"&&requestMap["moduleId"]=="user"){
		s.append(ym.getUserXml((char*)requestMap[string("loginName")].c_str(),""));
	}else if(requestMap["opt"]=="selectsdoor"&&requestMap["moduleId"]=="meeting"){
		s.append(ym.getUserMeetingXml((char*)requestMap[string("userId")].c_str()));
	}else if(requestMap["opt"]=="select"&&requestMap["moduleId"]=="meeting"){
		s.append(ym.getMeetingXml(requestMap[string("meetingId")]));
	}
	//userId=1001&opt=selectsdoor&moduleId=meeting&isGroup=N&deleteFlag=N

	//ym.getUserXml()
	return s;
}
