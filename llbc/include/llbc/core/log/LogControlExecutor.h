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
#include "llbc/core/log/LogControl.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
struct LLBC_LogData;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief Result of applying one (or a chain of) log control executor(s) to a
 *        (log record, appender) pair.
 *
 * - muted          : true iff the record is dropped on this appender by the
 *                    first hitting Mute executor (short-circuits the chain).
 * - effectiveLevel : level after possibly chained SetLevel actions. Subsequent
 *                    executors observe the rewritten level, matching the
 *                    declaration-order pipeline semantics.
 */
struct LLBC_LogControlApplyResult
{
    bool muted;
    int effectiveLevel;
};

/**
 * \brief Runtime executor of a single log control item.
 *
 * Why a dedicated class
 * ---------------------
 * `LLBC_LogControlItem` is a pure configuration DTO (what to match + what to
 * do); `LLBC_LogControlMgr` is the chain scheduler (ordered traversal, COW
 * snapshot publish, suppression accounting). The actual "match this item /
 * apply this action" logic is the third concern and used to be inlined in
 * Mgr::Apply as free functions. Lifting it into its own class:
 *
 *   - gives each control item a single, testable runtime object;
 *   - lets Mgr's main loop read as a clean pipeline:
 *         for (exec : snapshot) if (exec->Match(...)) exec->Action(r);
 *   - centralizes validation (Validate happens once at construction so an
 *     installed executor is by definition well-formed);
 *   - leaves room for future per-item optimizations (regex/glob compile,
 *     precomputed hash sets, etc.) without leaking into Mgr.
 *
 * Lifetime
 * --------
 * Immutable after construction. The configured `LLBC_LogControlItem` is
 * copied/moved in once and never mutated; Mgr stores executors via shared
 * snapshot (shared_ptr<const vector<unique_ptr<Executor>>>) so concurrent
 * Apply() callers always see a stable graph.
 *
 * Thread-safety
 * -------------
 * `Match()` / `Action()` / `GetItem()` are const and reentrant. Suppression
 * counting is intentionally NOT done here — Mgr owns the global counter
 * because suppression is a chain-level signal (the same record counted once
 * per appender, not once per executor).
 */
class LLBC_HIDDEN LLBC_LogControlExecutor
{
public:
    using ApplyResult = LLBC_LogControlApplyResult;

public:
    /**
     * Validate `item` and build an executor.
     *
     * Follows the llbc factory convention (cf. LLBC_BasePoller::Create,
     * LLBC_Service::Create, LLBC_IProtocolFactory::Create): returns a raw
     * owning pointer on success, nullptr on failure with LLBC_SetLastError
     * already set. Callers transfer ownership into whatever container they
     * use (Mgr stores executors in std::unique_ptr<Executor> inside its COW
     * snapshot vector).
     *
     * Centralizing validation here (instead of in Mgr::SetControls) guarantees
     * an installed executor is always well-formed; Mgr no longer needs to
     * recheck on the hot path.
     *
     * @param[in] item - configured control item (copied into the executor).
     * @return LLBC_LogControlExecutor * - newly constructed executor on success;
     *                                     nullptr on validation failure
     *                                     (LLBC_ERROR_INVALID is set).
     */
    static LLBC_LogControlExecutor *Create(const LLBC_LogControlItem &item);

    ~LLBC_LogControlExecutor() = default;

    // Non-copyable / non-movable: executors live in unique_ptr inside Mgr's
    // immutable snapshot; identity matters, copies don't.
    LLBC_LogControlExecutor(const LLBC_LogControlExecutor &) = delete;
    LLBC_LogControlExecutor &operator=(const LLBC_LogControlExecutor &) = delete;

public:
    /**
     * Test whether this executor is in-scope for and matches the given
     * (record, appender) pair.
     *
     * Combined semantics in one call:
     *   1) appender scope (item.appenderTypes empty == all);
     *   2) match-rules: enabled rules AND-combined, each rule's value-set
     *      OR-combined internally.
     *
     * Scope and match are fused here on purpose: from Mgr's view "does this
     * executor fire on this (record, appender)?" is one question; splitting it
     * would force Mgr to know about both notions to compose them correctly.
     *
     * Why split `currentLevel` from `data`
     * ------------------------------------
     * `data` is the immutable record context (file / func / line / threadId /
     * original level), while `currentLevel` is the chain-accumulated effective
     * level — earlier SetLevel executors may have rewritten it. Keeping the
     * two separate makes the input model explicit: `data` is "what the user
     * logged", `currentLevel` is "what the chain has decided so far". The
     * level rule is evaluated against `currentLevel`, not `data.level`.
     *
     * @param[in] appenderType - the appender currently being driven.
     * @param[in] data         - the log record (file / func / line / threadId
     *                           are consumed; `data.level` is intentionally
     *                           ignored — see `currentLevel`).
     * @param[in] currentLevel - current effective level (already possibly
     *                           rewritten by earlier SetLevel executors).
     */
    bool Match(int appenderType,
               const LLBC_LogData &data,
               int currentLevel) const;

    /**
     * Apply this executor's action onto `io`.
     *
     * Caller is responsible for having already passed `Match()` — Action()
     * does no re-check, by design (Mgr's pipeline never calls Action without
     * a positive Match, and re-checking would double the per-record cost).
     *
     * - Mute    : sets io.muted = true. Caller (Mgr) is expected to break the
     *             chain and bump the suppression counter.
     * - SetLevel: writes item.newLevel into io.effectiveLevel; subsequent
     *             executors observe the rewritten level.
     */
    void Action(ApplyResult &io) const;

    /**
     * Read-only access to the underlying configuration item. Used by Mgr to
     * implement GetControls() snapshot export.
     */
    const LLBC_LogControlItem &GetItem() const { return _item; }

private:
    explicit LLBC_LogControlExecutor(const LLBC_LogControlItem &item);

    /**
     * Validate the item under the same rules historically enforced by
     * LLBC_LogControlMgr::ValidateItem(). Pure check; does not mutate state.
     * Sets LLBC_ERROR_INVALID on failure.
     */
    static int Validate(const LLBC_LogControlItem &item);

private:
    LLBC_LogControlItem _item;
};

__LLBC_NS_END
