/**
 * @file    OS_Network.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/27
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Network.h"

__LLBC_NS_BEGIN

int LLBC_GetAddrInfo(const char *nodename,
                     const char *servname,
                     const struct addrinfo *hints,
                     struct addrinfo **res)
{
    int ret = ::getaddrinfo(nodename, servname, hints, res);
    if (ret != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_GAI);
        LLBC_SetSubErrorNo(ret);

        return LLBC_FAILED;
    }

    return LLBC_OK;
}

void LLBC_FreeAddrInfo(struct addrinfo *ai)
{
    ::freeaddrinfo(ai);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
