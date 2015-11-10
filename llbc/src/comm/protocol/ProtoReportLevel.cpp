/**
 * @file    ProtoReportLevel.cpp
 * @author  Lonwei Lai<lailongwei@126.com>
 * @date    2014/11/27
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/protocol/ProtoReportLevel.h"

namespace
{
    typedef LLBC_NS LLBC_ProtoReportLevel This;

    LLBC_NS LLBC_String __descs[] = 
    {
        "Debug",
        "Info",
        "Warn",
        "Error",

        "Invalid"
    };
}

__LLBC_NS_BEGIN

bool LLBC_ProtoReportLevel::IsValid(int level)
{
    return (This::Begin <= level && level < This::End);
}

const LLBC_String & LLBC_ProtoReportLevel::Level2Str(int level)
{
    return __descs[This::IsValid(level) ? level : This::End];
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
