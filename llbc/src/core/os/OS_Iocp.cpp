/**
 * @file    OS_Iocp.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/10/02
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Console.h"
#include "llbc/core/utils/Util_Debug.h"

#include "llbc/core/os/OS_Iocp.h"

#if LLBC_TARGET_PLATFORM_WIN32

__LLBC_NS_BEGIN

LLBC_IocpHandle LLBC_CreateIocp()
{
    LLBC_IocpHandle handle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE,
                                                      LLBC_INVALID_IOCP_HANDLE,
                                                      0,
                                                      0);
    if (handle == LLBC_INVALID_IOCP_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
    }

    return handle;
}

int LLBC_AddSocketToIocp(LLBC_IocpHandle handle,
                         LLBC_SocketHandle sock,
                         void *completionKey)
{
    LLBC_IocpHandle ret = ::CreateIoCompletionPort((HANDLE)sock,
                                                   handle, 
                                                   (ULONG_PTR)completionKey, 
                                                   0);
    if (ret == LLBC_INVALID_IOCP_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        trace("LLBC_AddSocketToIocp() failed, reason: %s\n", LLBC_FormatLastError());
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
}

int LLBC_CloseIocp(LLBC_IocpHandle handle)
{
    if (::CloseHandle(handle) == FALSE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
}

int LLBC_GetQueuedCompletionStatus(LLBC_IocpHandle handle,
                                   void *numOfBytes,
                                   void **compKey,
                                   LLBC_POverlapped *ol,
                                   ulong milliSeconds)
{
    BOOL ret = ::GetQueuedCompletionStatus(handle,
                                           (LPDWORD)numOfBytes,
                                           (PULONG_PTR)compKey,
                                           (LPOVERLAPPED *)ol,
                                           milliSeconds);
    if (ret == FALSE)
    {
        if (::GetLastError() == WAIT_TIMEOUT)
        {
            LLBC_SetLastError(LLBC_ERROR_TIMEOUT);
        }
        else
        {
            LLBC_SetLastError(LLBC_ERROR_OSAPI);
        }

        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
}

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_WIN32

#include "llbc/common/AfterIncl.h"
