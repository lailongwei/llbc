/**
 * @file    OS_Network.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/27
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_OS_OS_NETWORK_H__
#define __LLBC_CORE_OS_OS_NETWORK_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * This function privides protocol-independent translation from host name to address.
 * Note: For more about this function information, see MSDN or linux manual.
 * @param[in] nodename - Pointer to a NULL-terminated string containing a host (node) name or a numeric host address string.
 *                       The numeric host address string is a dotted-decimal IPv4 address or an IPv6 hexadecimal address.
 * @param[in] servname - Pointer to a NULL-terminated string containing either a service name or port number.
 * @param[in] hints    - Pointer to an addrinfo structure that provides hints about the type of socket the caller supports.
 * @param[out] res     - Pointer to a linked list of one or more addrinfo structures containing response information about the host.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_GetAddrInfo(const char *nodename,
                                             const char *servname,
                                             const struct addrinfo *hints,
                                             struct addrinfo **res);

/**
 * This function frees address information the the LLBC_GetAddrInfo function dynamically allocates its addrinfo structures.
 * Note: For more about this function information, see MSDN or linux manual.
 * @param[ai] - the addrinfo pointer.
 */
LLBC_EXTERN LLBC_EXPORT void LLBC_FreeAddrInfo(struct addrinfo *ai);

__LLBC_NS_END

#endif // !__LLBC_CORE_OS_OS_NETWORK_H__

