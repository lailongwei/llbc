/**
 * @file    Bundle.mm
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/25
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Bundle.h"
#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/bundle/Bundle.h"

#if LLBC_TARGET_PLATFORM_IPHONE

__LLBC_INTERNAL_NS_BEGIN

static LLBC_NS LLBC_Bundle *__g_mainBundle = NULL;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_Bundle::LLBC_Bundle()
: m_bundle(LLBC_INVALID_BUNDLE_HANDLE)

, m_bundleName()
, m_bundlePath()
{
}

LLBC_Bundle::~LLBC_Bundle()
{
    this->Finalize();
}

int LLBC_Bundle::CreateMainBundle()
{
    if(LLBC_INTERNAL_NS __g_mainBundle)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_RTN_FAILED;
    }
    
    LLBC_INTERNAL_NS __g_mainBundle = new LLBC_Bundle;
    return (LLBC_INTERNAL_NS __g_mainBundle)->Initialize("");
}

void LLBC_Bundle::DestroyMainBundle()
{
    SAFE_DELETE(LLBC_INTERNAL_NS __g_mainBundle);
}

const LLBC_Bundle *LLBC_Bundle::GetMainBundle()
{
    return LLBC_INTERNAL_NS __g_mainBundle;
}

int LLBC_Bundle::Initialize(const LLBC_String &path)
{
    
    if(m_bundle != LLBC_INVALID_BUNDLE_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_RTN_FAILED;
    }

    if( ( m_bundle = LLBC_CreateBundle(path) ) == LLBC_INVALID_BUNDLE_HANDLE )
    {
        return LLBC_RTN_FAILED;
    }

    m_bundlePath = LLBC_GetBundlePath(m_bundle);
    m_bundleName = LLBC_BaseName(m_bundlePath, true);

    return LLBC_RTN_OK;
}

void LLBC_Bundle::Finalize()
{
    if(m_bundle != LLBC_INVALID_BUNDLE_HANDLE)
    {
        LLBC_ReleaseBundle(m_bundle);
        m_bundle = LLBC_INVALID_BUNDLE_HANDLE;

        m_bundleName.clear();
        m_bundlePath.clear();
    }
}

LLBC_BundleHandle LLBC_Bundle::GetHandle() const
{
    return m_bundle;
}

const LLBC_String &LLBC_Bundle::GetBundleName() const
{
    return m_bundleName;
}

const LLBC_String &LLBC_Bundle::GetBundlePath() const
{
    return m_bundlePath;
}

LLBC_String LLBC_Bundle::GetResPath(const LLBC_String &name) const
{
    if(m_bundle == LLBC_INVALID_BUNDLE_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return "";
    }

    return LLBC_GetBundleResPath(m_bundle, name);
}

LLBC_String LLBC_Bundle::GetResPath(const LLBC_String &name, const LLBC_String &ext) const
{
    if(m_bundle == LLBC_INVALID_BUNDLE_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return "";
    }

    return LLBC_GetBundleResPath(m_bundle, name, ext);
}

LLBC_String LLBC_Bundle::GetResPath(const LLBC_String &name, const LLBC_String &ext, const LLBC_String &inDir) const
{
    if(m_bundle == LLBC_INVALID_BUNDLE_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return "";
    }
    
    return LLBC_GetBundleResPath(m_bundle, name, ext, inDir);
}

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_IPHONE

#include "llbc/common/AfterIncl.h"
