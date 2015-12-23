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

inline sint64 LLBC_GetMilliSeconds()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    struct timeval timeVal;
    gettimeofday(&timeval, NULL);

    return (LLBC_NS sint64)timeval.tv_sec * 1000 + timeval.tv_usec / 1000;
#else
    // Get time
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);

    // Convert to unix time
    sint64 time = ((LLBC_NS sint64)ft.dwHighDateTime) << 32;
    time |= ft.dwLowDateTime;
    time /= 10000;

    return time - LLBC_DELTA_EPOCH_IN_MSEC;
#endif
}

__LLBC_NS_END

#endif // __LLBC_CORE_OS_OS_TIME_H__