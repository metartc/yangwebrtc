/*
 * YangConnectionPool.h
 *
 *  Created on: 2019年3月11日
 *      Author: yang
 */

#ifndef YANGSQL_INCLUDE_YANGCONNECTIONPOOL_H_
#define YANGSQL_INCLUDE_YANGCONNECTIONPOOL_H_
#include <vector>
#include "mysql/mysql.h"
#include "YangConf.h"
struct YangPool{
	int useState;
	MYSQL* conn;
};
class YangConnectionPool {
public:
	YangConnectionPool();
	virtual ~YangConnectionPool();
	static std::vector<YangPool*> sqlPool;
	static YangPool* getConnection();
	static YangConf *conf;
	void init(YangConf *conf);
private:
	static int maxConnCount;
	static int initConnCount;



};

#endif /* YANGSQL_INCLUDE_YANGCONNECTIONPOOL_H_ */
