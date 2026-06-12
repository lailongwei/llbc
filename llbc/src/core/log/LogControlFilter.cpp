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

#include "llbc/core/thread/Guard.h"
#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/BaseLogAppender.h"
#include "llbc/core/log/LogControlFilter.h"

__LLBC_NS_BEGIN

LLBC_LogControlFilter::LLBC_LogControlFilter()
: _empty(true)
, _suppressedCount(0)
{
}

int LLBC_LogControlFilter::AddControl(const LLBC_LogControlItem &item)
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
        if (item.matchFile.empty() || item.matchLine < 0 || item.matchLineEnd < 0)
        {
            LLBC_SetLastError(LLBC_ERROR_INVALID);
            return LLBC_FAILED;
        }
        // Range mode (lineEnd > 0) requires a strictly positive, half-open
        // interval [matchLine, matchLineEnd) so that matchLine < matchLineEnd
        // and the interval covers at least one line.
        if (item.matchLineEnd > 0 && item.matchLineEnd <= item.matchLine)
        {
            LLBC_SetLastError(LLBC_ERROR_INVALID);
            return LLBC_FAILED;
        }
    }
    if (item.haveFunc && item.matchFunc.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    if (item.haveLevel && !LLBC_LogLevel::IsValid(item.matchLevel))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
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

    LLBC_LockGuard guard(_lock);
    _items.push_back(item);
    _empty = false;
    return LLBC_OK;
}

int LLBC_LogControlFilter::RemoveControl(size_t index)
{
    LLBC_LockGuard guard(_lock);
    if (index >= _items.size())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _items.erase(_items.begin() + static_cast<std::ptrdiff_t>(index));
    RefreshEmpty();
    return LLBC_OK;
}

void LLBC_LogControlFilter::ClearControls()
{
    LLBC_LockGuard guard(_lock);
    _items.clear();
    _empty = true;
}

size_t LLBC_LogControlFilter::GetControlCount() const
{
    LLBC_LockGuard guard(_lock);
    return _items.size();
}

void LLBC_LogControlFilter::GetControls(std::vector<LLBC_LogControlItem> &out) const
{
    out.clear();

    LLBC_LockGuard guard(_lock);
    out.reserve(_items.size());
    for (const auto &it : _items)
        out.push_back(it);
}

LLBC_LogControlFilter::ApplyResult LLBC_LogControlFilter::Apply(int appenderType,
                                                                const char *file,
                                                                int line,
                                                                const char *func,
                                                                LLBC_ThreadId threadId,
                                                                int level) const
{
    ApplyResult r{ false, level };

    LLBC_LockGuard guard(_lock);
    if (_items.empty())
        return r;

    for (const auto &item : _items)
    {
        // Scope filter.
        if (!ScopeAllows(item, appenderType))
            continue;

        // Match (one-of) — note: level is the *current* effective level so chained
        // SetLevel actions are observable to subsequent items.
        if (!MatchOne(item, file, line, func, threadId, r.effectiveLevel))
            continue;

        if (item.action == LLBC_LogControlAction::Mute)
        {
            r.muted = true;
            _suppressedCount.fetch_add(1, std::memory_order_relaxed);
            return r;
        }
        else // SetLevel
        {
            r.effectiveLevel = item.newLevel;
        }
    }

    return r;
}

void LLBC_LogControlFilter::RefreshEmpty()
{
    _empty = _items.empty();
}

bool LLBC_LogControlFilter::MatchOne(const LLBC_LogControlItem &item,
                                     const char *file,
                                     int line,
                                     const char *func,
                                     LLBC_ThreadId threadId,
                                     int level)
{
    // file rule
    if (item.haveFile)
    {
        if (file && *file != '\0' && item.matchFile == file)
        {
            // Range mode: [matchLine, matchLineEnd), half-open like
            // llbc's other range APIs (e.g. LLBC_Stream::Erase / Replace
            // use [n0, n1) semantics).
            if (item.matchLineEnd > 0)
            {
                if (line >= item.matchLine && line < item.matchLineEnd)
                    return true;
            }
            // Single-line mode (matchLine > 0) or whole-file wildcard
            // (matchLine == 0).
            else if (item.matchLine == 0 || item.matchLine == line)
            {
                return true;
            }
        }
    }
    // func rule
    if (item.haveFunc)
    {
        if (func && *func != '\0' && item.matchFunc == func)
            return true;
    }
    // threadId rule
    if (item.haveThreadId)
    {
        if (item.matchThreadId == threadId)
            return true;
    }
    // level rule
    if (item.haveLevel)
    {
        if (item.matchLevel == level)
            return true;
    }
    return false;
}

bool LLBC_LogControlFilter::ScopeAllows(const LLBC_LogControlItem &item, int appenderType)
{
    if (item.appenderTypes.empty())
        return true;
    for (int t : item.appenderTypes)
    {
        if (t == appenderType)
            return true;
    }
    return false;
}

__LLBC_NS_END
