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
#include "llbc/core/log/LogControlExecutor.h"

#include <memory>
#include <vector>

__LLBC_NS_BEGIN

// Pre-declare.
struct LLBC_LogData;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The log output control manager.
 *
 * Chain scheduler over an ordered list of LLBC_LogControlExecutor, driven
 * per-appender at LLBC_Logger::OutputLogData(). Per-item match/action logic
 * lives in the executor; Mgr only performs ordered traversal, snapshot
 * publish, and global suppression accounting.
 *
 * Writer model: the control list is configuration-only. SetControls() is
 * the only writer entry point (called at Logger::Initialize() and
 * LoggerMgr::Reload()); every write replaces the whole list. Validation
 * happens exactly once, at SetControls() time.
 *
 * Thread-safety: all public methods are safe to call from any thread. A
 * single LLBC_SpinLock (`_lock`) guards `_execs` (the snapshot pointer) and
 * `_suppressedCount`. Readers copy the shared_ptr under the lock and
 * traverse the immutable snapshot outside it, so chain evaluation never
 * blocks writers or other readers.
 */
class LLBC_HIDDEN LLBC_LogControlMgr
{
public:
    // Re-export the namespace-scope ApplyResult so legacy callers can spell
    // it as LLBC_LogControlMgr::ApplyResult.
    using ApplyResult = LLBC_LogControlApplyResult;

    // Immutable, ordered executor snapshot; each element is a unique_ptr so
    // executor addresses are stable for the snapshot's lifetime.
    using ExecutorList = std::vector<std::unique_ptr<LLBC_LogControlExecutor> >;

public:
    /**
     * Constructor. Starts with no installed controls and suppressed count 0.
     */
    LLBC_LogControlMgr();
    ~LLBC_LogControlMgr() = default;

public:
    /**
     * Replace the entire installed control item list with `items`.
     *
     * All-or-nothing: every item is validated first (by building executors);
     * if any fails, nothing is changed and LLBC_FAILED is returned with
     * LLBC_ERROR_INVALID set. On success a new snapshot is published via a
     * shared_ptr swap; in-flight Apply() callers keep using the previously
     * captured snapshot until they return.
     *
     * Passing an empty vector publishes a null snapshot (IsEmpty() returns
     * true). The suppressed count is preserved across calls; use
     * ResetSuppressedCount() to start a fresh baseline.
     *
     * @param[in] items - the new full list of control items.
     * @return int - LLBC_OK on success, LLBC_FAILED on any invalid item.
     */
    int SetControls(const std::vector<LLBC_LogControlItem> &items);

    /**
     * @return size_t - number of installed control items.
     */
    size_t GetControlCount() const;

    /**
     * Snapshot all installed control items (as configuration DTOs) in
     * declaration order.
     * @param[out] out - receives the items snapshot; cleared first.
     */
    void GetControls(std::vector<LLBC_LogControlItem> &out) const;

    /**
     * @return uint64 - total records dropped by mute action since
     *                  construction or last reset, counted once per
     *                  (record, appender) pair.
     */
    uint64 GetSuppressedCount() const;

    /**
     * Reset the suppressed count to zero.
     */
    void ResetSuppressedCount();

    /**
     * @return bool - true iff no control item is installed.
     */
    bool IsEmpty() const;

    /**
     * Apply the executor chain to one (log record, appender) pair and
     * return the merged outcome.
     *
     * Chain-merge semantics (declaration order):
     *   - SetLevel : overwrites `effectiveLevel`; the new level is visible
     *                to subsequent executors' level rules, so chains like
     *                    [SetLevel(Warn) -> match level==Warn -> Mute]
     *                are observable and intentional.
     *   - Mute     : sets `muted=true`, bumps the suppression counter once,
     *                and short-circuits the chain.
     *   - No match : `muted=false`, `effectiveLevel==originalLevel`.
     *
     * The rewrite does NOT leak across appenders: each Apply() call seeds
     * `effectiveLevel` from `originalLevel`.
     *
     * @param[in] appenderType  - the appender type currently being driven,
     *                            see LLBC_LogAppenderType.
     * @param[in] data          - the log record; `data.level` is ignored
     *                            in favour of `originalLevel` (so callers
     *                            may safely rewrite `data.level` for
     *                            per-appender output before calling Apply).
     * @param[in] originalLevel - the record's original (pre-chain) level;
     *                            seeds `effectiveLevel` and is the level
     *                            input for the first executor.
     * @return ApplyResult - merged {muted, effectiveLevel}.
     */
    ApplyResult Apply(int appenderType,
                      const LLBC_LogData &data,
                      int originalLevel) const;

private:
    // Non-copyable: owns a spinlock and a side-channel counter (see below);
    // copying a Mgr would duplicate neither meaningfully.
    LLBC_DISABLE_ASSIGNMENT(LLBC_LogControlMgr);

private:
    // Internal synchronization. Guards `_execs` (the snapshot pointer itself)
    // and `_suppressedCount`. The vector pointed to by `_execs` is immutable,
    // so once a reader has copied the `shared_ptr` under the lock it can
    // traverse the snapshot freely after releasing.
    mutable LLBC_SpinLock _lock;

    // Immutable executor snapshot, published wholesale by SetControls(). The
    // pointed vector is `const` — once published, it is never mutated. Each
    // element is a unique_ptr<Executor> so executor addresses are stable for
    // the lifetime of the snapshot.
    std::shared_ptr<const ExecutorList> _execs;

    // Total number of records dropped by mute action, counted per
    // (record, appender).
    mutable uint64 _suppressedCount;
};

__LLBC_NS_END
