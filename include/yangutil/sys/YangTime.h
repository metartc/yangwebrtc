#ifndef TIME_H__
#define TIME_H__

#include <sys/time.h>
#include <stdlib.h>
#include <iostream>

#include "TypeCast.h"
using namespace std;

#define yang_get_system_time YangSystime::get_system_time
#define yang_update_system_time YangSystime::update_system_time

int64_t yang_get_milli_time();//haomiao
int64_t yang_get_micro_time();//weimiao
int64_t yang_get_milli_tick();//haomiao
int64_t yang_get_micro_tick();//weimiao
int64_t yang_get_nano_tick();//namiao
class YangSystime{
public:
	YangSystime();
	~YangSystime();

	static int64_t system_time_us_cache;
	static int64_t system_time_startup_time;
	static int64_t update_system_time();
	static int64_t get_system_time();
};

class YangNtp
{
public:
    uint64_t system_ms_;
    uint64_t ntp_;
    uint32_t ntp_second_;
    uint32_t ntp_fractions_;
public:
    YangNtp();
    virtual ~YangNtp();
public:
    static YangNtp from_time_ms(uint64_t ms);
    static YangNtp to_time_ms(uint64_t ntp);
public:
    static uint64_t kMagicNtpFractionalUnit;
};

class YangWallClock
{
public:
    YangWallClock();
    virtual ~YangWallClock();
public:
    /**
     * Current time in srs_utime_t.
     */
    virtual int64_t now();
};


#endif
