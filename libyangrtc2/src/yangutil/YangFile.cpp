#include "stdio.h"
#include <string>
#include <yangutil/yang_unistd.h>
#include <sys/stat.h>
#include <yangutil/sys/YangFile.h>
#include <iostream>

#ifdef _MSC_VER
#include <direct.h>
#endif
YangFile::YangFile() {


}

YangFile::~YangFile() {

}

int32_t YangFile::hasPath(const char* path){
  //  _waccess(path,0);
#ifndef _MSC_VER
	if( access(path, F_OK)!=0) return 1;
	if( access(path, W_OK)!=0) return 2;
#endif
	return 0;
}

int32_t YangFile::createPath(const char* path){

    #ifdef _WIN32
    #ifdef _MSC_VER
    return _mkdir(path);
    #else
        return mkdir(path);
    #endif
    #else
    mode_t mode=0755;
    return mkdir(path,mode);
    #endif

}
int32_t YangFile::getCurpath(char* path){

	return 0;
}
/**
int32_t createDirectory(std::string path)
{
	int32_t len = path.length();
	char tmpDirPath[256] = { 0 };
	for (int32_t i = 0; i < len; i++)
	{
		tmpDirPath[i] = path[i];
		if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/')
		{
			if (_access(tmpDirPath, 0) == -1)
			{
				int32_t ret = _mkdir(tmpDirPath);
				if (ret == -1) return ret;
			}
		}
	}
	return 0;
}
**/
