/**
 * @file    LogLevel.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/09
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/utils/Util_Text.h"
#include "llbc/core/log/LogLevel.h"

__LLBC_INTERNAL_NS_BEGIN

static const LLBC_NS LLBC_String __level2StrDesc[LLBC_NS LLBC_LogLevel::End + 1] =
{
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",

    "UNKNOWN"
};

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

const LLBC_String &LLBC_LogLevel::GetDebugDesc()
{
    return LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Debug];
}

const LLBC_String &LLBC_LogLevel::GetInfoDesc()
{
    return LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Info];
}

const LLBC_String &LLBC_LogLevel::GetWarnDesc()
{
    return LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Warn];
}

const LLBC_String &LLBC_LogLevel::GetErrorDesc()
{
    return LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Error];
}

const LLBC_String &LLBC_LogLevel::GetFatalDesc()
{
    return LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Fatal];
}

const LLBC_String &LLBC_LogLevel::GetLevelDesc(int level)
{
    return ((level >= LLBC_LogLevel::Begin && level < LLBC_LogLevel::End) ?
        LLBC_INTERNAL_NS __level2StrDesc[level] : LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::End]);
}

int LLBC_LogLevel::Str2Level(const char *level)
{
    if (UNLIKELY(!level))
    {
        return LLBC_LogLevel::End;
    }

    LLBC_String upperStr = LLBC_ToUpper(level);
    if (upperStr == LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Debug])
    {
        return LLBC_LogLevel::Debug;
    }
    else if (upperStr == LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Info])
    {
        return LLBC_LogLevel::Info;
    }
    else if (upperStr == LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Warn])
    {
        return LLBC_LogLevel::Warn;
    }
    else if (upperStr == LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Error])
    {
        return LLBC_LogLevel::Error;
    }
    else if (upperStr == LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Fatal])
    {
        return LLBC_LogLevel::Fatal;
    }

    return LLBC_LogLevel::End;
}

bool LLBC_LogLevel::IsLegal(int level)
{
    return (LLBC_LogLevel::Begin <= level && level < LLBC_LogLevel::End);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
