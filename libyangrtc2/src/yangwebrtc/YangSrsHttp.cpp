#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangwebrtc/YangRtcSdp.h>
#include <yangwebrtc/YangSrsHttp.h>
#include <yangutil/sys/YangString.h>
#include <yangutil/sys/YangLog.h>

#include <vector>
using namespace std;

vector<string> split1(string s, char ch) {
	int32_t len = 0;
	vector<string> ret;
	for (size_t i = 0; i < s.length(); i++) {
		if (s[i] == ch) {
			ret.push_back(s.substr(0, i));
			ret.push_back(s.substr(i+1, s.length()-1));
			return ret;
		}
		else {
			len++;
		}
	}
	
	return ret;
}

void YangSrsHttp::loadLib(){

	yang_curl_global_init=(CURLcode (*)(long flags))m_lib.loadFunction("curl_global_init");
	yang_curl_easy_init=(CURL* (*)(void))m_lib.loadFunction("curl_easy_init");
	yang_curl_easy_setopt=(CURLcode (*)(CURL *curl, CURLoption option, ...))m_lib.loadFunction("curl_easy_setopt");
	yang_curl_slist_append=(struct curl_slist* (*)(struct curl_slist *,
		                                                 const char *))m_lib.loadFunction("curl_slist_append");
	yang_curl_easy_perform=(CURLcode (*)(CURL *curl))m_lib.loadFunction("curl_easy_perform");
	yang_curl_easy_cleanup=(void (*)(CURL *curl))m_lib.loadFunction("curl_easy_cleanup");
	yang_curl_global_cleanup=(void (*)(void))m_lib.loadFunction("curl_global_cleanup");
}
void YangSrsHttp::unloadLib(){
	yang_curl_global_init=NULL;
	yang_curl_easy_init=NULL;
	yang_curl_easy_setopt=NULL;
	yang_curl_slist_append=NULL;
	yang_curl_easy_perform=NULL;
	yang_curl_easy_cleanup=NULL;
	yang_curl_global_cleanup=NULL;
}

YangSrsHttp::YangSrsHttp()
{
	unloadLib();
	m_lib.loadObject("libcurl");
        loadLib();
}
YangSrsHttp::~YangSrsHttp()
{
	unloadLib();
	m_lib.unloadObject();
}
size_t YangSrsHttp::WriteFunction(void* input, size_t uSize, size_t uCount, void* avg)
{
	size_t uLen = uSize * uCount;
	string* pStr = (string*)(avg);
	pStr->append((char*)(input), uLen);
	return uLen;
}
void string_replace(std::string& strBig, const std::string& strsrc, const std::string& strdst)
{
	std::string::size_type pos = 0;
	std::string::size_type srclen = strsrc.size();
	std::string::size_type dstlen = strdst.size();

	while ((pos = strBig.find(strsrc, pos)) != std::string::npos)
	{
		strBig.replace(pos, srclen, strdst);
		pos += dstlen;
	}
}
int32_t YangSrsHttp::querySrs(char* purl1, string psdp)
{
	
	CURL* pCurl = NULL;
	CURLcode code;
	code = yang_curl_global_init(CURL_GLOBAL_DEFAULT);
	if (code != CURLE_OK) yang_error(" http global init error");
	pCurl = yang_curl_easy_init();
	if (pCurl == NULL) yang_error(" http easy init error");

	
	string sBuffer;
	yang_curl_easy_setopt(pCurl, CURLOPT_URL, purl1); 	//
	yang_curl_easy_setopt(pCurl, CURLOPT_POST, 1);
	yang_curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, psdp.c_str());
	yang_curl_easy_setopt(pCurl, CURLOPT_POSTFIELDSIZE, psdp.length());
	//curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 20); 			
	//curl_easy_setopt(pCurl, CURLOPT_HEADER, 1); 			

	//curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, "");
	yang_curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, &WriteFunction);
	yang_curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &sBuffer);


	//string header = "contentType: application/json";
	curl_slist* pHeaders = NULL;
	//pHeaders = curl_slist_append(pHeaders, "Content-Type:application/json;charset=UTF-8");
	pHeaders = yang_curl_slist_append(pHeaders, "Content-Type:application/json;charset=UTF-8");
	yang_curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pHeaders);
	yang_curl_easy_setopt(pCurl, CURLOPT_POST, 1);
	code = yang_curl_easy_perform(pCurl);
	if (code != CURLE_OK) {
		//string err = "curl_easy_perform() Err...code=="+ to_string((int)code);
		printf("curl_easy_perform() Err...code==%d",(int)code);
		return 1;
	}


	yang_curl_easy_cleanup(pCurl);
	yang_curl_global_cleanup();
	string_replace(sBuffer, "{", "");
	string_replace(sBuffer, "}", "");
	string_replace(sBuffer, "\\r\\n", "\r\n");

	vector<string> sb=yang_split(sBuffer, ',');
	for (int32_t i = 0; i < (int)sb.size(); i++) {
		vector<string> sb1 = split1(sb.at(i), ':');
		string key = sb1.at(0);
		string value = sb1.at(1);
		string_replace(key, "\"", "");
		string_replace(value, "\"", "");
		if (key.find("code") != key.npos) m_srs.retcode = value;
		if (key.find("server") != key.npos) m_srs.serverIp = value;
		if (key.find("sdp") != key.npos) {
			m_srs.sdp = value;
			//printf("\n%s\n",value.c_str());
		}
		if (key.find("sessionid") != key.npos) m_srs.sessionid = value;
		
	}
	if(m_srs.retcode!="0") return 1;
	return Yang_Ok;

}
