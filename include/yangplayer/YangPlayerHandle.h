/*
 * YangPlayerHandle.h
 *
 *  Created on: 2021年5月26日
 *      Author: yang
 */

#ifndef INCLUDE_YANGPLAYER_YANGPLAYERHANDLE_H_
#define INCLUDE_YANGPLAYER_YANGPLAYERHANDLE_H_
#include <string>
#include <yangutil/buffer/YangVideoBuffer.h>
class YangPlayerHandle {
public:
	YangPlayerHandle(){};
	virtual ~YangPlayerHandle(){};
	virtual YangVideoBuffer* getVideoBuffer()=0;
	virtual void play(std::string url,int localport)=0;
	virtual int playRtc(int puid,std::string localIp,int localPort, std::string server, int pport,std::string app,std::string stream)=0;
	static YangPlayerHandle* createPlayerHandle();
};

#endif /* INCLUDE_YANGPLAYER_YANGPLAYERHANDLE_H_ */
