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
#include "llbc/core/log/BaseLogAppender.h"
#include "llbc/core/log/LogControlExecutor.h"

__LLBC_NS_BEGIN

LLBC_LogControlExecutor::LLBC_LogControlExecutor(const LLBC_LogControlItem &item)
: _item(item)
{
}

LLBC_LogControlExecutor *LLBC_LogControlExecutor::Create(const LLBC_LogControlItem &item)
{
    // Validate() sets LLBC_ERROR_INVALID on failure; mirror it through the
    // null return per llbc factory convention (cf. LLBC_BasePoller::Create).
    if (Validate(item) != LLBC_OK)
        return nullptr;

    // `new` + private ctor keeps callers from bypassing Validate(); the
    // factory is the only legitimate construction path. The single
    // allocation is on a configuration path, not the log hot path.
    return new LLBC_LogControlExecutor(item);
}

int LLBC_LogControlExecutor::Validate(const LLBC_LogControlItem &item)
{
    // Validate match-rules: at least one rule must be enabled.
    if (!item.HasAnyMatch())
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    // Per-rule sanity.
    if (item.haveFile)
    {
        if (item.matchFile.empty())
        {
            LLBC_SetLastError(LLBC_ERROR_INVALID);
            return LLBC_FAILED;
        }
        // Each segment must be a non-empty half-open interval [begin, end)
        // with begin >= 0 (and thus end > 0). Empty matchLineRanges means
        // whole-file wildcard, which is OK.
        for (const auto &seg : item.matchLineRanges)
        {
            if (seg.first < 0 || seg.second <= seg.first)
            {
                LLBC_SetLastError(LLBC_ERROR_INVALID);
                return LLBC_FAILED;
            }
        }
    }
    if (item.haveFunc)
    {
        if (item.matchFuncs.empty())
        {
            LLBC_SetLastError(LLBC_ERROR_INVALID);
            return LLBC_FAILED;
        }
        for (const auto &fn : item.matchFuncs)
        {
            if (fn.empty())
            {
                LLBC_SetLastError(LLBC_ERROR_INVALID);
                return LLBC_FAILED;
            }
        }
    }
    if (item.haveThreadId && item.matchThreadIds.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    if (item.haveLevel)
    {
        if (item.matchLevels.empty())
        {
            LLBC_SetLastError(LLBC_ERROR_INVALID);
            return LLBC_FAILED;
        }
        for (int lvl : item.matchLevels)
        {
            if (!LLBC_LogLevel::IsValid(lvl))
            {
                LLBC_SetLastError(LLBC_ERROR_INVALID);
                return LLBC_FAILED;
            }
        }
    }
    // Action.
    if (!LLBC_LogControlAction::IsValid(item.action))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    if (item.action == LLBC_LogControlAction::SetLevel &&
        !LLBC_LogLevel::IsValid(item.newLevel))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    // Appender scope: any unrecognized type rejects the whole item.
    for (int t : item.appenderTypes)
    {
        if (!LLBC_LogAppenderType::IsValid(t))
        {
            LLBC_SetLastError(LLBC_ERROR_INVALID);
            return LLBC_FAILED;
        }
    }

    return LLBC_OK;
}

bool LLBC_LogControlExecutor::Match(int appenderType,
                                    const LLBC_LogData &data,
                                    int currentLevel) const
{
    // 1) Appender scope: empty == all appenders allowed.
    if (!_item.appenderTypes.empty())
    {
        bool scopeHit = false;
        for (int t : _item.appenderTypes)
        {
            if (t == appenderType)
            {
                scopeHit = true;
                break;
            }
        }
        if (!scopeHit)
            return false;
    }

    // 2) Match rules. Enabled rules are AND-combined: any enabled rule that
    //    fails to match immediately disqualifies this executor. Each rule's
    //    own value-set (matchFuncs / matchThreadIds / matchLevels /
    //    matchLineRanges) is OR-combined (one-of).
    //
    //    Validate() rejects items with no enabled rule (HasAnyMatch()), so
    //    here at least one rule is enabled.

    // file rule (basename equality + optional line range OR-set).
    if (_item.haveFile)
    {
        // data.file is a fixed-size char[] in LLBC_LogData; treat empty
        // first-byte as "no file info attached to this record".
        if (data.file[0] == '\0')
            return false;
        if (_item.matchFile != data.file)
            return false;
        // Empty matchLineRanges == whole-file wildcard. Otherwise the line
        // matches if it lies in ANY segment (half-open [begin, end), like
        // other llbc range APIs).
        if (!_item.matchLineRanges.empty())
        {
            bool lineHit = false;
            for (const auto &seg : _item.matchLineRanges)
            {
                if (data.line >= seg.first && data.line < seg.second)
                {
                    lineHit = true;
                    break;
                }
            }
            if (!lineHit)
                return false;
        }
    }

    // func rule (OR over matchFuncs).
    if (_item.haveFunc)
    {
        if (data.func[0] == '\0')
            return false;
        bool hit = false;
        for (const auto &fn : _item.matchFuncs)
        {
            if (fn == data.func)
            {
                hit = true;
                break;
            }
        }
        if (!hit)
            return false;
    }

    // threadId rule (OR over matchThreadIds).
    if (_item.haveThreadId)
    {
        bool hit = false;
        for (auto tid : _item.matchThreadIds)
        {
            if (tid == data.threadId)
            {
                hit = true;
                break;
            }
        }
        if (!hit)
            return false;
    }

    // level rule (OR over matchLevels). Evaluated against the chain's
    // current effective level, NOT data.level — see Match() doc.
    if (_item.haveLevel)
    {
        bool hit = false;
        for (int lvl : _item.matchLevels)
        {
            if (lvl == currentLevel)
            {
                hit = true;
                break;
            }
        }
        if (!hit)
            return false;
    }

    return true;
}

void LLBC_LogControlExecutor::Action(ApplyResult &io) const
{
    // Mute short-circuits the chain (signaled via io.muted = true); Mgr is
    // responsible for breaking the loop and bumping the suppression counter.
    if (_item.action == LLBC_LogControlAction::Mute)
    {
        io.muted = true;
    }
    else // SetLevel
    {
        io.effectiveLevel = _item.newLevel;
    }
}

__LLBC_NS_END
