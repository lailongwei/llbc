/**
 * @file    Util_Network.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/12/19
 * @version 1.0
 *
 * @brief   LLBC library network about util APIs encapsulation.
 */
#ifndef __LLBC_CORE_UTILS_UTIL_NETWORK_H__
#define __LLBC_CORE_UTILS_UTIL_NETWORK_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Check given address format is IPv4 address format or not.
 * @param[in] addr - the address.
 * @return bool - return true if given address format string is IPv4 format, otherwise return false.
 */
LLBC_EXTERN LLBC_EXPORT bool LLBC_IsIPv4Addr(const LLBC_String &addr);

__LLBC_NS_END

#endif // !__LLBC_CORE_UTILS_UTIL_NETWORK_H__

