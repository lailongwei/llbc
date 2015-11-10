/**
 * @file    OS_Directory.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/24
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Directory.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

#if LLBC_TARGET_PLATFORM_NON_IPHONE

__LLBC_NS_BEGIN

LLBC_String LLBC_GetHomeDirectory(bool appendSlash)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    char *valStr = getenv("HOME");
#else
    char *valStr = getenv("HOMEPATH");
#endif
    if (!valStr)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return "";
    }
    
    LLBC_String path = valStr;
    if (appendSlash)
    {
#if LLBC_TARGET_PLATFORM_NON_WIN32
    path.append(1, LLBC_SLASH_A);
#else
    path.append(1, LLBC_BACKLASH_A);
#endif
    }

    return path;
}

LLBC_String LLBC_GetDocumentDirectory(bool appendSlash)
{
 #if LLBC_TARGET_PLATFORM_NON_WIN32
    return LLBC_GetHomeDirectory(appendSlash);
 #else
    CHAR buf[MAX_PATH];
    memset(buf, 0, sizeof(CHAR) * MAX_PATH);
    if (::SHGetSpecialFolderPathA(NULL, buf, CSIDL_COMMON_DOCUMENTS, FALSE) == FALSE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return "";
    }

    LLBC_String path = buf;
    if (appendSlash)
    {
        path.append(1, LLBC_BACKLASH_A);
    }
    
    return path;
 #endif
}

LLBC_String LLBC_GetTemporaryDirectory(bool appendSlash)
{
 #if LLBC_TARGET_PLATFORM_NON_WIN32
    if (appendSlash)
    {
        return "/tmp/";
    }
    else
    {
        return "/tmp";
    }
 #else
    DWORD bufLen = 0;
    bufLen = ::GetTempPathA(0, NULL);
    bufLen += 1;

    LPSTR buf = reinterpret_cast<LPSTR>(::malloc(sizeof(CHAR) * bufLen));
    if (::GetTempPathA(bufLen, buf) == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        ::free(buf);
        return "";
    }

    LLBC_String path = buf;
    ::free(buf);
    
    if (!appendSlash)
    {
        path = path.substr(0, path.size() - 1);
    }
    
    return path;
 #endif
}

LLBC_String LLBC_GetCacheDirectory(bool appendSlash)
{
 #if LLBC_TARGET_PLATFORM_NON_WIN32
    if (appendSlash)
    {
        return "/tmp/";
    }
    else
    {
        return "/tmp";
    }
 #else
    CHAR buf[MAX_PATH];
    memset(buf, 0, sizeof(CHAR) * MAX_PATH);
    if (::SHGetSpecialFolderPathA(NULL, buf, CSIDL_INTERNET_CACHE, FALSE) == FALSE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return "";
    }
    
    LLBC_String path = buf;
    if (appendSlash)
    {
        path.append(1, LLBC_BACKLASH_A);
    }

    return path;
 #endif
}

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4996)
#endif

#endif // LLBC_TARGET_PLATFORM_NON_IPHONE

#include "llbc/common/AfterIncl.h"
