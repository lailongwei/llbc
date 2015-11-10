/**
 * @file    BaseLogToken.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/10
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/log/LogFormattingInfo.h"
#include "llbc/core/log/BaseLogToken.h"

__LLBC_NS_BEGIN

LLBC_BaseLogToken::LLBC_BaseLogToken()
: _formatter(NULL)
, _nextToken(NULL)
{
}

LLBC_BaseLogToken::~LLBC_BaseLogToken()
{
    LLBC_XDelete(_formatter);
}

LLBC_LogFormattingInfo *LLBC_BaseLogToken::GetFormatter() const
{
    return _formatter;
}

void LLBC_BaseLogToken::SetFormatter(LLBC_LogFormattingInfo *formatter)
{
    LLBC_XDelete(_formatter);
    _formatter = formatter;
}

LLBC_ILogToken *LLBC_BaseLogToken::GetTokenNext() const
{
    return _nextToken;
}

void LLBC_BaseLogToken::SetTokenNext(LLBC_ILogToken *next)
{
    _nextToken = next;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
