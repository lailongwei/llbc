/**
 * @file    GUIDHelper.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/20
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/helper/GUIDHelper.h"

__LLBC_NS_BEGIN

LLBC_GUID LLBC_GUIDHelper::Gen()
{
    LLBC_GUID guid;
    ::memset(&guid, 0, sizeof(LLBC_GUID));

#if LLBC_TARGET_PLATFORM_NON_WIN32
    uuid_generate(reinterpret_cast<unsigned char *>(&guid));
#else
    ::CoCreateGuid(&guid);
#endif

    return guid;
}

LLBC_String LLBC_GUIDHelper::Format(LLBC_GUIDCRef guid)
{
    LLBC_String str;
    str.format("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", 
            guid.Data1,
            guid.Data2,
            guid.Data3,
            guid.Data4[0],
            guid.Data4[1],
            guid.Data4[2],
            guid.Data4[3],
            guid.Data4[4],
            guid.Data4[5],
            guid.Data4[6],
            guid.Data4[7]);

    return str;
}

LLBC_String LLBC_GUIDHelper::GenStr()
{
    return Format(Gen());
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
