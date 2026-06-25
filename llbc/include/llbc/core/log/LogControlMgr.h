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

#include <atomic>
#include <memory>
#include <vector>

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
struct LLBC_LogData;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The log output control manager.
 *
 * Holds an ordered list of LLBC_LogControlExecutor (one executor per
 * configured LLBC_LogControlItem) and drives them per-appender at
 * `LLBC_Logger::OutputLogData()`.
 *
 * Responsibilities
 * ----------------
 * Mgr is the **chain scheduler**:
 *   - ordered traversal of executors;
 *   - atomic snapshot publish on the single writer entry point (SetControls);
 *   - global suppression accounting (per (record, appender) pair).
 *
 * Per-item logic (match-rules + scope filter + Mute/SetLevel action) lives
 * in LLBC_LogControlExecutor, which Mgr does not introspect — Mgr only asks
 * each executor "do you fire?" then "apply yourself".
 *
 * Writer model
 * ------------
 * The control list is **configuration-only**: it is populated from
 * LoggerConfigInfo at Logger::Initialize() and re-populated from a freshly
 * parsed config at LoggerMgr::Reload(). There is intentionally NO runtime
 * add/remove/clear API — every write is a whole-list replacement via
 * SetControls(). This keeps the writer surface minimal and lets log emitters
 * assume an installed executor is always well-formed (validation happens
 * exactly once, at SetControls() time).
 *
 * Application semantics
 * ---------------------
 * - For one log record + one specific appender, executors are applied in
 *   declaration order.
 * - SetLevel actions rewrite the log level used for the rest of the
 *   per-appender chain (and for level-based rules of subsequent executors in
 *   the chain). The rewrite does NOT leak across appenders: each appender
 *   starts from the original level.
 * - Mute action stops further processing for the current appender and reports
 *   suppression (suppressed count incremented exactly once per dropped record
 *   per appender).
 *
 * Thread-safety
 * -------------
 * Read-side (Apply / GetControlCount / GetControls / IsEmpty) is **lock-free**:
 * `_execs` is held as a `shared_ptr<const vector<unique_ptr<Executor>>>`;
 * readers atomically load the pointer, take a reference, and operate on an
 * immutable snapshot. The (sole) writer SetControls() builds the new vector
 * entirely outside the lock, then atomically publishes it (copy-on-write).
 * `_writeLock` only serializes concurrent SetControls() callers among
 * themselves (e.g. overlapping Reload()s); a write never blocks any reader.
 *
 * Visibility note: a successful SetControls() takes effect for the *next*
 * Apply() invocation; any Apply() already mid-traversal completes against
 * its captured snapshot. This is the desired semantics for log control.
 *
 * `IsEmpty()` is a `volatile bool` fast-path used by the logger to skip the
 * entire control mechanism when no executor is installed (the common case).
 */
class LLBC_HIDDEN LLBC_LogControlMgr
{
public:
    // Re-export the public ApplyResult type for callers that historically used
    // `LLBC_LogControlMgr::ApplyResult` (e.g. LLBC_Logger). The canonical type
    // lives at namespace scope so LLBC_LogControlExecutor can return/consume
    // it without a cyclic include via this header.
    using ApplyResult = LLBC_LogControlApplyResult;

    // Internal snapshot type: an immutable, ordered list of executors. Held
    // by shared_ptr<const ...> for lock-free reader access (see class-level
    // doc).
    using ExecutorList = std::vector<std::unique_ptr<LLBC_LogControlExecutor> >;

public:
    LLBC_LogControlMgr();
    ~LLBC_LogControlMgr() = default;

public:
    /**
     * Atomically replace the entire installed control item list with `items`.
     *
     * This is the **only** writer entry point on Mgr; Logger::Initialize()
     * calls it once with the parsed config, and LoggerMgr::Reload() calls it
     * again with the re-parsed config. There is no incremental add/remove API
     * by design — the control list is configuration-only.
     *
     * Semantics:
     *  - All items are validated first (by building executors); if any item
     *    is invalid, nothing is changed (strong exception-safety wrt the
     *    published snapshot) and LLBC_FAILED is returned with
     *    LLBC_ERROR_INVALID set.
     *  - On success a single new snapshot is atomically published, so any
     *    in-flight Apply() either sees the old snapshot in full or the new
     *    snapshot in full — never a mixed/empty intermediate state.
     *  - Passing an empty vector publishes an empty snapshot (IsEmpty() will
     *    return true and the hot-path skips the chain entirely).
     *  - The suppressed count is intentionally preserved across SetControls()
     *    calls; call ResetSuppressedCount() explicitly for a clean baseline.
     *
     * @param[in] items - the new full list of control items.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetControls(const std::vector<LLBC_LogControlItem> &items);

    /**
     * Get installed control item count.
     */
    size_t GetControlCount() const;

