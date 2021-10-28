/*
 * YangConf.cpp
 *
 *  Created on: 2019年3月10日
 *      Author: yang
 */

#include "../include/YangConf.h"
#include <stdio.h>
#include "fcntl.h"
#include "memory.h"
#include "stdlib.h"
#include <unistd.h>
#include <string.h>
#define SECTION_MAX_LEN 256
#define STRVALUE_MAX_LEN 256
#define LINE_CONTENT_MAX_LEN 256
#define FILEPATH_MAX 80
void yangTrim(char *ps, int len) {
	for (int i = 0; i < len; i++)
		if (*(ps + i) == 0x0a || *(ps + i) == 0x0d)
			*(ps + i) = 0x00;
}
YangConf::YangConf() {
	m_databasePort=3316;
	m_httpPort=8080;
	m_initConnectionCount=5;
	m_maxConnectionCount=5;
	m_usingSrtDataChannel=0;
	m_dataChannelPort=9999;
	file=NULL;


		memset(m_databaseName,0,sizeof(m_databaseName));
		memset(m_userName,0,sizeof(m_userName));
		memset(m_password,0,sizeof(m_password));
		memset(m_databaseHost,0,sizeof(m_databaseHost));

}

YangConf::~YangConf() {
	if(file) free(file);
	file=NULL;
}
void YangConf::initValue(){
	m_maxConnectionCount = readIntValue("database", "maxConnectionCount", 5);
	m_initConnectionCount = readIntValue("database", "initConnectionCount", 5);
	m_databasePort = readIntValue("database", "databasePort", 3306);
	m_httpPort = readIntValue("sys", "httpPort", 8080);
	m_dataChannelPort=readIntValue("sys", "dataChannelPort", 9999);
	m_usingSrtDataChannel=readIntValue("sys", "usingSrtDataChannel", 0);
	readStringValue("database", "databaseName", m_databaseName, "meeting");
	readStringValue("database", "userName", m_userName, "root");
	readStringValue("database", "password", m_password, "rf123456");
	readStringValue("database", "databaseHost", m_databaseHost, "127.0.0.1");
//	printf("\nport=%d,httpPort=%d,databaseName=%s,usrName=%s,pass=%s,host=%s\n",m_databasePort,m_httpPort,m_databaseName,m_userName,m_password,m_databaseHost);


}

int YangConf::readIntValue(const char *section, const char *key,
		int p_defaultInt) {
	char strValue[STRVALUE_MAX_LEN];
	memset(strValue, '\0', STRVALUE_MAX_LEN);
	if (readStringValue1(section, key, strValue) != 1) {
		return p_defaultInt;
	}
	return (atoi(strValue));
}

void YangConf::init(const char *p_filename){
	char file1[180];
	memset(file1, 0, 180);
	char file_path_getcwd[180];
	// file_path_getcwd=(char *)malloc(FILEPATH_MAX);
	memset(file_path_getcwd, 0, 180);
	getcwd(file_path_getcwd, 180);
	sprintf(file1, "%s/%s", file_path_getcwd, p_filename);
	int len = strlen(file1) + 1;
	file = (char*) malloc(len);
	memset(file, 0, len);
	strcpy(file, file1);
}

int YangConf::IniReadValue(const char *section, const char *key, char *val) {
	FILE *fp;
	int i = 0;
	int lineContentLen = 0;
	int position = 0;
	int ret = 1;
	char lineContent[LINE_CONTENT_MAX_LEN];
	bool bFoundSection = false;
	bool bFoundKey = false;
	fp = fopen(file, "r");
	if (fp == NULL) {
		//char buf[150];
		//memset(buf,0,150);
		// getcwd(buf, sizeof(buf));
		printf("%s: Opent file %s failed.\n", __FILE__, file);
		return -1;
	}
	while (feof(fp) == 0) {
		memset(lineContent, 0, LINE_CONTENT_MAX_LEN);
		fgets(lineContent, LINE_CONTENT_MAX_LEN, fp);
		if ((lineContent[0] == ';') || (lineContent[0] == '\0')
				|| (lineContent[0] == '\r') || (lineContent[0] == '\n')) {
			continue;
		}

		//check section
		if (strncmp(lineContent, section, strlen(section)) == 0) {
			bFoundSection = true;
			//printf("Found section = %s\n", lineContent);
			while (feof(fp) == 0) {
				memset(lineContent, 0, LINE_CONTENT_MAX_LEN);
				fgets(lineContent, LINE_CONTENT_MAX_LEN, fp);
				//check key
				if (strncmp(lineContent, key, strlen(key)) == 0) {
					bFoundKey = true;
					lineContentLen = strlen(lineContent);
					//find value
					for (i = strlen(key); i < lineContentLen; i++) {
						if (lineContent[i] == '=') {
							position = i + 1;
							break;
						}
					}
					if (i >= lineContentLen)
						break;
					strncpy(val, lineContent + position,
							strlen(lineContent + position));
					lineContentLen = strlen(val);
					for (i = 0; i < lineContentLen; i++) {
						if ((lineContent[i] == '\0') || (lineContent[i] == '\r')
								|| (lineContent[i] == '\n')) {
							val[i] = '\0';
							break;
						}
					}
				} else if (lineContent[0] == '[') {
					break;
				}
			}
			break;
		}
	}
	if (!bFoundSection) {
		ret = -1;
	} //printf("No section = %s\n", section);
	else if (!bFoundKey) {
		ret = -1;
	} //printf("No key = %s\n", key);
	fclose(fp);
	yangTrim(val, strlen(val));
	return ret;
}
int YangConf::readStringValue1(const char *section, const char *key,
		char *val) {
	char sect[SECTION_MAX_LEN];
	//printf("section = %s, key = %s, file = %s\n", section, key, file);
	if (section == NULL || key == NULL || val == NULL || file == NULL) {
		printf("%s: input parameter(s) is NULL!\n", __func__);
		return -1;
	}

	memset(sect, 0, SECTION_MAX_LEN);
	sprintf(sect, "[%s]", section);
	//printf("reading value...\n");
	return IniReadValue(sect, key, val);

	// return 1;
}
int YangConf::readStringValue(const char *section, const char *key,
		char *val, const char *p_defaultStr) {
	char sect[SECTION_MAX_LEN];
	//printf("section = %s, key = %s, file = %s\n", section, key, file);
	if (section == NULL || key == NULL || val == NULL || file == NULL) {
		printf("%s: input parameter(s) is NULL!\n", __func__);
		strcpy(val, p_defaultStr);
		return -1;
	}

	memset(sect, 0, SECTION_MAX_LEN);
	sprintf(sect, "[%s]", section);
	//printf("reading value...\n");
	int ret = IniReadValue(sect, key, val);
	if (ret == -1)
		strcpy(val, p_defaultStr);
	return ret;
}
