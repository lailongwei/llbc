/**
 * @file    RTTI.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2018/02/09
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/common/LibTlsInl.h"
#include "llbc/common/RTTI.h"

__LLBC_NS_BEGIN

#if LLBC_TARGET_PLATFORM_NON_WIN32

LLBC_String __LLBC_CxxDemangle(const char *name)
{
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    if (UNLIKELY(!libTls))
        return LLBC_String();

    int status = 0;
    size_t length = sizeof(libTls->commonTls.rtti);

    abi::__cxa_demangle(name, libTls->commonTls.rtti, &length, &status);
    if (status != 0)
        return LLBC_String();

    return libTls->commonTls.rtti;
}

#endif // Non-Win32

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
