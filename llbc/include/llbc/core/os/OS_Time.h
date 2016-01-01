/**
 * @file    OS_Time.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/05
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_OS_OS_TIME_H__
#define __LLBC_CORE_OS_OS_TIME_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Set timezone.
 */
LLBC_EXTERN LLBC_EXPORT void LLBC_TZSet();

/**
 * Get timezone value, the value is difference in seconds between coordinated universal 
 * time and local time, if in +8, return -28800, if in -8, return 28800.
 * @return int - timezone value.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_GetTimezone();

/**
 * Makesure given year is leap year or not.
 * @param[in] year - the given year.
 * @return bool - reutrn true if is leap year, otherwise return false.
 */
LLBC_EXTERN bool LLBC_IsLeapYear(int year);

/**
 * Get specific month max days.
 * @param[in] year  - the year.
 * @param[in] month - the month - [1, 12].
 * @return int - the specific month max days, if failed, return 0.
 */
LLBC_EXTERN int LLBC_GetMonthMaxDays(int year, int month);

/**
 * Get the current time.
 * @return sint64 - current time, in milli-seconds since midnight(0:0:0), 
 *                  January 1, 1970, coordinated universal time(UTC).
 */
LLBC_EXTERN sint64 LLBC_GetMilliSeconds();

/**
 * Get the current time as micro-seconds.
 * @return sint64 - current time, in micro-seconds since midnight(0:0:0),
 *                  January 1, 1970, corrdinated universal time(UTC).
 */
LLBC_EXTERN sint64 LLBC_GetMicroSeconds();

/**
 * Retrieves the number of milliseconds that have elapsed since the system was started, up to 49.7 days.
 * @return uint64 - the number of milliseconds that have elapsed since the system was started.
 */
LLBC_EXTERN LLBC_EXPORT uint64 LLBC_GetTickCount();

/**
 * Get struct timeval format time.
 * @param[in] tv - time value.
 * @param[in] tz - timezone value[WIN32 platform, not use].
 * @return int - return 0 if success, otherwise return -1.
 */
#if LLBC_TARGET_PLATFORM_NON_WIN32
 LLBC_EXTERN LLBC_EXPORT int LLBC_GetTimeOfDay(struct timeval *tv, struct timezone *tz);
#else
 LLBC_EXTERN LLBC_EXPORT int LLBC_GetTimeOfDay(struct timeval *tv, void *tz);
#endif

__LLBC_NS_END

/**
 * Get struct timeval format time[compatible for LINUX].
 * @param[in] tv - time value.
 * @param[in] tz - timezone value[WIN32 platform, not use].
 * @return int - return 0 if success, otherwise return -1.
 */
#if LLBC_TARGET_PLATFORM_WIN32
 #if LLBC_CFG_OS_IMPL_GETTIMEOFDAY
  LLBC_EXTERN LLBC_EXPORT int gettimeofday(struct timeval *tv, void *tz);
 #endif // LLBC_CFG_OS_IMPL_GETTIMEOFDAY
#endif // LLBC_TARGET_PLATFORM_WIN32

#include "llbc/core/os/OS_TimeImpl.h"

#endif // !__LLBC_CORE_OS_OS_TIME_H__
