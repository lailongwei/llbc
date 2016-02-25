/**
 * @file    Directory_NonIphone.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/02/23
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/file/Directory.h"

#if LLBC_TARGET_PLATFORM_NON_IPHONE

__LLBC_NS_BEGIN

LLBC_String LLBC_Directory::DocDir()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return HomeDir();
#else // Win32
    CHAR buf[MAX_PATH];
    memset(buf, 0, sizeof(CHAR) * MAX_PATH);
    if (::SHGetSpecialFolderPathA(NULL, buf, CSIDL_COMMON_DOCUMENTS, FALSE) == FALSE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return "";
    }

    return buf;
#endif // Non-Win32
}

LLBC_String LLBC_Directory::HomeDir()
{
#if LLBC_TARGET_PLATFORM_WIN32
    size_t requiredSize = 0;
    if (getenv_s(&requiredSize, NULL, 0, "HOMEPATH") != 0)
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_String();
    }
    else if (requiredSize == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_String();
    }

    char *envVal = LLBC_Malloc(char, requiredSize);
    if (getenv_s(&requiredSize, envVal, requiredSize, "HOMEPATH") != 0)
    {
        LLBC_Free(envVal);
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_String();
    }

    LLBC_String path(envVal);
    LLBC_Free(envVal);
#else // Non-Win32
    char *envVal = getenv("HOME");
    if (!envVal)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return "";
    }
    const LLBC_String path(envVal);
#endif // Win32

#if LLBC_TARGET_PLATFORM_WIN32
    const size_t pathLen = path.length();
    if (path[pathLen - 1] == LLBC_SLASH_A ||
        path[pathLen - 1] == LLBC_BACKLASH_A)
    {
        if (pathLen == 3 &&
            LLBC_String::isalpha(path[0]) &&
            path[1] == ':')
            return path;
        else
            return path.substr(0, pathLen - 1);
    }
    else
    {
        return path;
    }
#else // Non-Win32
    const size_t pathLen = path.length();
    if (path[pathLen - 1] == LLBC_SLASH_A)
    {
        if (pathLen == 1)
            return path;
        else
            return path.substr(0, path.length() -1);
    }
    else
    {
        return path;
    }
#endif // Win32
}

LLBC_String LLBC_Directory::TempDir()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return "/tmp";
#else // Win32
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

    if (path[path.length() - 1] == LLBC_BACKLASH_A)
        return path.substr(0, path.length() - 1);
    
    return path;
#endif // Non-Win32
}

LLBC_String LLBC_Directory::CacheDir()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return "/tmp";
#else // Win32
    CHAR buf[MAX_PATH];
    memset(buf, 0, sizeof(CHAR) * MAX_PATH);
    if (::SHGetSpecialFolderPathA(NULL, buf, CSIDL_INTERNET_CACHE, FALSE) == FALSE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return "";
    }
 
    return buf;
#endif
}

__LLBC_NS_END

#endif // Non-iPhone
