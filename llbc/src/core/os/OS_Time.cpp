/**
 * @file    OS_Time.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/05
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Time.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

__LLBC_INTERNAL_NS_BEGIN
static int __g_timezone;
__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

/*
 * Number of micro-seconds between the beginning of the Windows epoch
 * (Jan. 1, 1601) and the Unix epoch (Jan. 1, 1970).
 *
 * This assumes all Win32 compilers have 64-bit support.
 */
#if LLBC_TARGET_PLATFORM_WIN32
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS) || defined(__WATCOMC__)
  #define DELTA_EPOCH_IN_USEC  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_USEC  11644473600000000ULL
#endif
#endif // LLBC_PLATFORM_WIN32

void LLBC_TZSet()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    tzset();
    time_t now = time(NULL);
    LLBC_INTERNAL_NS __g_timezone = - static_cast<int>(localtime(&now)->tm_gmtoff);
#else
    _tzset();
    LLBC_INTERNAL_NS __g_timezone = _timezone;
#endif
}

int LLBC_GetTimezone()
{
    return LLBC_INTERNAL_NS __g_timezone;
}

void LLBC_LocalTime(time_t time, struct LLBC_TimeStruct &tm)
{
    static const unsigned char month_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static const bool leap_year[4] = {false, false, true, false};

    uint32 temp_value = 0;
    uint16 four_year_count = 0;
    uint32 leave_for_fouryear = 0;
    uint32 leave_for_year_days = 0;
    uint32 leave_for_month_days = 0;

    ::memset(&tm, 0, sizeof(struct LLBC_TimeStruct));

    tm.tm_sec = time % 60;
    temp_value = static_cast<uint32>(time / 60);
    tm.tm_min = temp_value % 60;
    temp_value /= 60;

    temp_value += (uint32)(- LLBC_GetTimezone() / 3600);
    tm.tm_gmtoff = - LLBC_GetTimezone();
    tm.tm_hour = temp_value % 24;
    temp_value /= 24;

    tm.tm_wday = (temp_value + 4) % 7; // 1970-01-01 is thursday

    four_year_count = temp_value / (365 * 4 + 1);
    leave_for_fouryear = temp_value % (365 * 4 + 1);
    leave_for_year_days = leave_for_fouryear;

    unsigned int day_count = 0;
    int i = 0;

    for(i = 0; i < 4; i ++)
    {
        day_count = leap_year[i] ? 366 : 365;
        if (leave_for_year_days < day_count)
        {
            break;
        }
        else
        {
            leave_for_year_days -= day_count;
        }
    }

    tm.tm_year = four_year_count * 4 + i + 1970 - 1900;
    tm.tm_yday = leave_for_year_days;

    leave_for_month_days = leave_for_year_days;
    int j = 0;
    for(j = 0; j < 12; j ++)
    {
        if (leap_year[i] && j == 1)
        {
            if (leave_for_month_days < 29)
            {
                break;
            }
            else if (leave_for_month_days == 29)
            {
                j ++;
                leave_for_month_days = 0;
                break;
            }
            else
            {
                leave_for_month_days -= 29;
            }

            continue;
        }

        if (leave_for_month_days < month_days[j])
        {
            break;
        }
        else if (leave_for_month_days == month_days[j])
        {
            j ++;
            leave_for_month_days = 0;
            break;
        }
        else
        {
            leave_for_month_days -= month_days[j];
        }
    }

    tm.tm_mday = leave_for_month_days + 1;
    tm.tm_mon = j;

    tm.tm_isdst = 0;

    return;
}

void LLBC_GMTime(time_t time, struct LLBC_TimeStruct &tm)
{
    static const unsigned char month_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static const bool leap_year[4] = {false, false, true, false};

    unsigned int temp_value = 0;
    unsigned short four_year_count = 0;
    unsigned int leave_for_fouryear = 0;
    unsigned int leave_for_year_days = 0;
    unsigned int leave_for_month_days = 0;

    ::memset(&tm, 0, sizeof(struct LLBC_TimeStruct));

    tm.tm_sec = time % 60;
    temp_value = static_cast<uint32>(time / 60);
    tm.tm_min = temp_value % 60;
    temp_value /= 60;

    tm.tm_hour = temp_value % 24;
    tm.tm_gmtoff = 0;
    temp_value /= 24;

    tm.tm_wday = (temp_value + 4) % 7;	// 1970-01-01 is thursday

    four_year_count = temp_value / (365 * 4 + 1);
    leave_for_fouryear = temp_value % (365 * 4 + 1);
    leave_for_year_days = leave_for_fouryear;

    unsigned int day_count = 0;
    int i = 0;

    for(i = 0; i < 4; i ++)
    {
        day_count = leap_year[i] ? 366 : 365;
        if (leave_for_year_days < day_count)
        {
            break;
        }
        else
        {
            leave_for_year_days -= day_count;
        }
    }

    tm.tm_year = four_year_count * 4 + i + 1970 - 1900;
    tm.tm_yday = leave_for_year_days;

    leave_for_month_days = leave_for_year_days;

    int j = 0;
    for(j = 0; j < 12; j ++)
    {
        if (leap_year[i] && j == 1)
        {
            if (leave_for_month_days < 29)
            {
                break;
            }
            else if (leave_for_month_days == 29)
            {
                j ++;
                leave_for_month_days = 0;
                break;
            }
            else
            {
                leave_for_month_days -= 29;
            }

            continue;
        }

        if (leave_for_month_days < month_days[j])
        {
            break;
        }
        else if (leave_for_month_days == month_days[j])
        {
            j ++;
            leave_for_month_days = 0;
            break;
        }
        else
        {
            leave_for_month_days -= month_days[j];
        }
    }

    tm.tm_mday = leave_for_month_days + 1;
    tm.tm_mon = j;

    tm.tm_isdst = 0;

    return;
}

