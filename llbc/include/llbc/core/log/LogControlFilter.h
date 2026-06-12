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
#include "llbc/core/log/LogControl.h"

#include <atomic>

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
struct LLBC_LogData;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The log output control filter.
 *
 * Holds an ordered list of LLBC_LogControlItem and applies them per-appender at
 * `LLBC_Logger::OutputLogData()`.
 *
 * Application semantics
 * ---------------------
 * - For one log record + one specific appender, items are applied in declaration order.
 * - An item's appenderTypes restricts where it takes effect; empty == all appenders.
 * - SetLevel actions rewrite the log level used for the rest of the per-appender
 *   chain (and for level-based rules of subsequent items in the chain). The rewrite
 *   does NOT leak across appenders: each appender starts from the original level.
 * - Mute action stops further processing for the current appender and reports
 *   suppression (suppressed count incremented exactly once per dropped record per
 *   appender).
 *
 * Thread-safety
 * -------------
 * Mutation (Add/Remove/Clear) and Apply() are serialized by an internal SpinLock.
 * A lock-free `IsEmpty()` fast-path lets callers skip the entire filter when no
 * control item is installed, which is the common case.
 */
class LLBC_HIDDEN LLBC_LogControlFilter
{
public:
    /**
     * Apply result of one Apply() invocation.
     */
    struct ApplyResult
    {
        bool muted;        // true if the record is dropped on this appender.
        int effectiveLevel; // level after possibly chained SetLevel(s).
    };

public:
    LLBC_LogControlFilter();
    ~LLBC_LogControlFilter() = default;

public:
    /**
     * Add a control item to the tail.
     * @param[in] item - the control item to add. Must have at least one match rule and a valid action.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddControl(const LLBC_LogControlItem &item);

    /**
     * Remove the control item at the given index.
     * @param[in] index - the item index, 0-based.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveControl(size_t index);

    /**
     * Clear all installed control items.
     */
    void ClearControls();

    /**
     * Get installed control item count.
     */
    size_t GetControlCount() const;

    /**
     * Snapshot all installed control items in declaration order. The output is cleared first.
     * @param[out] out - the items snapshot.
     */
    void GetControls(std::vector<LLBC_LogControlItem> &out) const;

    /**
     * Get suppressed (mute-action dropped) record count since construction or last reset.
     * Note: counted once per (record, appender) pair when a mute action fires.
     */
    uint64 GetSuppressedCount() const
    {
        return _suppressedCount.load(std::memory_order_relaxed);
    }

    /**
     * Reset the suppressed count to zero; rules are not affected.
     */
    void ResetSuppressedCount()
    {
        _suppressedCount.store(0, std::memory_order_relaxed);
    }

    /**
     * Check whether no control item is installed (lock-free fast-path).
     */
    bool IsEmpty() const { return _empty; }

    /**
     * Apply control items to one (log record, appender) pair.
     *
     * The current implementation does not require LLBC_LogData; the caller passes
     * the relevant fields directly. This avoids a header cycle on LogData.h.
     *
     * @param[in]  appenderType - the appender type currently being driven, see LLBC_LogAppenderType.
     * @param[in]  file         - log file basename (may be nullptr/empty).
     * @param[in]  line         - log line number.
     * @param[in]  func         - log function name (may be nullptr/empty).
     * @param[in]  threadId     - log native thread id.
     * @param[in]  level        - the input log level (record's original level).
     * @return ApplyResult - {muted, effectiveLevel}.
     */
    ApplyResult Apply(int appenderType,
                      const char *file,
                      int line,
                      const char *func,
                      LLBC_ThreadId threadId,
                      int level) const;

private:
    void RefreshEmpty();

    /**
     * Test whether the item's match-rules match the given log fields (one-of).
     */
    static bool MatchOne(const LLBC_LogControlItem &item,
                         const char *file,
                         int line,
                         const char *func,
                         LLBC_ThreadId threadId,
                         int level);

    /**
     * Test whether the appender scope allows this appender; empty == all.
     */
    static bool ScopeAllows(const LLBC_LogControlItem &item, int appenderType);

private:
    mutable LLBC_SpinLock _lock;
    std::vector<LLBC_LogControlItem> _items;

    // Lock-free fast-path: true iff _items is empty. Written under _lock; read lock-free
    // via IsEmpty(). Same caveats as the historical `volatile bool` style usage in llbc.
    volatile bool _empty;

    // Total number of records dropped by mute action, counted per (record, appender).
    mutable std::atomic<uint64> _suppressedCount;
};

__LLBC_NS_END
