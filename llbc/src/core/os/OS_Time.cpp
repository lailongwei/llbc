// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "llbc/common/Export.h"

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
    time_t now = time(nullptr);
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
    if (UNLIKELY(clock_gettime(CLOCK_MONOTONIC, &ts) != 0))
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return 0;
    }

    return (static_cast<uint64>(ts.tv_sec)) * 1000 + ts.tv_nsec / (1000 * 1000);
#elif LLBC_TARGET_PLATFORM_MAC
    // Mac Impl.
    static mach_timebase_info_data_t timeBaseInfo;
    uint64_t absoluteTime = mach_absolute_time();

    if (timeBaseInfo.denom == 0)
    {
        (void)mach_timebase_info(&timeBaseInfo);
    }

    uint64_t nanoSec = absoluteTime * timeBaseInfo.numer / timeBaseInfo.denom;
    return nanoSec / (1000 * 1000);
#elif LLBC_TARGET_PLATFORM_IPHONE
    // Iphone Impl.
    struct timeval tv;
    gettimeofday(&tv, nullptr);
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
    return gettimeofday(tv, tz);
#else
    if (UNLIKELY(!tv))
    {
        errno = EINVAL;
        return LLBC_FAILED;
    }

    sint64 timeVal = LLBC_GetMicroSeconds();
    tv->tv_sec = (long)(timeVal / 1000000L);
    tv->tv_usec = (long)(timeVal % 1000000L);

    return LLBC_OK;
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
