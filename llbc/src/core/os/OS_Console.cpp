/**
 * @file    OS_Console.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/05
 * @version 1.0
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/file/File.h"
#if LLBC_TARGET_PLATFORM_WIN32
#include "llbc/core/thread/FastLock.h"
#endif // LLBC_TARGET_PLATFORM_WIN32
#include "llbc/core/os/OS_Console.h"

#if LLBC_TARGET_PLATFORM_WIN32
__LLBC_INTERNAL_NS_BEGIN
static LLBC_NS LLBC_FastLock __g_consoleLock[2];
__LLBC_INTERNAL_NS_END
#endif // LLBC_TARGET_PLATFORM_WIN32

__LLBC_NS_BEGIN

int LLBC_GetConsoleColor(FILE *file)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
#else
    const int fileNo = LLBC_File::GetFileNo(file);
    if (UNLIKELY(fileNo == -1))
    {
        return LLBC_FAILED;
    }
    else if (UNLIKELY(fileNo != 1 && fileNo != 2))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    HANDLE handle = (fileNo == 1 ? 
        ::GetStdHandle(STD_OUTPUT_HANDLE) : GetStdHandle(STD_ERROR_HANDLE));

    CONSOLE_SCREEN_BUFFER_INFO info;
    ::GetConsoleScreenBufferInfo(handle, &info);

    return info.wAttributes;
#endif
}

int LLBC_SetConsoleColor(FILE *file, int color)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
#else
    const int fileNo = LLBC_File::GetFileNo(file);
    if (UNLIKELY(fileNo == -1))
    {
        return LLBC_FAILED;
    }
    else if (UNLIKELY(fileNo != 1 && fileNo != 2))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    HANDLE handle = (fileNo == 1 ? 
        ::GetStdHandle(STD_OUTPUT_HANDLE) : GetStdHandle(STD_ERROR_HANDLE));

    if (::SetConsoleTextAttribute(handle, color) == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }
    
    return LLBC_OK;
#endif
}

int __LLBC_FilePrint(bool newline, FILE *file, const char *fmt, ...)
{
    const int fileNo = LLBC_File::GetFileNo(file);
    if (UNLIKELY(fileNo == -1))
    {
        return LLBC_FAILED;
    }

    char *buf; int len;
    LLBC_FormatArg(fmt, buf, len);

#if LLBC_TARGET_PLATFORM_NON_WIN32
    flockfile(file);
    fprintf(file, (newline?"%s\n":"%s"), buf);
    fflush(file);
    funlockfile(file);
#else
    LLBC_INTERNAL_NS __g_consoleLock[(fileNo == 1 || fileNo == 2 ? 0 : 1)].Lock();
    fprintf(file, newline?"%s\n":"%s", buf);
    fflush(file);
    LLBC_INTERNAL_NS __g_consoleLock[(fileNo == 1 || fileNo == 2 ? 0 : 1)].Unlock();
#endif

    LLBC_Free(buf);

    return LLBC_OK;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
