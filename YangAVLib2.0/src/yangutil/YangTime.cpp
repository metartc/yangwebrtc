
#include <yangutil/yangavinfotype.h>
#include "yangutil/sys/YangTime.h"
int64_t YangSystime::system_time_us_cache = 0;
int64_t YangSystime::system_time_startup_time = 0;
#define Yang_TIME_RESOLUTION_US 300*1000
int64_t YangSystime::update_system_time() {
	int64_t now_us = yang_get_micro_time();
	if (system_time_us_cache <= 0) {
		system_time_startup_time = system_time_us_cache = now_us;
		return system_time_us_cache;
	}
	// use relative time.
	int64_t diff = now_us - system_time_us_cache;
	diff = yang_max(0, diff);
	if (diff < 0 || diff > 1000 * Yang_TIME_RESOLUTION_US) {
		system_time_startup_time += diff;
	}
	system_time_us_cache = now_us;

	return system_time_us_cache;
}
int64_t YangSystime::get_system_time()
{
    if (system_time_us_cache <= 0) {
        update_system_time();
    }

    return system_time_us_cache;
}
YangSystime::YangSystime(){

}
YangSystime::~YangSystime(){

}

uint64_t YangNtp::kMagicNtpFractionalUnit = 1ULL << 32;

YangNtp::YangNtp()
{
    system_ms_ = 0;
    ntp_ = 0;
    ntp_second_ = 0;
    ntp_fractions_ = 0;
}

YangNtp::~YangNtp()
{
}

YangNtp YangNtp::from_time_ms(uint64_t ms)
{
    YangNtp srs_ntp;
    srs_ntp.system_ms_ = ms;
    srs_ntp.ntp_second_ = ms / 1000;
    srs_ntp.ntp_fractions_ = (static_cast<double>(ms % 1000 / 1000.0)) * kMagicNtpFractionalUnit;
    srs_ntp.ntp_ = (static_cast<uint64_t>(srs_ntp.ntp_second_) << 32) | srs_ntp.ntp_fractions_;
    return srs_ntp;
}

YangNtp YangNtp::to_time_ms(uint64_t ntp)
{
    YangNtp srs_ntp;
    srs_ntp.ntp_ = ntp;
    srs_ntp.ntp_second_ = (ntp & 0xFFFFFFFF00000000ULL) >> 32;
    srs_ntp.ntp_fractions_ = (ntp & 0x00000000FFFFFFFFULL);
    srs_ntp.system_ms_ = (static_cast<uint64_t>(srs_ntp.ntp_second_) * 1000) +
        (static_cast<double>(static_cast<uint64_t>(srs_ntp.ntp_fractions_) * 1000.0) / kMagicNtpFractionalUnit);
    return srs_ntp;
}



//haomiao
int64_t yang_get_milli_time() {
    timeval time;
    gettimeofday(&time, 0);
   // long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    return (time.tv_sec * 1000) + (time.tv_usec / 1000);


}
//weimiao
int64_t yang_get_micro_time() {
	timeval time;
	gettimeofday(&time, 0);
	return time.tv_sec * 1000*1000 + time.tv_usec ;
}


//haomiao
int64_t yang_get_milli_tick() {
 	struct timespec ts;

 	clock_gettime(CLOCK_MONOTONIC, &ts);
 	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);


}
//weimiao
int64_t yang_get_micro_tick() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000000 + ts.tv_nsec/1000);
}
//namiao
int64_t yang_get_nano_tick(){
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000000000 + ts.tv_nsec);
}




