#ifndef INCLUDE_YANGUTIL_SYS_YANGMESSAGEI_H_
#define INCLUDE_YANGUTIL_SYS_YANGMESSAGEI_H_


class YangSysMessageI {
public:
	YangSysMessageI(){};
	virtual ~YangSysMessageI(){};
	virtual void success()=0;
	virtual void failure(int errcode)=0;
};
struct YangSysMessage {
	int uid;
	int messageId;
	int handleState;
	YangSysMessageI *handle;
};

void yang_post_message(int st, int uid,YangSysMessageI *mhandle);
void yang_post_state_message(int st, int uid, int handleState,YangSysMessageI *mhandle);
#endif /* INCLUDE_YANGUTIL_SYS_YANGMESSAGEI_H_ */
