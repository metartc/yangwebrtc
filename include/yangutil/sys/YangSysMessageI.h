#ifndef INCLUDE_YANGUTIL_SYS_YANGMESSAGEI_H_
#define INCLUDE_YANGUTIL_SYS_YANGMESSAGEI_H_
#include <stdint.h>

class YangSysMessageI {
public:
	YangSysMessageI(){};
	virtual ~YangSysMessageI(){};
	virtual void success()=0;
	virtual void failure(int32_t errcode)=0;
};
struct YangSysMessage {
	int32_t uid;
	int32_t messageId;
	int32_t handleState;
	YangSysMessageI *handle;
};

void yang_post_message(int32_t st, int32_t uid,YangSysMessageI *mhandle);
void yang_post_state_message(int32_t st, int32_t uid, int32_t handleState,YangSysMessageI *mhandle);
#endif /* INCLUDE_YANGUTIL_SYS_YANGMESSAGEI_H_ */
