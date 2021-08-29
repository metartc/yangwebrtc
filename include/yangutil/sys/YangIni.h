#ifndef __YYangIni__
#define __YYangIni__
#include "../yangtype.h"

class YangIni {
public:
	YangIni();
	~YangIni();
public:
	void init(const char *p_filename);
	int IniReadValue(const char *section, const char *key, char *val);
	int readStringValue(const char *section, const char *key, char *val,
			const char *p_defaultStr);
	int readIntValue(const char *section, const char *key, int p_defaultInt);
	void initVideo(YangVideoInfo *video);
	void initAudio(YangAudioInfo *audio);
	void initSys(YangSysInfo *sys);
	void initEnc(YangVideoEncInfo *enc);

protected:

private:
	char *m_file;
	int readStringValue1(const char *section, const char *key, char *val);

};
#endif
