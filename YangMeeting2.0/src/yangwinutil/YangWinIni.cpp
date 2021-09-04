#include "stdio.h"
#include "fcntl.h"
#include "memory.h"
#include "stdlib.h"
#include <unistd.h>
#include "YangIni.h"
#include "YangWinIni.h"
#define SECTION_MAX_LEN 256
#define STRVALUE_MAX_LEN 256
#define LINE_CONTENT_MAX_LEN 256
#define FILEPATH_MAX 80

YangUtilFactory::YangUtilFactory() {

}
YangConfig* YangUtilFactory::createConfig(const char *p_filename) {
	return new YangWinIni(p_filename);
}
YangWinIni::YangWinIni(const char *p_filename) {
	file = NULL;

	isPlayAudio = 0;
	isPlayVideo = 0;
	isPubAudio = 0;
	isPubVideo = 0;
	isUsingHwDec = 0, isUsingHwEnc = 0;
	hasAec = 0;
	httpPort = 8080;
	audioEncoderType = 0;
	audioDecoderType = 0;
	audioPlayType = 0;
	memset(serverIP, 0, sizeof(serverIP));
	//memset(playAudioStreams, 0, sizeof(playAudioStreams));
	//memset(playVideoStreams, 0, sizeof(playVideoStreams));
	//memset(playAudioStreams, 0, sizeof(pubAudioStreams));
	//memset(playVideoStreams, 0, sizeof(pubVideoStreams));
	//m_log=NULL;
	logLevel = 0;

	Video_Width = 1280;
	Video_Height = 720;
	Video_Width_Zb = 1280;
	Video_Height_Zb = 720;

	Audio_Sample = 44100;
	Audio_Cache_Size = 512;
	//config.Audio_Cache_Num= 500;
	Audio_Bitrate = 128;
	//config.Cache_Size= 250;
	Del_Frame_Count = 2;
	Video_Bit_Count = 16;
	//config.URL_STR= "rtmp:127.0.0.1/live/test";
	Frame_Num = 10;
	Audio_Frame_Size = 417;
	Lame_Sample_Number = 1152;
	Video_Rate = 2048;
	Audio_Cache_Num = 512;
	Video_Cache_Num = 50;
	EVideo_Cache_Num = 50;
	capture_type = 16;
	rtmp_port = 1935;
	hasZbAudio = 1;
	File_Size =
			(capture_type == 12) ?
					Video_Width * Video_Height * 3 / 2 :
					Video_Width * Video_Height * 2;
	X264_fps_num = 10;
	X264_keyint_max = 30;
	X264_threads = 2;
	is_aacZb = 0;
	init(p_filename);
	initAll();
	m_log = new YangLog();
	//m_log=new YangLog();
	//yang_config.ini
}

YangWinIni::~YangWinIni() {
	SAFE_FREE(file);
	SAFE_DELETE(m_log);
	//dtor
}
void YangWinIni::init(const char *p_filename) {
	char file1[180];
	memset(file1, 0, 180);
	char file_path_getcwd[180];
	// file_path_getcwd=(char *)malloc(FILEPATH_MAX);
	memset(file_path_getcwd, 0, 180);
	getcwd(file_path_getcwd, 180);
	sprintf(file1, "%s/%s", file_path_getcwd, p_filename);
	int len = strlen(file1) + 1;
	file = (char*) malloc(len);
	memset(file, 0, len);
	strcpy(file, file1);
	//printf("file==%s=====%s===%d==================%s\n",file_path_getcwd,file1,len,file);

}

