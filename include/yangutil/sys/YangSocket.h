
#ifndef YANGSOCKETUTIL_H_
#define YANGSOCKETUTIL_H_
#include <string>
class YangSocket {
public:
	YangSocket();
	virtual ~YangSocket();
	std::string getLocalInfo(void);
};

#endif /* YANGSOCKETUTIL_H_ */
