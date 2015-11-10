/**
 * @file    LogFormattingInfo.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/09
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/log/LogFormattingInfo.h"

__LLBC_NS_BEGIN

LLBC_LogFormattingInfo::LLBC_LogFormattingInfo(bool leftAlign,
                                               int minLen,
                                               int maxLen,
                                               int fillCharacter)
: _leftAlign(leftAlign)
, _minLen(minLen)
, _maxLen(maxLen)
, _fillCharacter(fillCharacter)
{
}

LLBC_LogFormattingInfo::~LLBC_LogFormattingInfo()
{
}

bool LLBC_LogFormattingInfo::GetLeftAlign() const
{
    return _leftAlign;
}

void LLBC_LogFormattingInfo::SetLeftAligh(bool leftAlign)
{
    _leftAlign = leftAlign;
}

int LLBC_LogFormattingInfo::GetMinLen() const
{
    return _minLen;
}

void LLBC_LogFormattingInfo::SetMinLen(int minLen)
{
    _minLen = MAX(0, minLen);
}

int LLBC_LogFormattingInfo::GetMaxLen() const
{
    return _maxLen;
}

void LLBC_LogFormattingInfo::SetMaxLen(int maxLen)
{
    _maxLen = MAX(0, maxLen);
}

char LLBC_LogFormattingInfo::GetFillCharacter() const
{
    return _fillCharacter;
}

void LLBC_LogFormattingInfo::SetFillCharacter(char fillCharacter)
{
    _fillCharacter = fillCharacter;
}

void LLBC_LogFormattingInfo::Format(LLBC_String &data, int fieldStart) const
{
    fieldStart = MAX(0, fieldStart);
    fieldStart = MIN(fieldStart, static_cast<int>(data.length()));

    int rawLen = static_cast<int>(data.length()) - fieldStart;
    if (rawLen > _maxLen)
        data.erase(fieldStart + _maxLen, rawLen - _maxLen);

    if (rawLen < _minLen)
    {
        if (_leftAlign)
            data.append(_minLen - rawLen, _fillCharacter);
        else
            data.insert(fieldStart, _minLen - rawLen, _fillCharacter);
    }
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
