/**
 * @file    OS_File.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/10/14
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_File.h"

__LLBC_NS_BEGIN

int LLBC_GetFileNo(FILE *file)
{
    if (UNLIKELY(!file))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_RTN_FAILED;
    }

#if LLBC_TARGET_PLATFORM_WIN32
    const int fd = ::_fileno(file);
#else // Non-Win32
    const int fd = ::fileno(file);
#endif // LLBC_TARGET_PLATFORM_WIN32
    if (UNLIKELY(fd == -1))
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_RTN_FAILED;
    }

    return fd;
}

bool LLBC_FileExist(const LLBC_String &file)
{
    if (file.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return false;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (access(file.c_str(), 0) != 0)
#else
    if (_access(file.c_str(), 0) != 0)
#endif
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return false;
    }

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    return true;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
