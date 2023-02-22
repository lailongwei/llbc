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

#if LLBC_TARGET_PLATFORM_WIN32

__LLBC_NS_BEGIN

/**
 * Create I/O completion port.
 * @param[in] numOfConcurrentThreads - the number of concurrent threads, default is 0.
 * @return LLBC_IocpHandle - IOCP handle.
 */
LLBC_EXPORT LLBC_IocpHandle LLBC_CreateIocp();

/**
 * Add socket to iocp.
 * @param[in] handle        - IOCP handle.
 * @param[in] sock          - socket handle.
 * @param[in] completionKey - completion key.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_AddSocketToIocp(LLBC_IocpHandle handle, 
                                                 LLBC_SocketHandle sock, 
                                                 void *completionKey);

/**
 * Close IOCP handle.
 * @param[in] handle - IOCP handle.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_CloseIocp(LLBC_IocpHandle handle);

/**
 * Attempts to dequeue an I/O completion packet from a specified I/O completion port.
 * @param[in] handle        - IOCP handle.
 * @param[out] numOfBytes   - number of bytes transferred during an I/O operation that has completed.
 * @param[out] compKey      - completion key.
 * @param[out] ol           - overlapped structure pointer.
 * @param[in]  milliSeconds - wait time.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_GetQueuedCompletionStatus(LLBC_IocpHandle handle,
                                               void *numOfBytes,
                                               void **compKey,
                                               LLBC_POverlapped *ol,
                                               ulong milliSeconds);

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_WIN32


