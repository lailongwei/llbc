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

#pragma once

#include "llbc/common/Common.h"
#include "llbc/core/thread/SpinLock.h"

#include <atomic>

__LLBC_NS_BEGIN

/**
 * \brief The log mute filter.
 *
 * Supports three kinds of rules:
 *   1) File + line rule:
 *        Mute logs whose `LogData.file == <file>` AND `LogData.line == <line>`.
 *        Special case: `line == 0` is a wildcard meaning "any line of <file>".
 *   2) Function rule:
 *        Mute logs whose `LogData.func == <func>`.
 *   3) File + level rule:
 *        Mute logs whose `LogData.file == <file>` AND `LogData.level < <minLevel>`.
 *        I.e. drop everything in <file> below <minLevel>; logs at or above
 *        <minLevel> still pass (unless caught by another rule).
 *
 * A log record is muted iff it matches any installed rule.
 *
 * Note: LogData.file is normally the basename of __FILE__ (controlled by the
 * logger's "logCodeFilePath" config). Rules' file strings must use the same
 * form to actually match.
 *
 * Thread-safety:
 *   This class is self-contained and thread-safe. Rule mutation
 *   (Add/Remove/Clear) is expected to be infrequent and is serialized by an
 *   internal SpinLock. The hot path (ShouldMute) takes the same lock.
 *   The deliberately lock-free `IsEmpty()` fast path lets callers skip both
 *   the lock acquisition and the hash lookups when no rules are installed,
 *   which is the common case for most projects.
 */
class LLBC_HIDDEN LLBC_LogMuteFilter
{
public:
    LLBC_LogMuteFilter();
    ~LLBC_LogMuteFilter() = default;

public:
    /**
     * Add a file+line mute rule.
     * @param[in] file - the file basename.
     * @param[in] line - the file line number, 0 means wildcard(any line of the file).
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddFileLineRule(const LLBC_String &file, int line);

    /**
     * Remove a file+line mute rule.
     * @param[in] file - the file basename.
     * @param[in] line - the file line number, 0 only removes the wildcard entry.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveFileLineRule(const LLBC_String &file, int line);

    /**
     * Remove all file+line mute rules of the specific file(both wildcard and specific lines).
     * @param[in] file - the file basename.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveFileRule(const LLBC_String &file);

    /**
     * Add a function-name mute rule.
     * @param[in] func - the function name.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddFuncRule(const LLBC_String &func);

    /**
     * Remove a function-name mute rule.
     * @param[in] func - the function name.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveFuncRule(const LLBC_String &func);

    /**
     * Add a file+level mute rule.
     * Logs in the file with level less than minLevel are muted; if a rule of
     * the same file already exists, it is overwritten.
     * @param[in] file     - the file basename.
     * @param[in] minLevel - the minimum kept log level.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddFileLevelRule(const LLBC_String &file, int minLevel);

    /**
     * Remove the file+level mute rule of the specific file.
     * @param[in] file - the file basename.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveFileLevelRule(const LLBC_String &file);

    /**
     * Clear all mute rules.
     */
    void ClearRules();

    /**
     * Get the count of installed file+line mute rules.
     * @return size_t - the rule count.
     */
    size_t GetFileLineRuleCount() const;

    /**
     * Get the count of installed function-name mute rules.
     * @return size_t - the rule count.
     */
    size_t GetFuncRuleCount() const;

    /**
     * Get the count of installed file+level mute rules.
     * @return size_t - the rule count.
     */
    size_t GetFileLevelRuleCount() const;

    /**
     * Snapshot all installed file+line mute rules. The output is cleared first; order unspecified.
     * @param[out] out - the rule snapshot, each entry is (file, line); line == 0 means wildcard.
     */
    void GetFileLineRules(std::vector<std::pair<LLBC_String, int> > &out) const;

    /**
     * Snapshot all installed function-name mute rules. The output is cleared first; order unspecified.
     * @param[out] out - the rule snapshot.
     */
    void GetFuncRules(std::vector<LLBC_String> &out) const;

    /**
     * Snapshot all installed file+level mute rules. The output is cleared first; order unspecified.
     * @param[out] out - the rule snapshot, each entry is (file, minLevel).
     */
    void GetFileLevelRules(std::vector<std::pair<LLBC_String, int> > &out) const;

    /**
     * Get the count of muted log records since construction or last ResetMutedCount().
     * @return uint64 - the muted log record count.
     */
    uint64 GetMutedCount() const
    {
        return _mutedCount.load(std::memory_order_relaxed);
    }

    /**
     * Reset the muted log record count to zero, rules are not affected.
     */
    void ResetMutedCount()
    {
        _mutedCount.store(0, std::memory_order_relaxed);
    }

    /**
     * Check whether no mute rule is installed(lock-free fast-path).
     * @return bool - true if no rule installed, otherwise false.
     */
    bool IsEmpty() const { return _empty; }

    /**
     * Test whether the given log record should be muted.
     * @param[in] file  - the log file basename, can be nullptr.
     * @param[in] line  - the log file line number.
     * @param[in] level - the log level.
     * @param[in] func  - the log function name, can be nullptr.
     * @return bool - true if the log record should be muted, otherwise false.
     */
    bool ShouldMute(const char *file, int line, int level, const char *func) const;

private:
    void RefreshEmpty();

private:
    mutable LLBC_SpinLock _lock;

    // file -> set of muted line numbers. The value 0 means
    // "wildcard: mute any line of this file".
    std::unordered_map<std::string, std::unordered_set<int> > _fileLines;
    // muted function names.
    std::unordered_set<std::string> _funcs;
    // file -> minimum kept level. Logs in this file with level < minLevel
    // are muted; logs at or above minLevel still pass (unless caught by
    // another rule).
    std::unordered_map<std::string, int> _fileLevels;

    // Lock-free fast-path flag (true iff no rules installed). Written under
    // the internal lock; read lock-free via IsEmpty().
    // NOTE: 出于与 llbc 既有风格保持一致考虑，仍使用 volatile bool；
    //       严格的内存序与 TSan 合规问题见 doc/log_mute_filter.md 5.5。
    volatile bool _empty;

    // Total number of log records dropped by ShouldMute().
    mutable std::atomic<uint64> _mutedCount;
};

__LLBC_NS_END
