/**
 * @file    OS_Select.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/17
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Select.h"

__LLBC_NS_BEGIN

int LLBC_Select(int nfds, LLBC_FdSet *readfds, LLBC_FdSet *writefds, LLBC_FdSet *exceptfds, int interval)
{
    struct timeval timeout;
    timeout.tv_sec = interval / 1000;
    timeout.tv_usec = (interval % 1000) * 1000;

    int ret = ::select(nfds, readfds, writefds, exceptfds, &timeout);
    if (ret == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_TIMEOUT);
    }
#if LLBC_TARGET_PLATFORM_NON_WIN32
    else if (ret == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return ret;
    }
#else
    else if (ret == SOCKET_ERROR)
    {
        LLBC_SetLastError(LLBC_ERROR_NETAPI);
    }
#endif

    return ret;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
