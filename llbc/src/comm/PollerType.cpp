/**
 * @file    PollerType.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/11
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/PollerType.h"

namespace
{
    typedef LLBC_NS LLBC_PollerType This;
}

__LLBC_INTERNAL_NS_BEGIN

static const LLBC_NS LLBC_String __g_descs[] =
{
    "SelectPoller",
#if LLBC_TARGET_PLATFORM_WIN32
    "IocpPoller",
#elif LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
    "EpollPoller",
#endif // LLBC_TARGET_PLATFORM_WIN32

    "Invalid"
};

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

bool LLBC_PollerType::IsValid(int type)
{
    return (This::Begin <= type && type < This::End);
}

const LLBC_String &LLBC_PollerType::Type2Str(int type)
{
    return LLBC_INL_NS __g_descs[This::IsValid(type) ? type : This::End];
}

int LLBC_PollerType::Str2Type(const LLBC_String &typeStr)
{
    const LLBC_String &lowercased = typeStr.tolower();
    for (int ty = This::Begin; ty != This::End; ty++)
        if (lowercased == LLBC_INL_NS __g_descs[ty].tolower())
            return ty;

    return This::End;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
