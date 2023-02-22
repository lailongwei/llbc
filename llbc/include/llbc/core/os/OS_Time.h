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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * The LLBC_RdTsc() support macro define.
 */
#if (LLBC_TARGET_PROCESSOR_X86_64 || LLBC_TARGET_PROCESSOR_X86)
 #define LLBC_SUPPORT_RDTSC 1
#else // Non x86 and x86_64
 #define LLBC_SUPPORT_RDTSC 0
#endif // x86 or x86_64

/**
 * Set timezone.
 */
LLBC_EXPORT void LLBC_TZSet();

/**
 * Get timezone value, the value is difference in seconds between coordinated universal 
 * time and local time, if in +8, return -28800, if in -8, return 28800.
 * @return int - timezone value.
 */
LLBC_EXPORT int LLBC_GetTimezone();

/**
 * Makesure given year is leap year or not.
 * @param[in] year - the given year.
 * @return bool - return true if is leap year, otherwise return false.
 */
LLBC_EXPORT bool LLBC_IsLeapYear(int year);

/**
 * Get specific month max days.
 * @param[in] year  - the year.
 * @param[in] month - the month - [1, 12].
 * @return int - the specific month max days, if failed, return 0.
 */
LLBC_EXPORT int LLBC_GetMonthMaxDays(int year, int month);

/**
 * Get specific month span days.
 * @param[in] year  - the year.
 * @param[in] month - the month - [1, 12].
 * @return int - the month span days(not included giving month).
 */
int LLBC_GetMonthSpanDays(int year, int month);

/**
 * Get the current time.
 * @return sint64 - current time, in milli-seconds since midnight(0:0:0), 
 *                  January 1, 1970, coordinated universal time(UTC).
 */
sint64 LLBC_GetMilliSeconds();

/**
 * Get the current time as micro-seconds.
 * @return sint64 - current time, in micro-seconds since midnight(0:0:0),
 *                  January 1, 1970, coordinated universal time(UTC).
 */
sint64 LLBC_GetMicroSeconds();

/**
 * Retrieves the number of milliseconds that have elapsed since the system was started, up to 49.7 days.
 * @return uint64 - the number of milliseconds that have elapsed since the system was started.
 */
LLBC_EXPORT uint64 LLBC_GetTickCount();

/**
 * Get struct timeval format time.
 * @param[in] tv - time value.
 * @param[in] tz - timezone value[WIN32 platform, not use].
 * @return int - return 0 if success, otherwise return -1.
 */
#if LLBC_TARGET_PLATFORM_NON_WIN32
 LLBC_EXPORT int LLBC_GetTimeOfDay(struct timeval *tv, struct timezone *tz);
#else
 LLBC_EXPORT int LLBC_GetTimeOfDay(struct timeval *tv, void *tz);
#endif

# if LLBC_TARGET_PLATFORM_WIN32
 /**
  * Convert filetime to timespec format.
  * @param[in] fileTime - the file time.
  * @param[out] ts      - the converted timespec structure.
  */
void LLBC_WinFileTime2TimeSpec(const FILETIME &fileTime, timespec &ts);

/**
 * Convert filetime to milliseconds.
 * @param[in] fileTime - the filetime.
 * @return sint64 - the converted time, in milliseconds.
 */
sint64 LLBC_WinFileTime2MilliSeconds(const FILETIME &fileTime);

/**
 * Convert filetime to microseconds.
 * @param[in] fileTime - the filetime.
 * @return sint64 - the converted time, in microseconds.
 */
sint64 LLBC_WinFileTime2MicroSeconds(const FILETIME &fileTime);

/**
 * Convert timespec to filetime.
 * @param[in] ts        - the timespec.
 * @param[out] fileTime - the filetime.
 */
void LLBC_WinTimeSpec2FileTime(const timespec &ts, FILETIME &fileTime);

/**
 * Convert milliseconds to filetime.
 * @param[in] milliSeconds - the milliseconds.
 * @param[out] fileTime    - the filetime.
 */
void LLBC_WinMilliSeconds2FileTime(sint64 milliSeconds, FILETIME &fileTime);

/**
 * Convert microseconds to filetime.
 * @param[in] microSeconds - the microseconds.
 * @param[out] fileTime    - the filetime.
 */
void LLBC_WinMicroSeconds2FileTime(sint64 microSeconds, FILETIME &fileTime);
#endif // LLBC_TARGET_PLATFORM_WIN32

/**
 * Get CpuCounter Frequency.
 * @return uint64 - cpucounter frequency(ticks per second).
 */
uint64 LLBC_GetCpuCounterFrequency();

/**
 * Get cpu timestamp counter value.
 * @return uint64 - current tsc ticks num.
 */
uint64 LLBC_RdTsc();

__LLBC_NS_END

/**
 * Get struct timeval format time[compatible for LINUX].
 * @param[in] tv - time value.
 * @param[in] tz - timezone value[WIN32 platform, not use].
 * @return int - return 0 if success, otherwise return -1.
 */
#if LLBC_TARGET_PLATFORM_WIN32
 #if LLBC_CFG_OS_IMPL_GETTIMEOFDAY
  LLBC_EXPORT int gettimeofday(struct timeval *tv, void *tz);
 #endif // LLBC_CFG_OS_IMPL_GETTIMEOFDAY
#endif // LLBC_TARGET_PLATFORM_WIN32

#include "llbc/core/os/OS_TimeInl.h"


