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
    _colorLogTag = false;
}

int LLBC_LogTraceMgr::AddLogTrace(const LLBC_LogTrace &logTrace)
{
    // Not allow empty trace content.
    if (UNLIKELY(!logTrace.traceContent))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    // Insert to _logTracesEx or add traceTimes.
    // - Find trace contents.
    bool needRebuild = false;
    auto keyIt = _logTracesEx.find(logTrace.traceKey);
    if (UNLIKELY(keyIt == _logTracesEx.end()))
        keyIt = _logTracesEx.emplace(logTrace.traceKey, std::vector<std::pair<LLBC_LogTrace::TraceContent, sint64>>()).first;

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
    {
        RebuildTraceInfo();
        UpdateColorLogTag();
    }
    return LLBC_OK;
}

int LLBC_LogTraceMgr::RemoveLogTrace(const LLBC_LogTrace &logTrace, bool setTraceTimesToZero)
{
    const auto keyIt = _logTracesEx.find(logTrace.traceKey);
    if (keyIt == _logTracesEx.end())
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
    RebuildTraceInfo();
    UpdateColorLogTag();

    return LLBC_OK;
}

void LLBC_LogTraceMgr::ClearLogTrace(const LLBC_LogTrace::TraceKey &traceKey)
{
    const auto keyIt = _logTracesEx.find(traceKey);
    if (keyIt == _logTracesEx.end())
        return;

    const bool needRebuild = !keyIt->second.empty();
    _logTracesEx.erase(keyIt);
    UpdateColorLogTag();

    if (needRebuild)
        RebuildTraceInfo();
}

size_t LLBC_LogTraceMgr::GetLogTraceTimes(const LLBC_LogTrace &logTrace) const
{
    const auto keyIt = _logTracesEx.find(logTrace.traceKey);
    if (keyIt == _logTracesEx.end())
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
    _logTracesEx.clear();
    _colorLogTag = false;
}

void LLBC_LogTraceMgr::UpdateColorLogTraces(const LLBC_LogTraces &requireColorLogTraces)
{
    _requireColorLogTraces = requireColorLogTraces;
    UpdateColorLogTag();
}

void LLBC_LogTraceMgr::UpdateColorLogTag()
{
    for (const auto &logTrace : _logTracesEx)
    {  
        // Check key.
        auto requireColorKeyContentsIt = _requireColorLogTraces.find(logTrace.first);
        if (requireColorKeyContentsIt == _requireColorLogTraces.end())
            continue;

        const auto &requireColorContents = requireColorKeyContentsIt->second;
        const auto &traceContents = logTrace.second;
        for (const auto &traceContent : traceContents)
        { 
            auto requireContentIt = std::find(requireColorContents.begin(), requireColorContents.end(), traceContent.first);
            if (requireContentIt != requireColorContents.end()) 
            {
                _colorLogTag = true;
                return;
            }
        }
    }

    _colorLogTag = false;
}

int LLBC_LogTraceMgr::AddColorLogTrace(const LLBC_LogTrace &logTrace) 
{
    auto keyContentsIt = _requireColorLogTraces.find(logTrace.traceKey);
    if (keyContentsIt == _requireColorLogTraces.end())
       keyContentsIt = _requireColorLogTraces.emplace(logTrace.traceKey, std::vector<LLBC_LogTrace::TraceContent>()).first;

    // - Find content it, insert new content.
    auto &contents = keyContentsIt->second;
    const auto contentIt = std::find(contents.begin(), contents.end(), logTrace.traceContent);
    if (contentIt != contents.end()) 
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    contents.emplace_back(logTrace.traceContent);
    UpdateColorLogTag();

    return LLBC_OK;
}

int LLBC_LogTraceMgr::RemoveColorLogTrace(const LLBC_LogTrace &logTrace) 
{
    const auto keyContentsIt = _requireColorLogTraces.find(logTrace.traceKey);
    if (keyContentsIt == _requireColorLogTraces.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    // - Find content it, delete content.
    auto &contents = keyContentsIt->second;
    const auto contentIt = std::find(contents.begin(), contents.end(), logTrace.traceContent);
    if (contentIt == contents.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    // - Delete from contents list.
    contents.erase(contentIt);
    if (contents.empty()) 
        _requireColorLogTraces.erase(keyContentsIt);

    UpdateColorLogTag();

    return LLBC_OK;
}

int LLBC_LogTraceMgr::RemoveColorLogKey(const LLBC_LogTrace::TraceKey &traceKey)
{
    const auto keyContentsIt = _requireColorLogTraces.find(traceKey);
    if (keyContentsIt == _requireColorLogTraces.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _requireColorLogTraces.erase(keyContentsIt);
    UpdateColorLogTag();

    return LLBC_OK;
}

void LLBC_LogTraceMgr::ClearAllColorLogTraces()
{
    _requireColorLogTraces.clear();
    _colorLogTag = false;
}

void LLBC_LogTraceMgr::SetSeparators(char logTracesSep, char traceKeyContentSep, char traceContentsSep)
{
    _logTracesSep = logTracesSep;
    _logTraceKeyContentSep = traceKeyContentSep;
    _traceContentsSep = traceContentsSep;

    RebuildTraceInfo();
}

void LLBC_LogTraceMgr::RebuildTraceInfo()
{
    auto newTraceInfo = new LLBC_String;
    for (auto &[traceKey, traceContents] : _logTracesEx)
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
