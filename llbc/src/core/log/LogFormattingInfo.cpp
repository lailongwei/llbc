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
