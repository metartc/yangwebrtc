
#ifndef YANGSOCKETUTIL_H_
#define YANGSOCKETUTIL_H_
#include <string>
class YangSocketUtil {
public:
	YangSocketUtil();
	virtual ~YangSocketUtil();
	std::string getLocalInfo(void);
};

#endif /* YANGSOCKETUTIL_H_ */
