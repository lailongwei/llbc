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

#include "llbc/core/log/LogData.h"
#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/LogMatchRule.h"

__LLBC_NS_BEGIN

int LLBC_LogFileMatchRule::Initialize(const LLBC_LogControlItem &item)
{
    // Empty lineRanges == whole-file wildcard (OK); non-empty segments must
    // be half-open [begin, end) with begin >= 0.
    if (item.file.file.empty())
        return LLBC_FAILED;
    for (const auto &seg : item.file.lineRanges)
    {
        if (seg.first < 0 || seg.second <= seg.first)
            return LLBC_FAILED;
    }

    _matchFile = item.file.file;
    _matchLineRanges = item.file.lineRanges;
    return LLBC_OK;
}

bool LLBC_LogFileMatchRule::Match(const LLBC_LogData &data, int currentLevel) const
{
    if (data.file[0] == '\0')
        return false;
    if (_matchFile != data.file)
        return false;
    if (_matchLineRanges.empty())
        return true;
    for (const auto &seg : _matchLineRanges)
    {
        if (data.line >= seg.first && data.line < seg.second)
            return true;
    }
    return false;
}

int LLBC_LogFuncMatchRule::Initialize(const LLBC_LogControlItem &item)
{
    if (item.func.values.empty())
        return LLBC_FAILED;
    for (const auto &fn : item.func.values)
    {
        if (fn.empty())
            return LLBC_FAILED;
    }

    _matchFuncs = item.func.values;
    return LLBC_OK;
}

bool LLBC_LogFuncMatchRule::Match(const LLBC_LogData &data, int currentLevel) const
{
    if (data.func[0] == '\0')
        return false;
    for (const auto &fn : _matchFuncs)
    {
        if (fn == data.func)
            return true;
    }
    return false;
}

int LLBC_LogThreadIdMatchRule::Initialize(const LLBC_LogControlItem &item)
{
    if (item.threadId.values.empty())
        return LLBC_FAILED;

    _matchThreadIds = item.threadId.values;
    return LLBC_OK;
}

bool LLBC_LogThreadIdMatchRule::Match(const LLBC_LogData &data, int currentLevel) const
{
    for (auto tid : _matchThreadIds)
    {
        if (tid == data.threadId)
            return true;
    }
    return false;
}

int LLBC_LogLevelMatchRule::Initialize(const LLBC_LogControlItem &item)
{
    if (item.level.values.empty())
        return LLBC_FAILED;
    for (int lvl : item.level.values)
    {
        if (!LLBC_LogLevel::IsValid(lvl))
            return LLBC_FAILED;
    }

    _matchLevels = item.level.values;
    return LLBC_OK;
}

bool LLBC_LogLevelMatchRule::Match(const LLBC_LogData &data, int currentLevel) const
{
    // Matches currentLevel (chain-rewritten effective level), not data.level,
    // so "SetLevel then match by level" is composable.
    for (int lvl : _matchLevels)
    {
        if (lvl == currentLevel)
            return true;
    }
    return false;
}

bool LLBC_BaseLogMatchRule::IsEnabled(int matchType, const LLBC_LogControlItem &item)
{
    switch (matchType)
    {
    case LLBC_LogControlMatchType::File:     return item.file.enabled;
    case LLBC_LogControlMatchType::Func:     return item.func.enabled;
    case LLBC_LogControlMatchType::ThreadId: return item.threadId.enabled;
    case LLBC_LogControlMatchType::Level:    return item.level.enabled;
    default:                                 return false;
    }
}

LLBC_BaseLogMatchRule *LLBC_BaseLogMatchRule::Create(int matchType, const LLBC_LogControlItem &item)
{
    LLBC_BaseLogMatchRule *rule = nullptr;
    switch (matchType)
    {
    case LLBC_LogControlMatchType::File:
        rule = new LLBC_LogFileMatchRule;
        break;

    case LLBC_LogControlMatchType::Func:
        rule = new LLBC_LogFuncMatchRule;
        break;

    case LLBC_LogControlMatchType::ThreadId:
        rule = new LLBC_LogThreadIdMatchRule;
        break;

    case LLBC_LogControlMatchType::Level:
        rule = new LLBC_LogLevelMatchRule;
        break;

    default:
        return nullptr;
    }

    if (rule->Initialize(item) != LLBC_OK)
    {
        LLBC_XDelete(rule);
        return nullptr;
    }

    return rule;
}

__LLBC_NS_END