time_t LLBC_MkTime(const struct LLBC_TimeStruct &tm)
{
    static const time_t secInHour = 3600;
    static const time_t secInDay = 24 * 3600;
    static const bool leapYearTable[4] = {false, false, true, false};

    int i = 0;
    time_t retTime = 0;

    retTime = tm.tm_sec;
    retTime += (tm.tm_min * 60);
    retTime += (tm.tm_hour * secInHour);
    //  retTime += (tm.tm_yday - 1) * secInDay;
    retTime += tm.tm_yday * secInDay;

    for(i = 1970; i < (tm.tm_year + 1900); i += 4)
    {
        retTime += (secInDay * (365 * 4 + 1));
    }

    if (i > tm.tm_year + 1900)
    {
        for(i = i - 1; i >=  (tm.tm_year + 1900); i --)
        {
            retTime -= (secInDay * 365);
            if (leapYearTable[(i - 1970) % 4])
            {
                retTime -= secInDay;
            }
        }
    }

    retTime -= tm.tm_gmtoff;

    return retTime;
}

sint64 LLBC_GetMilliSeconds()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    struct timeb tb;
    ftime(&tb);
#else
    struct ::_timeb tb;
    _ftime(&tb);
#endif

    return (static_cast<sint64>(tb.time)) * 1000 + tb.millitm;
}

uint64 LLBC_GetTickCount()
{
#if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
    // LINUX & ANDROID Impl.
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (static_cast<uint64>(ts.tv_sec)) * 1000 + ts.tv_nsec / (1000 * 1000);
#elif LLBC_TARGET_PLATFORM_MAC
    // Mac Impl.
    static mach_timebase_info_data_t timeBaseInfo;
    uint64_t absoluteTime = ::mach_absolute_time();

    if (timeBaseInfo.denom == 0)
    {
        (void)mach_timebase_info(&timeBaseInfo);
    }

    uint64_t nanoSec = absoluteTime * timeBaseInfo.numer / timeBaseInfo.denom;
    return nanoSec / (1000 * 1000);
#elif LLBC_TARGET_PLATFORM_IPHONE
    // Iphone Impl.
    struct timeval tv;
    ::gettimeofday(&tv, NULL);
    return static_cast<uint64>(tv.tv_sec) * 1000 + static_cast<uint64>(tv.tv_usec / 1000);
#elif LLBC_TARGET_PLATFORM_WIN32
    // WIN32 Impl.
 #if WINVER >= 0x600
    return ::GetTickCount64();
 #else
    return ::GetTickCount();
 #endif
#endif
}

#if LLBC_TARGET_PLATFORM_NON_WIN32
int LLBC_GetTimeOfDay(struct timeval *tv, struct timezone *tz)
#else
int LLBC_GetTimeOfDay(struct timeval *tv, void *tz)
#endif
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return ::gettimeofday(tv, tz);
#else
    FILETIME ft;
    LLBC_NS uint64 time = 0L;

    if (!tv) {
        errno = EINVAL;
        return LLBC_RTN_FAILED;
    }

    // get time
    ::GetSystemTimeAsFileTime(&ft);

    // convert to unix time
    time = (LLBC_NS uint64)(ft.dwHighDateTime) << 32;
    time |= ft.dwLowDateTime;
    time /= 10;
    time -= DELTA_EPOCH_IN_USEC;

    // convert to timeval struct
    tv->tv_sec = (long)(time / 1000000L);
    tv->tv_usec = (long)(time % 1000000L);

    // success
    return LLBC_RTN_OK;
#endif
}

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
 #if LLBC_CFG_OS_IMPL_GETTIMEOFDAY

int gettimeofday(struct timeval *tv, void *tz)
{
    return LLBC_NS LLBC_GetTimeOfDay(tv, tz);
}

 #endif // LLBC_CFG_OS_IMPL_GETTIMEOFDAY
#endif // LLBC_TARGET_PLATFORM_WIN32

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4996)
#endif

#include "llbc/common/AfterIncl.h"
