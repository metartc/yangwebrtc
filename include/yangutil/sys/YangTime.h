#ifndef TIME_H__
#define TIME_H__

#include <sys/time.h>
#include <stdlib.h>
#include <iostream>

#include "TypeCast.h"
using namespace std;


class YangTime
{
    public:

         long long getTimeMilliseconds();//haomiao
         long long getTimeMicroseconds();//weimiao

        static long long getMilliTick();//haomiao
       static long long getMicroTick();//weimiao
        long long getNanoTick();//namiao
        /**
         * @brief returns the time and date formatted
         * (i.e Wed Oct 12 19:43:44 2011)
         * @return the string containing the formated date
         */
         string getTimeDate();

        /**
         * @brief returns the time and date formatted by the format string
         * @param format: the format (eg dd/MM/YYYY - HH:mm::SS).
         * You can use all formats you want the only restriction is to use
         * dd for the day
         * MM for the month
         * YYYY for the year
         * HH for the hour
         * mm for the minutes
         * SS for the seconds
         */
         string getTimeDate(string format);






};

#endif
