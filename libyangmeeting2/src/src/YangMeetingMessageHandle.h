/*
 * YangMeetingHandle.cpp
 *
 *  Created on: 2019年10月13日
 *      Author: yang
 */

#ifndef SRC_YANGMEETING_SRC_YANGMEETINGMESSAGEHANDLE_H_
#define SRC_YANGMEETING_SRC_YANGMEETINGMESSAGEHANDLE_H_
#include "YangMeetingHandleImpl.h"
#include <yangmeeting/YangMeetingMessageI.h>
#include <yangutil/sys/YangSysMessageHandle.h>
#include <vector>
#include <mutex>
#include "yangutil/sys/YangThread.h"
#include "yangmeeting/yangmeetingtype.h"
#include "yangmeeting/YangMeetingHandle.h"
using namespace std;

class YangMeetingMessageHandle :public YangSysMessageHandle{
public:
	YangMeetingMessageHandle(YangMeetingContext* pcontext,YangReceiveMessageI* prec);
	virtual ~YangMeetingMessageHandle();

	void initAll();
	void deleteAll();
	void handleMessage(YangSysMessage* mss);
	YangMeetingHandleImpl* m_videoMeeting;
private:
	YangMeetingContext *m_context;
	YangReceiveMessageI *m_receive;

};

#endif /* SRC_YANGMEETING_SRC_YANGMEETINGMESSAGEHANDLE_H_ */
