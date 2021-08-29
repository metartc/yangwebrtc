
#ifndef INCLUDE_YANGUTIL_SYS_YANGSTRINGUTIL_H_
#define INCLUDE_YANGUTIL_SYS_YANGSTRINGUTIL_H_
#include <vector>
#include <string.h>
#include <string>
using namespace std;
class YangStringUtil {
public:
	YangStringUtil();
	virtual ~YangStringUtil();
	void replaceAll(std::string& strBig, const std::string& strsrc, const std::string& strdst);
	vector<string> split(string s, char ch);
};

#endif /* INCLUDE_YANGUTIL_SYS_YANGSTRINGUTIL_H_ */
