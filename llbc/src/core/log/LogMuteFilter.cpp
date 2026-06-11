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
#include "llbc/core/log/LogMuteFilter.h"

__LLBC_NS_BEGIN

LLBC_LogMuteFilter::LLBC_LogMuteFilter()
: _empty(true)
, _mutedCount(0)
{
}

int LLBC_LogMuteFilter::AddFileLineRule(const LLBC_String &file, int line)
{
    if (file.empty() || line < 0)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);

    auto &lines = _fileLines[file];
    if (!lines.insert(line).second)
    {
        if (lines.empty())
            _fileLines.erase(file);
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    _empty = false;
    return LLBC_OK;
}

int LLBC_LogMuteFilter::RemoveFileLineRule(const LLBC_String &file, int line)
{
    LLBC_LockGuard guard(_lock);

    const auto it = _fileLines.find(file);
    if (it == _fileLines.end() || it->second.erase(line) == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    if (it->second.empty())
        _fileLines.erase(it);

    RefreshEmpty();
    return LLBC_OK;
}

int LLBC_LogMuteFilter::RemoveFileRule(const LLBC_String &file)
{
    LLBC_LockGuard guard(_lock);

    if (_fileLines.erase(file) == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    RefreshEmpty();
    return LLBC_OK;
}

int LLBC_LogMuteFilter::AddFuncRule(const LLBC_String &func)
{
    if (func.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);

    if (!_funcs.insert(func).second)
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    _empty = false;
    return LLBC_OK;
}

int LLBC_LogMuteFilter::RemoveFuncRule(const LLBC_String &func)
{
    LLBC_LockGuard guard(_lock);

    if (_funcs.erase(func) == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    RefreshEmpty();
    return LLBC_OK;
}

int LLBC_LogMuteFilter::AddFileLevelRule(const LLBC_String &file, int minLevel)
{
    if (file.empty() || !LLBC_LogLevel::IsValid(minLevel))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);

    // Overwrite-on-conflict.
    _fileLevels[file] = minLevel;
    _empty = false;
    return LLBC_OK;
}

int LLBC_LogMuteFilter::RemoveFileLevelRule(const LLBC_String &file)
{
    LLBC_LockGuard guard(_lock);

    if (_fileLevels.erase(file) == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    RefreshEmpty();
    return LLBC_OK;
}

void LLBC_LogMuteFilter::ClearRules()
{
    LLBC_LockGuard guard(_lock);

    _fileLines.clear();
    _funcs.clear();
    _fileLevels.clear();
    _empty = true;
}

size_t LLBC_LogMuteFilter::GetFileLineRuleCount() const
{
    LLBC_LockGuard guard(_lock);

    size_t cnt = 0;
    for (const auto &kv : _fileLines)
        cnt += kv.second.size();
    return cnt;
}

size_t LLBC_LogMuteFilter::GetFuncRuleCount() const
{
    LLBC_LockGuard guard(_lock);
    return _funcs.size();
}

size_t LLBC_LogMuteFilter::GetFileLevelRuleCount() const
{
    LLBC_LockGuard guard(_lock);
    return _fileLevels.size();
}

void LLBC_LogMuteFilter::GetFileLineRules(std::vector<std::pair<LLBC_String, int> > &out) const
{
    out.clear();

    LLBC_LockGuard guard(_lock);

    // Reserve a reasonable lower bound; exact size requires a pre-pass.
    out.reserve(_fileLines.size());
    for (const auto &kv : _fileLines)
    {
        for (const auto &line : kv.second)
            out.emplace_back(LLBC_String(kv.first.data(), kv.first.size()), line);
    }
}

void LLBC_LogMuteFilter::GetFuncRules(std::vector<LLBC_String> &out) const
{
    out.clear();

    LLBC_LockGuard guard(_lock);

    out.reserve(_funcs.size());
    for (const auto &f : _funcs)
        out.emplace_back(f.data(), f.size());
}

void LLBC_LogMuteFilter::GetFileLevelRules(std::vector<std::pair<LLBC_String, int> > &out) const
{
    out.clear();

    LLBC_LockGuard guard(_lock);

    out.reserve(_fileLevels.size());
    for (const auto &kv : _fileLevels)
        out.emplace_back(LLBC_String(kv.first.data(), kv.first.size()), kv.second);
}

bool LLBC_LogMuteFilter::ShouldMute(const char *file, int line, int level, const char *func) const
{
    LLBC_LockGuard guard(_lock);

    const bool fileValid = (file && *file != '\0');

    // Check (file, line) rules.
    if (!_fileLines.empty() && fileValid)
    {
        const auto it = _fileLines.find(file);
        if (it != _fileLines.end())
        {
            const auto &lineSet = it->second;
            // Wildcard (line == 0) matches any line of this file.
            if (lineSet.count(0) > 0 || lineSet.count(line) > 0)
            {
                _mutedCount.fetch_add(1, std::memory_order_relaxed);
                return true;
            }
        }
    }

    // Check function rules.
    if (!_funcs.empty() && func && *func != '\0')
    {
        if (_funcs.count(func) > 0)
        {
            _mutedCount.fetch_add(1, std::memory_order_relaxed);
            return true;
        }
    }

    // Check (file, minLevel) rules: drop logs whose level is below minLevel.
    if (!_fileLevels.empty() && fileValid)
    {
        const auto it = _fileLevels.find(file);
        if (it != _fileLevels.end() && level < it->second)
        {
            _mutedCount.fetch_add(1, std::memory_order_relaxed);
            return true;
        }
    }

    return false;
}

void LLBC_LogMuteFilter::RefreshEmpty()
{
    _empty = _fileLines.empty() && _funcs.empty() && _fileLevels.empty();
}

__LLBC_NS_END
