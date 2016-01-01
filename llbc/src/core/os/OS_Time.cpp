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

void LLBC_TZSet()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    tzset();
 #if defined(_SVID_SOURCE) || defined(_XOPEN_SOURCE)
    LLBC_INTERNAL_NS __g_timezone = static_cast<int>(timezone);
 #else
    time_t now = time(NULL);
    LLBC_INTERNAL_NS __g_timezone = 
        static_cast<int>(localtime(&now)->tm_gmtoff);
 #endif // defined _SVID_SOURCE || _XOPEN_SOURCE
#else // WIN32
    _tzset();
    LLBC_INTERNAL_NS __g_timezone = _timezone;
#endif // Non-WIN32
}

int LLBC_GetTimezone()
{
    return LLBC_INTERNAL_NS __g_timezone;
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
    if (UNLIKELY(!tv))
    {
        errno = EINVAL;
        return LLBC_RTN_FAILED;
    }

    sint64 timeVal = LLBC_GetMicroSeconds();
    tv->tv_sec = (long)(timeVal / 1000000L);
    tv->tv_usec = (long)(timeVal % 1000000L);

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
