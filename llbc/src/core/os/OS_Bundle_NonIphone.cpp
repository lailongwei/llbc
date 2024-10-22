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

#include "llbc/core/file/File.h"
#include "llbc/core/file/Directory.h"

#include "llbc/core/os/OS_Bundle.h"

#if LLBC_TARGET_PLATFORM_NON_IPHONE

__LLBC_NS_BEGIN

LLBC_String LLBC_GetMainBundlePath()
{
    // We use current directory to return.
    return LLBC_Directory::CurDir();
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
    if (!LLBC_Directory::Exists(realPath))
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

LLBC_String LLBC_GetBundleResPath(LLBC_BundleHandle bundle,
                                  const LLBC_String &name)
{
    return LLBC_GetBundleResPath(bundle, name, "", "");
}

LLBC_String LLBC_GetBundleResPath(LLBC_BundleHandle bundle,
                                  const LLBC_String &name,
                                  const LLBC_String &ext)
{
    return LLBC_GetBundleResPath(bundle, name, ext, "");
}

LLBC_String LLBC_GetBundleResPath(LLBC_BundleHandle bundle,
                                  const LLBC_String &name,
                                  const LLBC_String &ext,
                                  const LLBC_String &inDir)
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

    if (!LLBC_File::Exists(path))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return "";
    }

    return path;
}

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_NON_IPHONE
