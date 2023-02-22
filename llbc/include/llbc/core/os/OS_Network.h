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
 * This function privides protocol-independent translation from host name to address.
 * Note: For more about this function information, see MSDN or linux manual.
 * @param[in] nodename - Pointer to a nullptr-terminated string containing a host (node) name or a numeric host address string.
 *                       The numeric host address string is a dotted-decimal IPv4 address or an IPv6 hexadecimal address.
 * @param[in] servname - Pointer to a nullptr-terminated string containing either a service name or port number.
 * @param[in] hints    - Pointer to an addrinfo structure that provides hints about the type of socket the caller supports.
 * @param[out] res     - Pointer to a linked list of one or more addrinfo structures containing response information about the host.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_GetAddrInfo(const char *nodename,
                                 const char *servname,
                                 const struct addrinfo *hints,
                                 struct addrinfo **res);

/**
 * This function frees address information the the LLBC_GetAddrInfo function dynamically allocates its addrinfo structures.
 * Note: For more about this function information, see MSDN or linux manual.
 * @param[ai] - the addrinfo pointer.
 */
LLBC_EXPORT void LLBC_FreeAddrInfo(struct addrinfo *ai);

__LLBC_NS_END
