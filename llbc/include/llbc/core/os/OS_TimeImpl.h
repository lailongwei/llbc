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
 * Number of micro-seconds/milli-seconds between the beginning of the
 * Windows epoch (Jan. 1, 1601) and the Unix epoch (Jan. 1, 1970).
 *
 * This assumes all Win32 compilers have 64-bit support.
 */
#if LLBC_TARGET_PLATFORM_WIN32
 #if defined(_MSC_VER) || defined(_MSC_EXTENSIONS) || defined(__WATCOMC__)
  #define LLBC_DELTA_EPOCH_IN_USEC  11644473600000000Ui64
  #define LLBC_DELTA_EPOCH_IN_MSEC  11644473600000Ui64
 #else
  #define LLBC_DELTA_EPOCH_IN_USEC  11644473600000000ULL
  #define LLBC_DELTA_EPOCH_IN_MSEC  11644473600000ULL
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

    return (LLBC_NS sint64)timeVal.tv_sec * 1000 + timeVal.tv_usec / 1000;
#else
    // Get time.
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);

    // Convert to unix time.
    sint64 time = ((LLBC_NS sint64)ft.dwHighDateTime) << 32;
    time |= ft.dwLowDateTime;
    time /= 10000;

    return time - LLBC_DELTA_EPOCH_IN_MSEC;
#endif
}

inline sint64 LLBC_GetMicroSeconds()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    struct timeval timeVal;
    gettimeofday(&timeVal, NULL);

    return (LLBC_NS sint64)timeVal.tv_sec * 1000000 + timeVal.tv_usec;
#else
    //Get time.
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);

    // Convert to unix time.
    sint64 time = ((LLBC_NS sint64)ft.dwHighDateTime) << 32;
    time |= ft.dwLowDateTime;
    time /= 10;

    return time - LLBC_DELTA_EPOCH_IN_USEC;
#endif
}

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
 #undef LLBC_DELTA_EPOCH_IN_MSEC
 #undef LLBC_DELTA_EPOCH_IN_USEC
#endif

#endif // __LLBC_CORE_OS_OS_TIME_H__

