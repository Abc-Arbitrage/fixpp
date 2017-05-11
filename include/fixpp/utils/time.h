/* time.h
   Mathieu Stefani, 11 may 2017
   
  A collection of utilites for time management
*/

#pragma once

#include <ctime>

// converts a tm represented as UTC to a time_t in UTC

inline std::time_t mkgmtime(const struct std::tm* ptm)
{
    static constexpr auto SecondsPerMinute = 60;
    static constexpr auto SecondsPerHour = 3600;
    static constexpr auto SecondsPerDay = 86400;

    static constexpr int DaysOfMonth[12] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };

    auto isLeapYear = [](int year)
    {
        if (year % 4 != 0)
            return false;
        if (year % 100 != 0)
            return true;

        return (year % 400) == 0;
    };

    std::time_t secs = 0;
    int year = ptm->tm_year + 1900;
    for (int y = 1970; y < year; ++y)
        secs += (isLeapYear(y) ? 366 : 365) * SecondsPerDay;
    for (int m = 0; m < ptm->tm_mon; ++m)
    {
        secs += DaysOfMonth[m] * SecondsPerDay;
        if (m == 1 && isLeapYear(year))
            secs += SecondsPerDay;
    }

    secs += (ptm->tm_mday - 1) * SecondsPerDay;
    secs += ptm->tm_hour       * SecondsPerHour;
    secs += ptm->tm_min        * SecondsPerMinute;
    secs += ptm->tm_sec;
    return secs;
}

