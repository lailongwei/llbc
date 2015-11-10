/**
 * @file    OS_Iocp.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/17
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_OS_OS_IOCP_H__
#define __LLBC_CORE_OS_OS_IOCP_H__

#include "llbc/common/Common.h"

#if LLBC_TARGET_PLATFORM_WIN32

__LLBC_NS_BEGIN

/**
 * Create I/O completion port.
 * @param[in] numOfConcurrentThreads - the number of concurrent threads, default is 0.
 * @return LLBC_IocpHandle - IOCP handle.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_IocpHandle LLBC_CreateIocp();

/**
 * Add socket to iocp.
 * @param[in] handle        - IOCP handle.
 * @param[in] sock          - socket handle.
 * @param[in] completionKey - completion key.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_AddSocketToIocp(LLBC_IocpHandle handle, 
                                                 LLBC_SocketHandle sock, 
                                                 void *completionKey);

/**
 * Close IOCP handle.
 * @param[in] handle - IOCP handle.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_CloseIocp(LLBC_IocpHandle handle);

/**
 * Attempts to dequeue an I/O completion packet from a specified I/O completion port.
 * @param[in] handle        - IOCP handle.
 * @param[out] numOfBytes   - number of bytes transferred during an I/O operation that has completed.
 * @param[out] compKey      - completion key.
 * @param[out] ol           - overlapped structure pointer.
 * @param[in]  milliSeconds - wait time.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_GetQueuedCompletionStatus(LLBC_IocpHandle handle,
                                                           void *numOfBytes,
                                                           void **compKey,
                                                           LLBC_POverlapped *ol,
                                                           ulong milliSeconds);

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_WIN32

#endif // !__LLBC_CORE_OS_OS_IOCP_H__