void YangWinIni::test() {
	//IniReadValue("zb","Video_Width_Zb");
	init("yang_config.ini");
	char ss[128];
	memset(ss, 0, 128);
	readStringValue("zb", "controlIP1", ss, "192.168.0.106");
	printf("width=%d,ip=%s", readIntValue("zb", "Video_Width_Zb1", 2), ss);
}
void yangTrim(char *ps, int len) {
	for (int i = 0; i < len; i++)
		if (*(ps + i) == 0x0a || *(ps + i) == 0x0d)
			*(ps + i) = 0x00;
}
int YangWinIni::IniReadValue(const char *section, const char *key, char *val) {
	FILE *fp;
	int i = 0;
	int lineContentLen = 0;
	int position = 0;
	int ret = 1;
	char lineContent[LINE_CONTENT_MAX_LEN];
	bool bFoundSection = false;
	bool bFoundKey = false;
	fp = fopen(file, "r");
	if (fp == NULL) {
		//char buf[150];
		//memset(buf,0,150);
		// getcwd(buf, sizeof(buf));
		printf("%s: Opent file %s failed.\n", __FILE__, file);
		return -1;
	}
	while (feof(fp) == 0) {
		memset(lineContent, 0, LINE_CONTENT_MAX_LEN);
		fgets(lineContent, LINE_CONTENT_MAX_LEN, fp);
		if ((lineContent[0] == ';') || (lineContent[0] == '\0')
				|| (lineContent[0] == '\r') || (lineContent[0] == '\n')) {
			continue;
		}

		//check section
		if (strncmp(lineContent, section, strlen(section)) == 0) {
			bFoundSection = true;
			//printf("Found section = %s\n", lineContent);
			while (feof(fp) == 0) {
				memset(lineContent, 0, LINE_CONTENT_MAX_LEN);
				fgets(lineContent, LINE_CONTENT_MAX_LEN, fp);
				//check key
				if (strncmp(lineContent, key, strlen(key)) == 0) {
					bFoundKey = true;
					lineContentLen = strlen(lineContent);
					//find value
					for (i = strlen(key); i < lineContentLen; i++) {
						if (lineContent[i] == '=') {
							position = i + 1;
							break;
						}
					}
					if (i >= lineContentLen)
						break;
					strncpy(val, lineContent + position,
							strlen(lineContent + position));
					lineContentLen = strlen(val);
					for (i = 0; i < lineContentLen; i++) {
						if ((lineContent[i] == '\0') || (lineContent[i] == '\r')
								|| (lineContent[i] == '\n')) {
							val[i] = '\0';
							break;
						}
					}
				} else if (lineContent[0] == '[') {
					break;
				}
			}
			break;
		}
	}
	if (!bFoundSection) {
		ret = -1;
	} //printf("No section = %s\n", section);
	else if (!bFoundKey) {
		ret = -1;
	} //printf("No key = %s\n", key);
	fclose(fp);
	yangTrim(val, strlen(val));
	return ret;
}

int YangWinIni::readStringValue(const char *section, const char *key,
		char *val, const char *p_defaultStr) {
	char sect[SECTION_MAX_LEN];
	//printf("section = %s, key = %s, file = %s\n", section, key, file);
	if (section == NULL || key == NULL || val == NULL || file == NULL) {
		printf("%s: input parameter(s) is NULL!\n", __func__);
		strcpy(val, p_defaultStr);
		return -1;
	}

	memset(sect, 0, SECTION_MAX_LEN);
	sprintf(sect, "[%s]", section);
	//printf("reading value...\n");
	int ret = IniReadValue(sect, key, val);
	if (ret == -1)
		strcpy(val, p_defaultStr);
	return ret;
}

int YangWinIni::readStringValue1(const char *section, const char *key,
		char *val) {
	char sect[SECTION_MAX_LEN];
	//printf("section = %s, key = %s, file = %s\n", section, key, file);
	if (section == NULL || key == NULL || val == NULL || file == NULL) {
		printf("%s: input parameter(s) is NULL!\n", __func__);
		return -1;
	}

	memset(sect, 0, SECTION_MAX_LEN);
	sprintf(sect, "[%s]", section);
	//printf("reading value...\n");
	return IniReadValue(sect, key, val);

	// return 1;
}

int YangWinIni::readIntValue(const char *section, const char *key,
		int p_defaultInt) {
	char strValue[STRVALUE_MAX_LEN];
	memset(strValue, '\0', STRVALUE_MAX_LEN);
	if (readStringValue1(section, key, strValue) != 1) {
		return p_defaultInt;
	}
	return (atoi(strValue));
}

