/**
 * @file    Bundle.mm
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/25
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"

#include "llbc/core/os/OS_Bundle.h"
#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/bundle/Bundle.h"

#if LLBC_TARGET_PLATFORM_IPHONE
#import <Foundation/Foundation.h>

__LLBC_INTERNAL_NS_BEGIN

static LLBC_NS LLBC_Bundle *__g_mainBundle = NULL;

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
    this->Finalize();
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

    if((_bundle = LLBC_CreateBundle(path)) == LLBC_INVALID_BUNDLE_HANDLE)
        return LLBC_FAILED;

    _bundlePath = LLBC_GetBundlePath(_bundle);
    _bundleName = LLBC_BaseName(_bundlePath, true);

    return LLBC_OK;
}

void LLBC_Bundle::Finalize()
{
    if(_bundle != LLBC_INVALID_BUNDLE_HANDLE)
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
    if(_bundle == LLBC_INVALID_BUNDLE_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return "";
    }

    return LLBC_GetBundleResPath(_bundle, name);
}

LLBC_String LLBC_Bundle::GetResPath(const LLBC_String &name, const LLBC_String &ext) const
{
    if(_bundle == LLBC_INVALID_BUNDLE_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return "";
    }

    return LLBC_GetBundleResPath(_bundle, name, ext);
}

LLBC_String LLBC_Bundle::GetResPath(const LLBC_String &name, const LLBC_String &ext, const LLBC_String &inDir) const
{
    if(_bundle == LLBC_INVALID_BUNDLE_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return "";
    }
    
    return LLBC_GetBundleResPath(_bundle, name, ext, inDir);
}

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_IPHONE
