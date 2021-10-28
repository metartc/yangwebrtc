
#ifndef SRC_YANGUSERDATA_INCLUDE_YANGCOMMANDHANDLE_H_
#define SRC_YANGUSERDATA_INCLUDE_YANGCOMMANDHANDLE_H_
#include "yangsrt/YangTsdemux.h"
#include "yangmeeting/yangmeetingtype.h"
#include "YangUserDataBase.h"
#include "YangDataChannelConnection.h"
#include <string>
#include <vector>
using namespace std;
class YangCommandHandle:public YangDataChannelConnection,public ts_media_data_callback_I {
public:
	YangCommandHandle();
	virtual ~YangCommandHandle();
	YangUserDataBase* m_userData;
	YangMeetingContext *m_context;
	void decodeMessage(uint8_t* p,int32_t plen);
	void on_data_callback(SRT_DATA_MSG_PTR data_ptr, uint32_t  media_type,
				uint64_t dts, uint64_t pts);
public:
	int32_t login(string loginStr);
	int32_t logout();

	int32_t pubVideo();
	int32_t unPubVideo();
	int32_t pubAudio();
	int32_t unPubAudio();
	int32_t sendCommand(int32_t pcommandId);
	int32_t sendCommand(int32_t pcommandId,uint8_t *p);
	int32_t sendRoomInfoUser();
private:
	YangTsdemux demux;

	void handleMessage(uint8_t* p,int32_t plen);
	void parseCommand(uint8_t *p, int32_t plen);
	void handleCommand(int32_t puserid,int32_t paimuserid,int32_t pcommandid,uint8_t* content);

private:
	void split(string s, vector<string> *sv, const char flag);
	int32_t getConfUserInfosResult(uint8_t *p, int32_t plen);
};

#endif /* SRC_YANGUSERDATA_INCLUDE_YANGCOMMANDHANDLE_H_ */
