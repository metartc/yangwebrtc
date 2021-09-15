#include "stdio.h"
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <yangutil/sys/YangFile.h>
#include <iostream>
YangFile::YangFile() {


}

YangFile::~YangFile() {

}

int32_t YangFile::hasPath(const char* path){
	if( access(path, F_OK)!=0) return 1;
	if( access(path, W_OK)!=0) return 2;
	return 0;
}

int32_t YangFile::createPath(const char* path){

    #ifdef _WIN32
    return mkdir(path);
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
