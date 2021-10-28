/*
 * YangMysql.cpp
 *
 *  Created on: 2021年3月6日
 *      Author: yang
 */

#include "../include/YangMysqlBase.h"

#include <stdio.h>
YangMysqlBase::YangMysqlBase() {

	m_sql=NULL;
	//m_conf=NULL;
}

YangMysqlBase::~YangMysqlBase() {
	//if(m_sql) {
	//	mysql_close(m_sql);
	//	delete m_sql;
	//}
	//printf("\nreuse mysql..................\n");
	m_sql->useState=0;
	m_sql=NULL;
	//m_conf=NULL;

}

void YangMysqlBase::initMysql(){//YangConf *conf) {
	if(!m_sql){
		m_sql=YangConnectionPool::getConnection();
		//m_sql=new MYSQL();
		//mysql_init(m_sql);
		//mysql_set_character_set(m_sql, "utf8");
		//if(!mysql_real_connect(m_sql, conf->m_databaseHost, conf->m_userName, conf->m_password, conf->m_databaseName, conf->m_databasePort, NULL, 0)){
		//	printf("\nconnect mysql error!");
		//}
	}

}
int YangMysqlBase::exeSql(string str){
	initMysql();
	int ret=mysql_query(m_sql->conn, str.c_str());
	if(ret){
		printf("sql query fail!\n");
	}
	return ret;
}


vector<vector<string>> YangMysqlBase::querySql(char *psql) {
	initMysql();
	vector<vector<string>> vRet;
	//if(strchr(psql,'\'')) return vRet;
	//printf("\n*******************start query..%s\n", psql);
	//if(mysql_query(m_sql,);
	if (mysql_query(m_sql->conn, psql)) {
		string vsRet(mysql_error(m_sql->conn));
		return vRet;
	}
	MYSQL_RES *res = mysql_store_result(m_sql->conn);
	if (res == NULL) {
		return vRet;
	}
	MYSQL_ROW rows;
	int num_fields = mysql_num_fields(res);
	while (rows = mysql_fetch_row(res)) {
		int i = 0;
		vector<string> vItem;
		vItem.resize(num_fields);
		for (i = 0; i < num_fields; ++i) {
			vItem[i] = rows[i];
		}
		vRet.push_back(vItem);
	}
	const char *str1 = mysql_error(m_sql->conn);
	mysql_free_result(res);
	res = NULL;
	return vRet;
}
