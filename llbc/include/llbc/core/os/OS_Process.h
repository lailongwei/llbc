/**
 * @file    OS_Process.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/03/23
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_OS_OS_PROCESS_H__
#define __LLBC_CORE_OS_OS_PROCESS_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Get current process Id.
 * @return int - the current process Id.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_GetCurrentProcessId();

__LLBC_NS_END

#endif // !__LLBC_CORE_OS_OS_PROCESS_H__
