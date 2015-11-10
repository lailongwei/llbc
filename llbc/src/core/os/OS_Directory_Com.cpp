/**
 * @file    OS_Directory_Com.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/10/14
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/os/OS_Directory.h"

#if LLBC_TARGET_PLATFORM_WIN32
 #pragma warning(disable:4996)
#endif

#ifndef PATH_MAX
 #define __LLBC_DIRECTORY_INTERNAL_DEFINE_PATH_MAX
 #define PATH_MAX 4096
#endif

__LLBC_NS_BEGIN

bool LLBC_DirectoryExist(const LLBC_String &dir)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    struct stat st;
    if (stat(dir.c_str(), &st) != 0)
#else
    struct _stat st;
    if (_stat(dir.c_str(), &st) != 0)
#endif
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return false;
    }

    bool exist = (st.st_mode & S_IFDIR) ? true : false;
    if (!exist)
    {
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    }
    else
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
    }

    return exist;
}

LLBC_String LLBC_JoinDirectory(const LLBC_String &path1,
                               const LLBC_String &path2)
{
    LLBC_String joined(path1);

    bool path1HasSlash = false;
    if (!path1.empty())
    {
        const char &lastCh = *path1.rbegin();
        if (lastCh == LLBC_SLASH_A ||
            lastCh == LLBC_BACKLASH_A)
            path1HasSlash = true;
    }

    bool path2HasSlash = false;
    if (!path2.empty())
    {
        const char &firstCh = *path2.begin();
        if (firstCh == LLBC_SLASH_A ||
            firstCh == LLBC_BACKLASH_A)
            path2HasSlash = true;
    }

    if (path1HasSlash)
        joined += path2HasSlash ? path2.substr(1) : path2;
    else
#if LLBC_TARGET_PLATFORM_NON_WIN32
        joined += path2HasSlash ? path2 : LLBC_String().format("%c%s", LLBC_SLASH_A, path2.c_str());
#else // WIN32
        joined += path2HasSlash ? path2 : LLBC_String().format("%c%s", LLBC_BACKLASH_A, path2.c_str());
#endif // Non-WIN32

    return joined;
}

LLBC_String LLBC_JoinDirectory(const LLBC_String &path1,
                               const LLBC_String &path2,
                               const LLBC_String &path3)
{
    return LLBC_JoinDirectory(LLBC_JoinDirectory(path1, path2), path3);
}

LLBC_String LLBC_JoinDirectory(const LLBC_String &path1,
                               const LLBC_String &path2,
                               const LLBC_String &path3,
                               const LLBC_String &path4)
{
    return LLBC_JoinDirectory(LLBC_JoinDirectory(path1, path2, path3), path4);
}

LLBC_String LLBC_JoinDirectory(const LLBC_String &path1,
                               const LLBC_String &path2,
                               const LLBC_String &path3,
                               const LLBC_String &path4,
                               const LLBC_String &path5)
{
    return LLBC_JoinDirectory(LLBC_JoinDirectory(path1, path2, path3, path4), path5);
}

LLBC_String LLBC_JoinDirectory(const LLBC_String &path1,
                               const LLBC_String &path2,
                               const LLBC_String &path3,
                               const LLBC_String &path4,
                               const LLBC_String &path5,
                               const LLBC_String &path6)
{
    return LLBC_JoinDirectory(LLBC_JoinDirectory(path1, path2, path3, path4, path5), path6);
}

LLBC_String LLBC_JoinDirectory(const LLBC_String &path1,
                               const LLBC_String &path2,
                               const LLBC_String &path3,
                               const LLBC_String &path4,
                               const LLBC_String &path5,
                               const LLBC_String &path6,
                               const LLBC_String &path7)
{
    return LLBC_JoinDirectory(LLBC_JoinDirectory(path1, path2, path3, path4, path5, path6), path7);
}

LLBC_String LLBC_ExpandDirectory(const LLBC_String &path, bool appendSlash)
{
    LLBC_String ret = path;
    if (path.empty())
    {
        if (UNLIKELY((ret = LLBC_GetCurrentDirectory()).empty()))
        {
            return ret;
        }
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (ret[0] != LLBC_SLASH_A)
    {
        LLBC_String cwd = LLBC_GetCurrentDirectory();
        if (UNLIKELY(cwd.empty()))
        {
            return cwd;
        }
        if (cwd[cwd.length() - 1] != LLBC_SLASH_A)
        {
            cwd += LLBC_SLASH_A;
        }

        ret.insert(0, cwd);
    }
#else // LLBC_TARGET_PLATFORM_WIN32
    if (ret.length() == 1 || 
        (ret[1] != ':' && ret[0] != LLBC_BACKLASH_A))
    {
        LLBC_String cwd = LLBC_GetCurrentDirectory();
        if (UNLIKELY(cwd.empty()))
        {
            return cwd;
        }

        if (cwd[cwd.length() - 1] != LLBC_BACKLASH_A)
        {
            cwd += LLBC_BACKLASH_A;
        }

        ret.insert(0, cwd);
    }
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    const LLBC_String::size_type len = ret.length();
#if LLBC_TARGET_PLATFORM_NON_WIN32
    if (appendSlash && ret[len - 1] != LLBC_SLASH_A)
    {
        ret.append(1, LLBC_SLASH_A);
    }
    else if (!appendSlash && ret[len - 1] == LLBC_SLASH_A)
    {
        ret.erase(len - 1, 1);
    }
#else // LLBC_TARGET_PLATFORM_WIN32
    if (appendSlash && (ret[len - 1] != LLBC_SLASH_A && ret[len - 1] != LLBC_BACKLASH_A))
    {
        ret.append(1, LLBC_BACKLASH_A);
    }
    else if (!appendSlash && (ret[len - 1] == LLBC_SLASH_A || ret[len - 1] == LLBC_BACKLASH_A))
    {
        ret.erase(len - 1, 1);
    }
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    return ret;
}

LLBC_String LLBC_GetModuleFileName()
{
	char buf[PATH_MAX];
#if LLBC_TARGET_PLATFORM_NON_WIN32
	ssize_t ret = -1;
	if ((ret = readlink("/proc/self/exe", buf, PATH_MAX - 1)) == -1)
	{
		LLBC_SetLastError(LLBC_ERROR_CLIB);
		return LLBC_String();
	}

	buf[ret] = '\0';

	return buf;
#else // LLBC_TARGET_PLATFORM_WIN32
	if (!::GetModuleFileNameA(NULL, buf, PATH_MAX))
	{
		LLBC_SetLastError(LLBC_ERROR_OSAPI);
		return LLBC_String();
	}

	return buf;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

LLBC_String LLBC_GetModuleFileDirectory(bool appendSlash)
{
	LLBC_String path = LLBC_DirName(LLBC_GetModuleFileName());
	if (appendSlash)
	{
#if LLBC_TARGET_PLATFORM_NON_WIN32
		path.append(1, LLBC_SLASH_A);
#else // LLBC_TARGET_PLATFORM_WIN32
		path.append(1, LLBC_BACKLASH_A);
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
	}

	return path;
}

LLBC_String LLBC_GetCurrentDirectory(bool appendSlash)
{
    LLBC_String path;
#if LLBC_TARGET_PLATFORM_NON_WIN32
    uint32 bufLen = 0;
#if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
    bufLen = PATH_MAX;
#else
    bufLen = MAXPATHLEN;
#endif
    
    char cwd[bufLen];
    if (!getcwd(cwd, bufLen))
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_String();
    }
    
    path = cwd;
    if (appendSlash)
    {
        path.append(1, LLBC_SLASH_A);
    }
#else
    LPSTR cwd = NULL;
    DWORD cwdSize = 0;
    cwdSize = ::GetCurrentDirectoryA(0, NULL);
    cwd = reinterpret_cast<LPSTR>(::malloc(cwdSize * sizeof(CHAR)));
    if (::GetCurrentDirectoryA(cwdSize, cwd) == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        ::free(cwd);
        return "";
    }
    
    path = cwd;
    if (appendSlash)
    {
        path.append(1, LLBC_BACKLASH_A);
    }
    
    ::free(cwd);
#endif
    
    return path;
}

int LLBC_SetCurrentDirectory(const LLBC_String &curDir)
{
	if (curDir.empty())
	{
		LLBC_SetLastError(LLBC_ERROR_ARG);
		return LLBC_RTN_FAILED;
	}

#if LLBC_TARGET_PLATFORM_NON_WIN32
	if (chdir(curDir.c_str()) == -1)
	{
		LLBC_SetLastError(LLBC_ERROR_CLIB);
		return LLBC_RTN_FAILED;
	}

	return LLBC_RTN_OK;
#else // LLBC_TARGET_PLATFORM_WIN32
	if (!::SetCurrentDirectoryA(curDir.c_str()) == 0)
	{
		LLBC_SetLastError(LLBC_ERROR_OSAPI);
		return LLBC_RTN_FAILED;
	}

	return LLBC_RTN_OK;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

__LLBC_NS_END

#ifdef __LLBC_DIRECTORY_INTERNAL_DEFINE_PATH_MAX
 #undef __LLBC_DIRECTORY_INTERNAL_DEFINE_PATH_MAX
 #undef PATH_MAX
#endif

#if LLBC_TARGET_PLATFORM_WIN32
 #pragma warning(default:4996)
#endif

#include "llbc/common/AfterIncl.h"
