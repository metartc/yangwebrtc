#include "yangutil/sys/YangLog.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <mutex>

#ifdef _WIN32
#include <io.h>
#endif
#include <sys/time.h>
class YangLogFile {
public:
	std::mutex m_lock;
	FILE *fmsg = NULL;
	YangLogFile() {
		fmsg = NULL;
	}
	;
	~YangLogFile() {
		if (fmsg)
			fclose(fmsg);
		fmsg = NULL;
	}
	;
	void writeFileData(char *p, int32_t plen) {
		//if(!fmsg) return;
		m_lock.lock();
		fwrite(p, plen, 1, fmsg);
		fflush(fmsg);
		m_lock.unlock();
	}
	;

};

YangLogFile *g_yangLogFile = NULL;
//#define STR_DATE_TIME_LEN 32
//#define MAX_PRINT_LEN	4096
int64_t yang_gettime_ms(void) //rturn micro-second
		{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((int64_t) tv.tv_sec * 1000000 + tv.tv_usec) / 1000;
}

void yang_gettime_fmt(char *dst, int64_t cur_time_sec) {
	time_t rawtime;
	char timef[32] = { 0 };

	time(&rawtime);
	rawtime = (time_t) cur_time_sec;
	struct tm *timeinfo = localtime(&rawtime);
	strftime(timef, sizeof(timef), "%Y-%m-%d %H:%M:%S", timeinfo);
	strcpy(dst, timef);
	return;
}

static char const *YANG_LOG_LEVEL_NAME[] = { "FATAL", "ERROR", "WARNING",
		"INFO", "DEBUG", "TRACE" };

YangCLog::YangCLog() {
	//neednl = 0;

}

YangCLog::~YangCLog() {

}

void YangCLog::setLogFile(int32_t isSetLogFile) {
	if (!g_yangLogFile)
		g_yangLogFile = new YangLogFile();
	if (m_hasLogFile)
		return;
	m_hasLogFile = isSetLogFile;
	if (isSetLogFile) {
		char file1[300];
		memset(file1, 0, 300);
		char file_path_getcwd[255];
		memset(file_path_getcwd, 0, 255);
		if (getcwd(file_path_getcwd, 255)) {
			sprintf(file1, "%s/yang_log.log", file_path_getcwd);
			g_yangLogFile->fmsg = fopen(file1, "wb+");

		}

	}
}
void YangCLog::closeLogFile() {
	if (g_yangLogFile)
		delete g_yangLogFile;
	g_yangLogFile = NULL;
}

void YangCLog::log(int32_t level, const char *fmt, ...) {
	if (level > logLevel)
		return;
	char buf[4096];
	memset(buf, 0, 4096);
	//int32_t len=0;
	va_list args;
	va_start(args, fmt);
	//int32_t len =
	vsnprintf(buf, 4095, fmt, args);
	va_end(args);

	char cur_time[32] = { 0 };
	int64_t cur_time_msec = yang_gettime_ms();
	int64_t cur_time_sec = cur_time_msec / 1000;
	cur_time_msec = cur_time_msec - cur_time_sec * 1000;
	yang_gettime_fmt(cur_time, cur_time_sec);

	printf("%s:%ld Yang %s: %s\n", cur_time, cur_time_msec,
			YANG_LOG_LEVEL_NAME[level], buf);

	if (m_hasLogFile) {
		if (!g_yangLogFile)
			g_yangLogFile = new YangLogFile();
		char sf[4196];
		memset(sf, 0, 4196);
		int32_t sfLen = sprintf(sf, "%s:%ld Yang %s: %s\n", cur_time,
				cur_time_msec, YANG_LOG_LEVEL_NAME[level], buf);
		if (g_yangLogFile->fmsg)
			g_yangLogFile->writeFileData(sf, sfLen);
	}

}
int32_t yang_error_wrap(int32_t errcode, const char *fmt, ...) {
	char buf[4096];
	memset(buf, 0, 4096);
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, 4095, fmt, args);
	va_end(args);

	char cur_time[32] = { 0 };
	int64_t cur_time_msec = yang_gettime_ms();
	int64_t cur_time_sec = cur_time_msec / 1000;
	cur_time_msec = cur_time_msec - cur_time_sec * 1000;
	yang_gettime_fmt(cur_time, cur_time_sec);

	printf("%s:%ld Yang Error(%d): %s\n", cur_time, cur_time_msec, errcode,
			buf);

	if (YangCLog::m_hasLogFile) {
		if (!g_yangLogFile)
			g_yangLogFile = new YangLogFile();
		char sf[4196];
		memset(sf, 0, 4196);
		int32_t sfLen = sprintf(sf, "%s:%ld Yang Error(%d): %s\n", cur_time,
				cur_time_msec, errcode, buf);
		if (g_yangLogFile->fmsg)
			g_yangLogFile->writeFileData(sf, sfLen);
	}

	return errcode;
}

void YangCLog::logf(int32_t level, const char *fmt, ...) {
	if (level > logLevel)
		return;
	char buf[4096];
	memset(buf, 0, 4096);
	int32_t len = 0;
	va_list args;
	va_start(args, fmt);
	len = vsnprintf(buf, 4095, fmt, args);
	va_end(args);

	printf(buf);

	if (m_hasLogFile) {
		if (!g_yangLogFile)
			g_yangLogFile = new YangLogFile();
		if (g_yangLogFile->fmsg)
			g_yangLogFile->writeFileData(buf, len);
	}

}
int32_t YangCLog::logLevel = YANG_LOG_ERROR;
int32_t YangCLog::m_hasLogFile = 0;
void YangCLog::setLogLevel(int32_t plevel) {
	logLevel = plevel;
	if (logLevel > YANG_LOG_TRACE)
		logLevel = YANG_LOG_TRACE;
}