    /**
     * Snapshot all installed control items (as configuration DTOs) in
     * declaration order. The output is cleared first.
     *
     * Note: Executors themselves are not exposed — they're an internal
     * implementation type and callers only need the original configuration
     * back (e.g. for serialization / display).
     *
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
     * Apply executors to one (log record, appender) pair.
     *
     * Chain-merge semantics
     * ---------------------
     * The returned ApplyResult is the **merged** outcome of evaluating every
     * matching executor in declaration order; it is NOT a per-executor list.
     * The merge rules are intentionally simple and write-through:
     *
     *   - SetLevel  : `effectiveLevel` is overwritten by the last-matching
     *                 SetLevel executor in declaration order. Earlier
     *                 SetLevel writes ARE visible to later executors' level
     *                 rules (Match() is evaluated against the running
     *                 `effectiveLevel`), so chains like
     *                     [SetLevel(Warn) → match level==Warn → Mute]
     *                 are observable and intentional.
     *   - Mute      : short-circuits the chain. `muted` is set to true,
     *                 `effectiveLevel` keeps whatever value it had at that
     *                 point, the suppression counter is bumped exactly once
     *                 per (record, appender), and remaining executors are
     *                 NOT evaluated (their match status is unobservable to
     *                 the rest of the system, by design — a dropped record
     *                 has no observable level).
     *   - No match  : `muted=false`, `effectiveLevel==originalLevel`.
     *
     * The single-result shape is deliberate: callers only need "did this
     * record survive on this appender, and at which level?". If you ever
     * need "which executor fired", add a separate verbose API rather than
     * widening ApplyResult — the current shape is on the hot path.
     *
     * @param[in] appenderType - the appender type currently being driven,
     *                           see LLBC_LogAppenderType.
     * @param[in] data         - the log record context. `data.file` /
     *                           `data.func` / `data.line` / `data.threadId`
     *                           are consumed by match rules. `data.level` is
     *                           NOT used as the level-rule input — pass the
     *                           original level via `originalLevel` instead;
     *                           this avoids relying on whether the caller
     *                           has temporarily rewritten `data.level` for
     *                           per-appender output.
     * @param[in] originalLevel - the record's original (pre-chain) level.
     *                            Used both as the seed of the returned
     *                            effectiveLevel and as the level-rule input
     *                            for the first executor; SetLevel actions
     *                            rewrite it for subsequent executors in this
     *                            same chain only.
     * @return ApplyResult - merged {muted, effectiveLevel}.
     */
    ApplyResult Apply(int appenderType,
                      const LLBC_LogData &data,
                      int originalLevel) const;

private:
    // Writers serialize among themselves via this lock; readers do NOT take it.
    mutable LLBC_SpinLock _writeLock;

    // Immutable snapshot of installed executors. Writers publish a new
    // shared_ptr via std::atomic_store; readers consume via std::atomic_load.
    // The pointed vector is `const`: once published, it's never mutated. This
    // gives readers a stable snapshot for the duration of one Apply() with
    // zero locking. Each element is a unique_ptr<Executor> so the executor's
    // identity (and address) is stable across snapshots until removed.
    std::shared_ptr<const ExecutorList> _execs;

    // Lock-free fast-path: true iff the published _execs snapshot is empty.
    // Written under _writeLock after a successful publish; read lock-free via
    // IsEmpty(). Same caveats as the historical `volatile bool` style usage in
    // llbc (eventual visibility is sufficient for log filtering).
    volatile bool _empty;

    // Total number of records dropped by mute action, counted per (record, appender).
    mutable std::atomic<uint64> _suppressedCount;
};

__LLBC_NS_END
