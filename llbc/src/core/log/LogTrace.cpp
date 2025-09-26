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

#include "llbc/core/log/LogTrace.h"

__LLBC_NS_BEGIN

LLBC_LogTraceMgr::LLBC_LogTraceMgr(char logTracesSep, char traceKeyContentSep, char traceContentsSep)
: _logTracesSep(logTracesSep)
, _logTraceKeyContentSep(traceKeyContentSep)
, _traceContentsSep(traceContentsSep)
{
}

int LLBC_LogTraceMgr::AddLogTrace(const LLBC_LogTrace &logTrace)
{
    // Not allow empty trace content.
    if (UNLIKELY(!logTrace.traceContent))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    // Insert to _logTraces or add traceTimes.
    // - Find trace contents.
    bool needRebuild = false;
    auto keyIt = _logTraces.find(logTrace.traceKey);
    if (UNLIKELY(keyIt == _logTraces.end()))
        keyIt = _logTraces.emplace(logTrace.traceKey, std::vector<std::pair<LLBC_LogTrace::TraceContent, sint64>>()).first;

    // - Find content it, incr traceTimes or insert new content.
    auto &traceContents = keyIt->second;
    const auto contentIt = std::find_if(traceContents.begin(),
                                        traceContents.end(),
                                        [&logTrace](const auto &item) {
        return item.first == logTrace.traceContent;
    });
    if (contentIt == traceContents.end())
    {
        if (UNLIKELY(traceContents.size() >= LLBC_CFG_CORE_LOG_TRACE_SAME_KEY_CONTENT_COUNT_LIMIT))
        {
            LLBC_SetLastError(LLBC_ERROR_LIMIT);
            return LLBC_FAILED;
        }

        traceContents.emplace_back(logTrace.traceContent, 1);
        needRebuild = true;
    }
    else
    {
        (*contentIt).second += 1;
    }

    if (needRebuild)
        _RebuildTraceInfo();

    return LLBC_OK;
}

int LLBC_LogTraceMgr::RemoveLogTrace(const LLBC_LogTrace &logTrace, bool setTraceTimesToZero)
{
    const auto keyIt = _logTraces.find(logTrace.traceKey);
    if (keyIt == _logTraces.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    auto &traceContents = keyIt->second;
    const auto contentIt = std::find_if(traceContents.begin(),
                                        traceContents.end(),
                                        [&logTrace](const auto &item) {
        return item.first == logTrace.traceContent;
    });
    if (contentIt == traceContents.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    auto &traceContentPair = *contentIt;
    if (!setTraceTimesToZero && --traceContentPair.second > 0)
            return LLBC_OK;

    traceContents.erase(contentIt);
    _RebuildTraceInfo();

    return LLBC_OK;
}

void LLBC_LogTraceMgr::ClearLogTrace(const LLBC_LogTrace::TraceKey &traceKey)
{
    const auto keyIt = _logTraces.find(traceKey);
    if (keyIt == _logTraces.end())
        return;

    const bool needRebuild = !keyIt->second.empty();
    _logTraces.erase(keyIt);

    if (needRebuild)
        _RebuildTraceInfo();
}

const std::vector<std::pair<LLBC_LogTrace::TraceContent, sint64>>* LLBC_LogTraceMgr::GetLogTraceContents(const LLBC_LogTrace::TraceKey &traceKey) const
{
    const auto keyIt = _logTraces.find(traceKey);
    if (keyIt == _logTraces.end())
    {
        return nullptr;
    }
    return &(keyIt->second);
}

size_t LLBC_LogTraceMgr::GetLogTraceTimes(const LLBC_LogTrace &logTrace) const
{
    const auto keyIt = _logTraces.find(logTrace.traceKey);
    if (keyIt == _logTraces.end())
        return 0;

    const auto &traceContents = keyIt->second;
    const auto contentIt = std::find_if(traceContents.begin(),
                                        traceContents.end(),
                                        [&logTrace](const auto &item) {
        return item.first == logTrace.traceContent;
    });

    return contentIt != traceContents.end() ? contentIt->second : 0;
}

void LLBC_LogTraceMgr::ClearAllLogTraces()
{
    _traceInfo.reset();
    _logTraces.clear();
}

void LLBC_LogTraceMgr::SetSeparators(char logTracesSep, char traceKeyContentSep, char traceContentsSep)
{
    _logTracesSep = logTracesSep;
    _logTraceKeyContentSep = traceKeyContentSep;
    _traceContentsSep = traceContentsSep;

    _RebuildTraceInfo();
}

void LLBC_LogTraceMgr::_RebuildTraceInfo()
{
    auto newTraceInfo = new LLBC_String;
    for (auto &[traceKey, traceContents] : _logTraces)
    {
        if (traceContents.empty())
            continue;

        if (!newTraceInfo->empty())
            newTraceInfo->append(1, _logTracesSep);

        if (traceKey)
        {
            newTraceInfo->append(traceKey.str, traceKey.strLen);
            newTraceInfo->append(1, _logTraceKeyContentSep);
        }

        for (size_t traceContentIdx = 0; traceContentIdx < traceContents.size(); ++traceContentIdx)
        {
            if (traceContentIdx != 0)
                newTraceInfo->append(1, _traceContentsSep);
            const auto &traceContent = traceContents[traceContentIdx].first;
            newTraceInfo->append(traceContent.str, traceContent.strLen);
        }

        if (newTraceInfo->size() >= LLBC_CFG_CORE_LOG_TRACE_BUILT_CONTENT_SIZE_LIMIT)
        {
            newTraceInfo->resize(LLBC_CFG_CORE_LOG_TRACE_BUILT_CONTENT_SIZE_LIMIT - 1);
            break;
        }
    }

    _traceInfo.reset(newTraceInfo);
}


__LLBC_NS_END
