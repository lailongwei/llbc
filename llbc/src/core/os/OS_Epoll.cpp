/**
 * @file    OS_Epoll.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/17
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Epoll.h"

__LLBC_NS_BEGIN

#if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID

LLBC_Handle LLBC_EpollCreate(int size)
{
    LLBC_Handle handle = ::epoll_create(size);
    if (handle == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_INVALID_HANDLE;
    }

    return handle;
}

int LLBC_EpollCtl(LLBC_Handle epfd, int op, int fd, LLBC_EpollEvent *event)
{
    if (::epoll_ctl(epfd, op, fd, event) == -1)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_EpollWait(LLBC_Handle epfd, LLBC_EpollEvent *events, int maxEvents, int timeout)
{
    int ret = 0;
    while ((ret = epoll_wait(epfd, events, maxEvents, timeout)) < 0 && errno == EINTR);

    if (ret < 0)
        LLBC_SetLastError(LLBC_ERROR_CLIB);

    return ret;
}

int LLBC_EpollClose(LLBC_Handle epfd)
{
    if (::close(epfd) != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

#endif // LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
