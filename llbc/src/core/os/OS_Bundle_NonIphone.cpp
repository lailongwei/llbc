/**
 * @file    OS_Bundle_NonIphone.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/10/14
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_File.h"
#include "llbc/core/os/OS_Directory.h"

#include "llbc/core/os/OS_Bundle.h"

#if LLBC_TARGET_PLATFORM_NON_IPHONE

__LLBC_NS_BEGIN

LLBC_String LLBC_GetMainBundlePath()
{
    // We use current directory to return.
    return LLBC_GetCurrentDirectory(false);
}

LLBC_BundleHandle LLBC_CreateBundle(const LLBC_String &path)
{
    LLBC_String realPath = LLBC_GetMainBundlePath();
    if (UNLIKELY(realPath.empty()))
        return LLBC_INVALID_BUNDLE_HANDLE;

    // Main bundle-path + /(\\) + path.
    // Trim right(/(\\)).
    if (!path.empty())
    {
#if LLBC_TARGET_PLATFORM_NON_WIN32
        realPath.append(1, LLBC_SLASH_A);
#else
        realPath.append(1, LLBC_BACKLASH_A);
#endif

        realPath.append(path);

        const LLBC_String::size_type len = realPath.length();
        if (realPath[len - 1] == LLBC_SLASH_A || realPath[len - 1] == LLBC_BACKLASH_A)
            realPath.erase(len - 1, 1);
    }

    // Check path.
    if (!LLBC_DirectoryExist(realPath))
        return LLBC_INVALID_BUNDLE_HANDLE;

    return new LLBC_String(realPath);
}

void LLBC_ReleaseBundle(LLBC_BundleHandle bundle)
{
    if (LIKELY(bundle != LLBC_INVALID_BUNDLE_HANDLE))
        delete reinterpret_cast<LLBC_String *>(bundle);
}

LLBC_String LLBC_GetBundlePath(LLBC_BundleHandle bundle)
{
    bool needRelease = false;
    if (bundle == LLBC_INVALID_BUNDLE_HANDLE)
    {
        if ((bundle = LLBC_CreateBundle(
             LLBC_GetMainBundlePath())) == LLBC_INVALID_BUNDLE_HANDLE)
            return "";

        needRelease = true;
    }

    LLBC_String path = *(reinterpret_cast<LLBC_String *>(bundle));

    if (needRelease)
        LLBC_ReleaseBundle(bundle);

    return path;
}

LLBC_String LLBC_GetBundleResPath(LLBC_BundleHandle bundle, const LLBC_String &name)
{
    return LLBC_GetBundleResPath(bundle, name, "", "");
}

LLBC_String LLBC_GetBundleResPath(LLBC_BundleHandle bundle, const LLBC_String &name, const LLBC_String &ext)
{
    return LLBC_GetBundleResPath(bundle, name, ext, ""); 
}

LLBC_String LLBC_GetBundleResPath(LLBC_BundleHandle bundle, const LLBC_String &name, const LLBC_String &ext, const LLBC_String &inDir)
{
    if (UNLIKELY(name.empty()))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return "";
    }

    // Get bundle path.
    LLBC_String path = LLBC_GetBundlePath(bundle);
    if (UNLIKELY(path.empty()))
        return "";

    // Append intermediate directory.
    if (!inDir.empty())
    {
        if (inDir[0] != LLBC_SLASH_A && inDir[0] != LLBC_BACKLASH_A)
        {
#if LLBC_TARGET_PLATFORM_NON_WIN32
            path.append(1, LLBC_SLASH_A);
#else
            path.append(1, LLBC_BACKLASH_A);
#endif
        }

        path.append(inDir);

        if (inDir.size() > 1)
        {
            const LLBC_String::size_type endPos = inDir.length();
            if (inDir[endPos - 1] != LLBC_SLASH_A && inDir[endPos - 1] != LLBC_BACKLASH_A)
            {
#if LLBC_TARGET_PLATFORM_NON_WIN32
                path.append(1, LLBC_SLASH_A);
#else
                path.append(1, LLBC_BACKLASH_A);
#endif
            }
        }
    }
    else
    {
    // Append slash/backlash.
#if LLBC_TARGET_PLATFORM_NON_WIN32
        path.append(1, LLBC_SLASH_A);
#else
        path.append(1, LLBC_BACKLASH_A);
#endif
    }

    // Append file name.
    path.append(name);

    // Append extension.
    if (!ext.empty())
    {
        if (ext[0] != '.')
            path.append(1, '.');

        path.append(ext);
    }

    if (!LLBC_FileExist(path))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return "";
    }

    return path;
}

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_NON_IPHONE

#include "llbc/common/AfterIncl.h"
