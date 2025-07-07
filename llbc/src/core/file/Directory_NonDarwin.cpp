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

#include "llbc/core/file/Directory.h"

#if LLBC_TARGET_PLATFORM_IOS == 0 && LLBC_TARGET_PLATFORM_MAC == 0

__LLBC_NS_BEGIN

LLBC_String LLBC_Directory::ModuleFilePath(bool readLink)
{
#if LLBC_TARGET_PLATFORM_WIN32
    // Mark <readLink> as unused.
    LLBC_UNUSED_PARAM(readLink);

    // Call WIN32 API, get module file path.
    DWORD ret;
    LLBC_String modFilePath;
    // modFilePath.resize(LLBC_PATH_MAX + 1);
    modFilePath.resize(1);
    while ((ret = ::GetModuleFileNameA(nullptr,
                                       const_cast<char *>(modFilePath.c_str()),
                                       static_cast<DWORD>(modFilePath.size()))) == modFilePath.size())
        modFilePath.resize(modFilePath.size() + LLBC_PATH_MAX);
    if (UNLIKELY(ret == 0))
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI); 
        return "";
    }

    modFilePath.resize(ret);

    // Return.
    return modFilePath;
#else // LLBC_TARGET_PLATFORM_MAC == 0 && LLBC_TARGET_PLATFORM_IPHONE == 0
    ssize_t pathLen;
    auto &pathBuf = __LLBC_GetLibTls()->commonTls.pathBuf;
    if (readLink)
    {
        // Read link.
        if ((pathLen = readlink("/proc/self/exe", pathBuf, sizeof(pathBuf))) == -1)
        {
            LLBC_SetLastError(LLBC_ERROR_CLIB);
            return "";
        }

        // Convert to LLBC_String.
        return LLBC_String(pathBuf, pathLen);
    }
    else
    {
        // Open cmdline file.
        FILE *f = fopen("/proc/self/cmdline", "rb");
        if (UNLIKELY(!f))
        {
            LLBC_SetLastError(LLBC_ERROR_CLIB);
            return "";
        }

        // Read first argument.
        int ch;
        pathLen = 0;
        while ((ch = fgetc(f)) != EOF)
        {
            LLBC_BreakIf(ch == '\0');
            pathBuf[pathLen++] = static_cast<char>(ch);
            if (UNLIKELY(pathLen == sizeof(pathBuf)))
            {
                LLBC_SetLastError(LLBC_ERROR_LIMIT);
                fclose(f);
                return "";
            }
        }

        // Process I/O error.
        if (UNLIKELY(ch == EOF && ferror(f)))
        {
            LLBC_SetLastError(LLBC_ERROR_CLIB);
            fclose(f);
            return "";
        }

        // Close cmdline file.
        fclose(f);

        // Process unknown error.
        if (UNLIKELY(pathLen == 0))
        {
            LLBC_SetLastError(LLBC_ERROR_UNKNOWN);
            return "";
        }

        return AbsPath(LLBC_String(pathBuf, pathLen));
    }
#endif // Win32
}

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_IOS == 0 && LLBC_TARGET_PLATFORM_MAC == 0


#if LLBC_TARGET_PLATFORM_NON_IPHONE

__LLBC_NS_BEGIN

LLBC_String LLBC_Directory::DocDir()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return HomeDir();
#else // Win32
    auto &pathBuf = __LLBC_GetLibTls()->commonTls.pathBuf;
    memset(pathBuf, 0, sizeof(pathBuf));
    if (::SHGetSpecialFolderPathA(nullptr, pathBuf, CSIDL_COMMON_DOCUMENTS, FALSE) == FALSE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return "";
    }

    return pathBuf;
#endif // Non-Win32
}

LLBC_String LLBC_Directory::HomeDir()
{
#if LLBC_TARGET_PLATFORM_WIN32
    size_t requiredSize = 0;
    if (getenv_s(&requiredSize, nullptr, 0, "HOMEPATH") != 0)
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
        free(envVal);
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_String();
    }

    LLBC_String path(envVal);
    free(envVal);
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
    if (path[pathLen - 1] == '/' || path[pathLen - 1] == '\\')
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
    if (path[pathLen - 1] == '/')
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
    const DWORD bufLen = ::GetTempPathA(0, nullptr) + 1;
    if (UNLIKELY(bufLen == 1))
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return "";
    }

    LPSTR buf = reinterpret_cast<LPSTR>(malloc(bufLen));
    if (::GetTempPathA(bufLen, buf) == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        free(buf);
        return "";
    }

    LLBC_String path = buf;
    free(buf);

    if (path[path.length() - 1] == '\\')
        return path.substr(0, path.length() - 1);
    
    return path;
#endif // Non-Win32
}

LLBC_String LLBC_Directory::CacheDir()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    return "/tmp";
#else // Win32
    auto &pathBuf = __LLBC_GetLibTls()->commonTls.pathBuf;
    memset(pathBuf, 0, sizeof(pathBuf));
    if (::SHGetSpecialFolderPathA(nullptr, pathBuf, CSIDL_INTERNET_CACHE, FALSE) == FALSE)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return "";
    }
 
    return pathBuf;
#endif
}

__LLBC_NS_END

#endif // Non-iPhone
