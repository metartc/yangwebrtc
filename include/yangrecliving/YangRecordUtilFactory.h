/*
 * YangRecordUtilFactory.h
 *
 *  Created on: 2020年10月17日
 *      Author: yang
 */

#ifndef YANGRECLIVING_INCLUDE_YANGRECORDUTILFACTORY_H_
#define YANGRECLIVING_INCLUDE_YANGRECORDUTILFACTORY_H_
#include <yangrecliving/YangLivingType.h>
#include "YangVrHandle.h"
#include "YangLivingHandle.h"
#include "YangScreenHandle.h"
#include "yangutil/sys/YangSysMessageHandle.h"


class YangRecordUtilFactory {
public:
	YangRecordUtilFactory();
	virtual ~YangRecordUtilFactory();
	void createIni(const char* p_filename,YangRecordContext *pini);
	void createIni(const char* p_filename,YangRecordParam *pini);
	YangVrHandle* createRecordHandle(YangRecordContext *pini);
	YangLivingHandle* createLivingHandle(YangRecordContext *pini);
	YangScreenHandle* createScreenHandle(YangRecordContext *pini);

	YangSysMessageHandle* createVrRecMessageHandle(YangRecordContext *pini);
	YangSysMessageHandle* createRecMessageHandle(YangRecordContext *pini);

	YangVrHandle* getVrHandle(YangSysMessageHandle* pms);
	YangLivingHandle* getLivingHandle(YangSysMessageHandle* pms);
};

#endif /* YANGRECLIVING_INCLUDE_YANGRECORDUTILFACTORY_H_ */