void YangWinIni::initIniConfig() {
	Video_Width = readIntValue("record", "Video_Width", 1280);
	Video_Height = readIntValue("record", "Video_Height", 720);
	Video_Width_Zb = readIntValue("zb", "Video_Width_Zb", 1280);
	Video_Height_Zb = readIntValue("zb", "Video_Height_Zb", 720);
	Audio_Sample = readIntValue("record", "Audio_Sample", 44100);
	Audio_Cache_Size = readIntValue("record", "Audio_Cache_Size", 512);
	Audio_Bitrate = readIntValue("record", "Audio_Bitrate", 128);
	Del_Frame_Count = readIntValue("record", "Del_Frame_Count", 2);
	Video_Bit_Count = readIntValue("record", "Video_Bit_Count", 16);
	Frame_Num = readIntValue("record", "Frame_Num", 10);
	Audio_Frame_Size = readIntValue("record", "Audio_Frame_Size", 417);
	Lame_Sample_Number = readIntValue("record", "Lame_Sample_Number", 1152);
	Video_Rate = readIntValue("record", "Video_Rate", 2048);
	Audio_Cache_Num = readIntValue("record", "Audio_Cache_Num", 512);
	Video_Cache_Num = readIntValue("record", "Video_Cache_Num", 50);
	EVideo_Cache_Num = readIntValue("record", "EVideo_Cache_Num", 50);
	capture_type = readIntValue("record", "capture_type", 16);
	rtmp_port = readIntValue("sys", "rtmp_port", 1935);
	hasZbAudio = readIntValue("record", "hasZbAudio", 1);
	X264_fps_num = readIntValue("record", "X264_fps_num", 10);
	X264_fps_num_Res = readIntValue("record", "X264_fps_num_Res", 10);
	X264_keyint_max = readIntValue("record", "X264_keyint_max", 30);
	X264_keyint_max_Res = readIntValue("record", "X264_keyint_max_Res", 30);
	X264_threads = readIntValue("record", "X264_threads", 2);
	X264_threads_Res = readIntValue("record", "X264_threads_Res", 2);
	is_aacZb = readIntValue("zb", "is_aacZb", 1);
	//usingHw= readIntValue("sys","usingHw",0);
	File_Size =
			capture_type == 12 ?
					Video_Width * Video_Height * 3 / 2 :
					Video_Width * Video_Height * 2;
}

void YangWinIni::initAll() {
	/**g_initconfig11(&config);
	 sprintf(serverIP,"%s","192.168.0.105");
	 sprintf(playVideoStreams,"%s","lubo1-2");
	 sprintf(playAudioStreams,"%s","lubo1-1a");
	 isPlayAudio=0;
	 isPlayVideo=1;**/

	//initConfig(&config);
	//g_initconfig11();
	initIniConfig();

	//memset(meetingId, 0, sizeof(meetingId));
	meetingId = readIntValue("hd", "meetingId", 1001);
	//memset(userId, 0, sizeof(userId));
	userId = readIntValue("hd", "userId", 1001);

	//memset(playVideoStreams, 0, sizeof(playVideoStreams));
	//readStringValue("zb", "playVideoStreams", playVideoStreams, "lubo1-1");

	//memset(playAudioStreams, 0, sizeof(playAudioStreams));
//	readStringValue("zb", "playAudioStreams", playAudioStreams, "lubo1-1a");

	//memset(pubVideoStreams, 0, sizeof(pubVideoStreams));
	//readStringValue("zb", "pubVideoStreams", pubVideoStreams, "lubo1-1");

	//memset(pubAudioStreams, 0, sizeof(pubAudioStreams));
	//readStringValue("zb", "pubAudioStreams", pubAudioStreams, "lubo1-1a");
	isPlayAudio = readIntValue("zb", "isPlayAudio", 0);
	isPlayVideo = readIntValue("zb", "isPlayVideo", 0);
	isPubAudio = readIntValue("zb", "isPubAudio", 0);
	isPubVideo = readIntValue("zb", "isPubVideo", 0);
	isUsingHwDec = readIntValue("sys", "isUsingHwDec", 0);
	isUsingHwEnc = readIntValue("sys", "isUsingHwEnc", 0);
	audioEncoderType = readIntValue("sys", "audioEncoderType", 0);
	audioDecoderType = readIntValue("sys", "audioDecoderType", 0);
	audioPlayType = readIntValue("sys", "audioPlayType", 0);
	hasAec = readIntValue("sys", "hasAec", 0);
	httpPort = readIntValue("sys", "httpPort", 8080);
	logLevel = readIntValue("sys", "logLevel", 8080);

	memset(serverIP, 0, sizeof(serverIP));
	readStringValue("zb", "serverIP", serverIP, "192.168.0.4");
	memset(app, 0, sizeof(app));
	readStringValue("zb", "app", app, "live");
	memset(userName, 0, sizeof(userName));
	readStringValue("hd", "userName", userName, "学校1");
	//printf("\n*************************httpPort==%d\n",httpPort);
}
