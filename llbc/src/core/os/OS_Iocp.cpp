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


#include "llbc/common/Export.h"

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
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_CloseIocp(LLBC_IocpHandle handle)
{
    if (::CloseHandle(handle) == FALSE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    return LLBC_OK;
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
            LLBC_SetLastError(LLBC_ERROR_TIMEOUTED);
        }
        else
        {
            DWORD flags = 0;
            DWORD bytesTransfer = 0;
#if LLBC_DEBUG
            BOOL grRet =
#endif
            ::WSAGetOverlappedResult((*ol)->sock,
                                     *ol,
                                     &bytesTransfer,
                                     TRUE,
                                     &flags);
#if LLBC_DEBUG
            ASSERT(grRet == FALSE && "llbc library internal error, in LLBC_GetQueuedCompletionStatus()!");
#endif
            LLBC_SetLastError(LLBC_ERROR_NETAPI);
        }

        return LLBC_FAILED;
    }

    return LLBC_OK;
}

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_WIN32
