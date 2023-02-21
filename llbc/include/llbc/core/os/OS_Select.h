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
 * Clear fd set.
 * @param[in] fd     - file descriptor.
 * @param[in] setPtr - file descriptor set.
 */
#define LLBC_ClrFd(fd, setPtr) FD_CLR(fd, setPtr)

/**
 * Check specific fd is in set.
 * @param[in] fd     - file descriptor.
 * @param[in] setPtr - file descriptor set.
 */
#define LLBC_FdIsSet(fd, setPtr) FD_ISSET(fd, setPtr)

/**
 * Set specific fd to set.
 * @param[in] fd     - file descriptor.
 * @param[in] setPtr - file descriptor set.
 */
#define LLBC_SetFd(fd, setPtr) FD_SET(fd, setPtr)

/**
 * Reset the file descriptor set.
 * @param[in] setPtr - file descriptor set.
 */
#define LLBC_ZeroFdSet(setPtr) FD_ZERO(setPtr)

/**
 * The select function determines the status of one or more sockets, warning if necessary, to perform synchronous I/O.
 * @param[in] nfds      - highest-numbered file descriptor in any of the three sets, plus 1, in WIN32 platform, ignored.
 * @param[in] readfds   - optional pointer to a set of sockets to be checked for readability.
 * @param[in] writefds  - optional pointer to a set of sockets to be checked for writability.
 * @param[in] exceptfds - optional pointer to a set of sockets to be checked for errors.
 * @param[in] interval  - maximum time for select to wait.
 * @return int - return the total number of socket handles that are ready and contained in the fd_set structures, zero if
 *               timeout, if error occurs, return -1.
 */
LLBC_EXPORT int LLBC_Select(int nfds, LLBC_FdSet *readfds, LLBC_FdSet *writefds, LLBC_FdSet *exceptfds, int interval);

__LLBC_NS_END
