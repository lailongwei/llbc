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

#if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID

/**
 * \brief The epoll event structure typedef.
 */
typedef struct epoll_event LLBC_EpollEvent;

/**
 * Open an epoll file descriptor.
 * @param[in] size - Scene linux 2.6.8, the size param is ignored, but must greater than 0.
 *                   In the initial epoll_create() implementation, the size argument
 *                   informed the kernel of the number of file descriptors that the caller
 *                   expected to add to the epoll instance.  The kernel used this
 *                   information as a hint for the amount of space to initially allocate
 *                   in internal data structures describing events.  (If necessary, the
 *                   kernel would allocate more space if the caller's usage exceeded the
 *                   hint given in size.)  Nowadays, this hint is no longer required (the
 *                   kernel dynamically sizes the required data structures without needing
 *                   the hint), but size must still be greater than zero, in order to
 *                   ensure backward compatibility when new epoll applications are run on
 *                   older kernels.
 * @return LLBC_Handle - epool file descriptor, if error occurred, return LLBC_INVALID_HANDLE.
 */
LLBC_EXPORT LLBC_Handle LLBC_EpollCreate(int size);

/**
 * Control interface for an epool descriptor.
 * @param[in] epfd  - epool handle.
 * @param[in] op    - operation, like EPOLL_CTL_ADD, EPOLL_CTL_MOD, EPOLL_CTL_DEL.
 * @param[in] fd    - target file descriptor.
 * @param[in] event - event data.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_EpollCtl(LLBC_Handle epfd, int op, int fd, LLBC_EpollEvent *event);

/**
 * Wait for an I/O event on an epoll file descriptor.
 * @param[in] epfd      - epoll handle.
 * @param[in] events    - storage location for epoll event.
 * @param[in] maxEvents - events count.
 * @param[in] timeout   - timeout value, in milliseconds.
 * @return int - When successful, return the number of file descriptors ready for the requested I/O,
 *               or zero if no file descriptor became ready during the requested timeout milliseconds.
 *               When an error occurs, the function return -1 and to call LLBC_GetLastError() to fetch error.
 */
LLBC_EXPORT int LLBC_EpollWait(LLBC_Handle epfd, LLBC_EpollEvent *events, int maxEvents, int timeout);

/**
 * Close the epoll handle.
 * @param[in] epfd - the epoll handle.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_EpollClose(LLBC_Handle epfd);

#endif // LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID

__LLBC_NS_END
