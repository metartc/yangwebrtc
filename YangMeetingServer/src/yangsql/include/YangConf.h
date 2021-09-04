/*
 * YangConf.h
 *
 *  Created on: 2019年3月10日
 *      Author: yang
 */

#ifndef YANGSQL_INCLUDE_YANGCONF_H_
#define YANGSQL_INCLUDE_YANGCONF_H_
#include <stdio.h>

//#include <string>
//using namespace std;
class YangConf {
public:
	YangConf();
	virtual ~YangConf();
	char m_databaseHost[30];
	char m_databaseName[30];
	char m_userName[30];
	char m_password[30];
	int m_databasePort;
	int m_httpPort;
	int m_initConnectionCount;
	int m_maxConnectionCount;

	int m_dataChannelPort;
	int m_usingSrtDataChannel;
	void init(const char *p_filename);
	void initValue();

	char *file;


    int IniReadValue(const char* section,const char* key, char* val);
    int readStringValue(const char* section,const char* key, char* val,const char *p_defaultStr);
    int readStringValue1(const char *section, const char *key, char *val);
    int readIntValue(const char* section,const char* key,int p_defaultInt);

};

#endif /* YANGSQL_INCLUDE_YANGCONF_H_ */
