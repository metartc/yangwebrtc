
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangTime.h>
#include <yangutil/sys/YangLog.h>

#ifdef _MSC_VER
#include <windows.h>
int gettimeofday(struct timeval *tp, void *tzp)
{
  time_t clock;
  struct tm tm;
  SYSTEMTIME wtm;
  GetLocalTime(&wtm);
  tm.tm_year   = wtm.wYear - 1900;
  tm.tm_mon   = wtm.wMonth - 1;
  tm.tm_mday   = wtm.wDay;
  tm.tm_hour   = wtm.wHour;
  tm.tm_min   = wtm.wMinute;
  tm.tm_sec   = wtm.wSecond;
  tm. tm_isdst  = -1;
  clock = mktime(&tm);
  tp->tv_sec = clock;
  tp->tv_usec = wtm.wMilliseconds * 1000;
  return (0);
}

#endif


int64_t yang_get_system_time(){
    return yang_get_micro_time();
}
int64_t  yang_update_system_time(){
    return yang_get_micro_time();
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

#ifdef _MSC_VER
//weimiao
int64_t yang_get_micro_time() {
    //LARGE_INTEGER  large_interger;
    //QueryPerformanceFrequency(&large_interger);
    return GetTickCount()*1000;
}
//haomiao
int64_t yang_get_milli_tick() {
    return GetTickCount();
}


#else
//haomiao
int64_t yang_get_milli_time() {
    timeval time;
    gettimeofday(&time, 0);
   // long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    return (time.tv_sec * 1000) + (time.tv_usec / 1000);


}
//haomiao
int64_t yang_get_milli_tick() {
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);


}
//weimiao
int64_t yang_get_micro_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000000 + ts.tv_nsec/1000);
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
#endif


