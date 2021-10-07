#ifndef YANGRTP_Httpurl_H_
#define YANGRTP_Httpurl_H_
#include <string>
#include <string.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangstream/YangStreamType.h>
#include <yangutil/sys/YangLoadLib.h>
#include "curl/curl.h"
using namespace std;
class YangSrsHttp
{
public:
	YangSrsHttp();
	~YangSrsHttp();
	static size_t WriteFunction(void* input, size_t uSize, size_t uCount, void* avg);
	int32_t querySrs(char* purl, string psdp);
	SrsSdpResponseType m_srs;
private:
	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
	CURLcode (*yang_curl_global_init)(long flags);
	CURL* (*yang_curl_easy_init)(void);
	CURLcode (*yang_curl_easy_setopt)(CURL *curl, CURLoption option, ...);
	struct curl_slist* (*yang_curl_slist_append)(struct curl_slist *,
	                                                 const char *);
	CURLcode (*yang_curl_easy_perform)(CURL *curl);
	void (*yang_curl_easy_cleanup)(CURL *curl);
	void (*yang_curl_global_cleanup)(void);
};

#endif
