#ifndef YANGINI_H
#define YANGINI_H
#include "yangtype.h"
#include "YangLog.h"
class YangWinIni:public YangConfig
{
    public:
	YangWinIni(const char* p_filename);
    ~YangWinIni();
    public:
        void init(const char* p_filename);
        int IniReadValue(const char* section,const char* key, char* val);
        int readStringValue(const char* section,const char* key, char* val,const char *p_defaultStr);
        int readIntValue(const char* section,const char* key,int p_defaultInt);
        void test();
        void initIniConfig();
        void initAll();

       // Config config;
    protected:

    private:
        //curPath;
        char *file;

        int readStringValue1(const char* section,const char* key, char* val);
            //FILE* fp;

};

#endif // YANGINI_H
