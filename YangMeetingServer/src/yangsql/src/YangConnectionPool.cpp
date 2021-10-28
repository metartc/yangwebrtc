/*
 * YangConnectionPool.cpp
 *
 *  Created on: 2019年3月11日
 *      Author: yang
 */

#include "../include/YangConnectionPool.h"

YangConnectionPool::YangConnectionPool() {


}

YangConnectionPool::~YangConnectionPool() {
	for(int i=0;i<(int)sqlPool.size();i++){
		mysql_close(sqlPool.at(i)->conn);
			delete sqlPool.at(i)->conn;
			sqlPool.at(i)->conn=NULL;
			delete sqlPool.at(i);
			sqlPool.at(i)=NULL;
	}
	sqlPool.clear();
}
YangConf* YangConnectionPool::conf=NULL;
std::vector<YangPool*> YangConnectionPool::sqlPool;
int YangConnectionPool::maxConnCount=0;
int YangConnectionPool::initConnCount=0;
void YangConnectionPool::init(YangConf *pconf){
	if(initConnCount>0) return;
	conf=pconf;
	maxConnCount=conf->m_maxConnectionCount;
	initConnCount=conf->m_initConnectionCount;
	for(int i=0;i<initConnCount;i++){
		//YangPool yp;
		sqlPool.push_back(new YangPool());
		sqlPool.back()->conn=new MYSQL();
		sqlPool.back()->useState=0;
		mysql_init(sqlPool.back()->conn);
		mysql_set_character_set(sqlPool.back()->conn, "utf8");
		if(!mysql_real_connect(sqlPool.back()->conn, conf->m_databaseHost, conf->m_userName, conf->m_password, conf->m_databaseName, conf->m_databasePort, NULL, 0)){
			printf("\nconnect mysql error!");
		}
	//	printf("\nadd mysql conn .....%d\n",i);
	}
}

YangPool* YangConnectionPool::getConnection(){
	for(int i=0;i<(int)sqlPool.size();i++){
		if(sqlPool.at(i)->useState==0){
			sqlPool.at(i)->useState=1;
			return sqlPool.at(i);
		}
	}
	sqlPool.push_back(new YangPool());
	sqlPool.back()->conn=new MYSQL();
	sqlPool.back()->useState=0;
	mysql_init(sqlPool.back()->conn);
	mysql_set_character_set(sqlPool.back()->conn, "utf8");
	if(!mysql_real_connect(sqlPool.back()->conn, conf->m_databaseHost, conf->m_userName, conf->m_password, conf->m_databaseName, conf->m_databasePort, NULL, 0)){
		printf("\nconnect mysql error!");
	}
	return sqlPool.back();
}
