/*
 * YangUserContext.h
 *
 *  Created on: 2021年4月3日
 *      Author: yang
 */

#ifndef YANGDATASERVER_YANGUSERCONTEXT_H_
#define YANGDATASERVER_YANGUSERCONTEXT_H_
#include "YangCommon.h"
#include <st.h>
struct YangUserContext{
	YangUser user;
	int userId;
	int meetingId;
	st_netfd_t fd;
};



#endif /* YANGDATASERVER_YANGUSERCONTEXT_H_ */
