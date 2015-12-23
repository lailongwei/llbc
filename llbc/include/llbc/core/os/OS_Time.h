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
 * Get local time by calendar time value, thread safety.
 * @param[in] time - calendar time.
 * @param[out] tm  - time structure reference.
 */
LLBC_EXTERN LLBC_EXPORT void LLBC_LocalTime(time_t time, struct LLBC_TimeStruct &tm);

/**
 * Get UTC time by calendar time value, thread safety.
 * @param[in] time - calendar time.
 * @param[out] tm  - time structure reference.
 */
LLBC_EXTERN LLBC_EXPORT void LLBC_GMTime(time_t time, struct LLBC_TimeStruct &tm);

/**
 * Convert tm structure value to calendar time.
 * @param[in] tm - time structure.
 * @return time_t - calendar time.
 */
LLBC_EXTERN LLBC_EXPORT time_t LLBC_MkTime(const struct LLBC_TimeStruct &tm);

/**
 * Get the current time.
 * @return sint64 - current time, in seconds since midnight(0:0:0), 
 *                  January 1, 1970, coordinated universal time(UTC).
 */
LLBC_EXTERN sint64 LLBC_GetMilliSeconds();

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
