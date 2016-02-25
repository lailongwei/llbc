/**
 * @file    OS_TimeImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/12/13
 * @version 1.0
 *
 * @brief
 */

#ifdef __LLBC_CORE_OS_OS_TIME_H__

__LLBC_NS_BEGIN

/*
 * Number of milli-seconds/micro-seconds/100-nano seconds between the beginning of the
 * Windows epoch (Jan. 1, 1601) and the Unix epoch (Jan. 1, 1970).
 *
 * This assumes all Win32 compilers have 64-bit support.
 */
#if LLBC_TARGET_PLATFORM_WIN32
 #if defined(_MSC_VER) || defined(_MSC_EXTENSIONS) || defined(__WATCOMC__)
  #define LLBC_DELTA_EPOCH_IN_MSEC    11644473600000Ui64
  #define LLBC_DELTA_EPOCH_IN_USEC    11644473600000000Ui64
  #define LLBC_DELTA_EPOCH_IN_100NSEC 116444736000000000Ui64
 #else
  #define LLBC_DELTA_EPOCH_IN_MSEC    11644473600000ULL
  #define LLBC_DELTA_EPOCH_IN_USEC    11644473600000000ULL
  #define LLBC_DELTA_EPOCH_IN_100NSEC 116444736000000000ULL
 #endif
#endif // LLBC_PLATFORM_WIN32

inline bool LLBC_IsLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

inline int LLBC_GetMonthMaxDays(int year, int month)
{
    if (month >= 1 && month <= 7)
    {
        if (month % 2 == 1)
            return 31;
        else if (month == 2)
            return LLBC_IsLeapYear(year) ? 29 : 28;
        else
            return 30;
    }
    else if (month >= 8 && month <= 12)
    {
        return month % 2 == 0 ? 31 : 30;
    }
    else
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return 0;
    }
}

inline sint64 LLBC_GetMilliSeconds()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    struct timeval timeVal;
    gettimeofday(&timeVal, NULL);

    return (sint64)timeVal.tv_sec * 1000 + timeVal.tv_usec / 1000;
#else // LLBC_TARGET_PLATFORM_WIN32
    // Get time.
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);

    // Convert to unix time.
    return LLBC_WinFileTime2MilliSeconds(ft);
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

inline sint64 LLBC_GetMicroSeconds()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    struct timeval timeVal;
    gettimeofday(&timeVal, NULL);

    return (sint64)timeVal.tv_sec * 1000000 + timeVal.tv_usec;
#else // LLBC_TARGET_PLATFORM_WIN32
    //Get time.
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);

    // Convert to unix time.
    return LLBC_WinFileTime2MicroSeconds(ft);
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

#if LLBC_TARGET_PLATFORM_WIN32
inline void LLBC_WinFileTime2TimeSpec(const FILETIME &fileTime, timespec &ts)
{
    sint64 time = ((sint64)fileTime.dwHighDateTime) << 32;
    time |= fileTime.dwLowDateTime;

    time -= LLBC_DELTA_EPOCH_IN_100NSEC;
    ts.tv_sec = static_cast<time_t>(time / 10000000);
    ts.tv_nsec = static_cast<long>((time % 10000000 * 100));
}

inline sint64 LLBC_WinFileTime2MilliSeconds(const FILETIME &fileTime)
{
    sint64 timeInMilliSec = ((sint64)fileTime.dwHighDateTime) << 32;
    timeInMilliSec |= fileTime.dwLowDateTime;
    timeInMilliSec /= 10000;

    return timeInMilliSec - LLBC_DELTA_EPOCH_IN_MSEC;
}

inline sint64 LLBC_WinFileTime2MicroSeconds(const FILETIME &fileTime)
{
    sint64 timeInMicroSec = ((sint64)fileTime.dwHighDateTime) << 32;
    timeInMicroSec |= fileTime.dwLowDateTime;
    timeInMicroSec /= 10;

    return timeInMicroSec - LLBC_DELTA_EPOCH_IN_USEC;
}

inline void LLBC_WinTimeSpec2FileTime(const timespec &ts, FILETIME &fileTime)
{
    sint64 ticks = ((sint64)ts.tv_sec * 10000000 + 
        ts.tv_nsec / 100) + LLBC_DELTA_EPOCH_IN_100NSEC;
    fileTime.dwHighDateTime = (DWORD)(ticks >> 32 & 0xffffffff);
    fileTime.dwLowDateTime = (DWORD)(ticks & 0xffffffff);
}

inline void LLBC_WinMilliSeconds2FileTime(sint64 milliSeconds, FILETIME &fileTime)
{
    sint64 ticks = milliSeconds * 10000 + LLBC_DELTA_EPOCH_IN_100NSEC;
    fileTime.dwHighDateTime = (DWORD)(ticks >> 32 & 0xffffffff);
    fileTime.dwLowDateTime = (DWORD)(ticks & 0xffffffff);
}

inline void LLBC_WinMicroSeconds2FileTime(sint64 microSeconds, FILETIME &fileTime)
{
    sint64 ticks = microSeconds * 10 + LLBC_DELTA_EPOCH_IN_100NSEC;
    fileTime.dwHighDateTime = (DWORD)(ticks >> 32 & 0xffffffff);
    fileTime.dwLowDateTime = (DWORD)(ticks & 0xffffffff);
}
#endif // LLBC_TARGET_PLATFORM_WIN32

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
 #undef LLBC_DELTA_EPOCH_IN_MSEC
 #undef LLBC_DELTA_EPOCH_IN_USEC
#endif

#endif // __LLBC_CORE_OS_OS_TIME_H__

