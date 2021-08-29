
#ifndef INCLUDE_YANGUTIL_SYS_YANGFILEUTIL_H_
#define INCLUDE_YANGUTIL_SYS_YANGFILEUTIL_H_

class YangFileUtil {
public:
	YangFileUtil();
	virtual ~YangFileUtil();
	int hasPath(const char* path);
	int createPath(const char* path);
	int getCurpath(char* path);
};

#endif /* INCLUDE_YANGUTIL_SYS_YANGFILEUTIL_H_ */
