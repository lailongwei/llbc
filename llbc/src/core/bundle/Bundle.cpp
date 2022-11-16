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

#include "llbc/core/os/OS_Bundle.h"
#include "llbc/core/file/Directory.h"

#include "llbc/core/bundle/Bundle.h"

__LLBC_INTERNAL_NS_BEGIN

static LLBC_NS LLBC_Bundle *__g_mainBundle = nullptr;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_Bundle::LLBC_Bundle()
: _bundle(LLBC_INVALID_BUNDLE_HANDLE)

, _bundleName()
, _bundlePath()
{
}

LLBC_Bundle::~LLBC_Bundle()
{
    Finalize();
}

int LLBC_Bundle::CreateMainBundle()
{
    if (LLBC_INTERNAL_NS __g_mainBundle)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }
    
    LLBC_INTERNAL_NS __g_mainBundle = new LLBC_Bundle;
    return (LLBC_INTERNAL_NS __g_mainBundle)->Initialize("");
}

void LLBC_Bundle::DestroyMainBundle()
{
    LLBC_XDelete(LLBC_INTERNAL_NS __g_mainBundle);
}

const LLBC_Bundle *LLBC_Bundle::GetMainBundle()
{
    return LLBC_INTERNAL_NS __g_mainBundle;
}

int LLBC_Bundle::Initialize(const LLBC_String &path)
{
    if (_bundle != LLBC_INVALID_BUNDLE_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    if ((_bundle = LLBC_CreateBundle(path)) == LLBC_INVALID_BUNDLE_HANDLE)
        return LLBC_FAILED;

    _bundlePath = LLBC_GetBundlePath(_bundle);
    _bundleName = LLBC_Directory::BaseName(_bundlePath);

    return LLBC_OK;
}

void LLBC_Bundle::Finalize()
{
    if (_bundle != LLBC_INVALID_BUNDLE_HANDLE)
    {
        LLBC_ReleaseBundle(_bundle);
        _bundle = LLBC_INVALID_BUNDLE_HANDLE;

        _bundleName.clear();
        _bundlePath.clear();
    }
}

LLBC_BundleHandle LLBC_Bundle::GetHandle() const
{
    return _bundle;
}

const LLBC_String &LLBC_Bundle::GetBundleName() const
{
    return _bundleName;
}

const LLBC_String &LLBC_Bundle::GetBundlePath() const
{
    return _bundlePath;
}

LLBC_String LLBC_Bundle::GetResPath(const LLBC_String &name) const
{
    if (_bundle == LLBC_INVALID_BUNDLE_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return "";
    }

    return LLBC_GetBundleResPath(_bundle, name);
}

LLBC_String LLBC_Bundle::GetResPath(const LLBC_String &name, const LLBC_String &ext) const
{
    if (_bundle == LLBC_INVALID_BUNDLE_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return "";
    }

    return LLBC_GetBundleResPath(_bundle, name, ext);
}

LLBC_String LLBC_Bundle::GetResPath(const LLBC_String &name, const LLBC_String &ext, const LLBC_String &inDir) const
{
    if (_bundle == LLBC_INVALID_BUNDLE_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return "";
    }
    
    return LLBC_GetBundleResPath(_bundle, name, ext, inDir);
}

__LLBC_NS_END
