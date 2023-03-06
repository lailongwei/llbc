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


#pragma once

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

inline int LLBC_GetMonthSpanDays(int year, int month)
{
    int spanDays = 0;
    for (int m = 1; m <= month; ++m)
        spanDays += LLBC_GetMonthMaxDays(year, m);

    return spanDays;
}

inline sint64 LLBC_GetMilliSeconds()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    struct timeval timeVal;
    gettimeofday(&timeVal, nullptr);

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
    gettimeofday(&timeVal, nullptr);

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

inline uint64 LLBC_GetCpuCounterFrequency()
{
#if LLBC_SUPPORT_RDTSC
    #if LLBC_TARGET_PLATFORM_WIN32
    LARGE_INTEGER freq;
    ::QueryPerformanceFrequency(&freq);
    return static_cast<uint64>(freq.QuadPart);
    #else
    const auto tscBegin = LLBC_RdTsc();
    usleep(200 * 1000);
    return (LLBC_RdTsc() - tscBegin) * 5;
    #endif // LLBC_TARGET_PLATFORM_WIN32
#else // Not supp rdtsc
    return LLBC_INFINITE;
#endif // Supp rdtsc
}

inline uint64 LLBC_RdTsc()
{
#if LLBC_SUPPORT_RDTSC
    #if LLBC_TARGET_PLATFORM_WIN32
    LARGE_INTEGER cur;
    ::QueryPerformanceCounter(&cur);
    return static_cast<uint64>(cur.QuadPart);
    #else // Non-win32
    uint32 lo = 0, hi = 0;
    __asm__ volatile ("lfence\n\t"
                    "rdtsc\n\t"
                    "mov %%edx, %1;"
                    "mov %%eax, %0;"
                    "lfence\n\t":"=r"(lo), "=r"(hi)
                    ::"%eax", "%ebx", "%ecx", "%edx");

    return (static_cast<uint64>(hi) << 32) | lo;
    #endif // LLBC_TARGET_PLATFORM_WIN32
#else
    return LLBC_GetMicroSeconds();
#endif
}

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
 #undef LLBC_DELTA_EPOCH_IN_MSEC
 #undef LLBC_DELTA_EPOCH_IN_USEC
#endif



