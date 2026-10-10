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

// Pre-declare.
struct LLBC_LogData;
class LLBC_BaseLogMatchRule;
class LLBC_BaseLogControlAction;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief Cross-executor apply result for one (record, appender) pair.
 *
 * - muted          : set to true by a Mute action; short-circuits the chain.
 * - effectiveLevel : level after chained SetLevel actions; later executors
 *                    evaluate against this, not against `data.level`.
 */
struct LLBC_LogControlApplyResult
{
    bool muted;
    int effectiveLevel;
};

/**
 * \brief Runtime executor of a single log control item.
 *
 * Compiles a configuration DTO (LLBC_LogControlItem) into its polymorphic
 * runtime form: a set of AND-combined match rules plus one action.
 * Immutable after construction; owned by LLBC_LogControlMgr's snapshot.
 * Match() / Action() / GetItem() are const and reentrant.
 */
class LLBC_HIDDEN LLBC_LogControlExecutor
{
public:
    using ApplyResult = LLBC_LogControlApplyResult;

public:
    /**
     * Validate `item` and build an executor. Translates the DTO into its
     * polymorphic form once so the hot path never re-validates.
     *
     * @param[in] item - the source control item (copied into the executor).
     * @return LLBC_LogControlExecutor * - the new executor on success
     *                                     (ownership transferred); nullptr
     *                                     on validation failure with
     *                                     LLBC_ERROR_INVALID set.
     */
    static LLBC_LogControlExecutor *Create(const LLBC_LogControlItem &item);

    /**
     * Destructor. Deletes every owned match rule and the action.
     */
    ~LLBC_LogControlExecutor();

    // Executors live inside Mgr's immutable snapshot; identity matters,
    // copies don't.
    LLBC_DISABLE_ASSIGNMENT(LLBC_LogControlExecutor);

public:
    /**
     * Test whether this executor is in-scope for and matches the given
     * (record, appender) pair. Fuses two checks:
     *   1) appender scope (item.appenderTypes empty == all);
     *   2) all owned match rules return true (AND-combined; each rule's
     *      own value-set is OR-combined internally).
     *
     * @param[in] appenderType - the appender currently being driven.
     * @param[in] data         - the log record; `data.level` is ignored
     *                           in favour of `currentLevel`.
     * @param[in] currentLevel - chain-accumulated effective level (possibly
     *                           rewritten by earlier SetLevel executors);
     *                           the level rule evaluates against this.
     * @return bool - true iff both appender scope and every rule match.
     */
    bool Match(int appenderType,
               const LLBC_LogData &data,
               int currentLevel) const;

    /**
     * Apply this executor's action onto `io`. Caller must have already
     * passed Match() — Action() does no re-check.
     * @param[in,out] io - cross-executor apply result to mutate.
     */
    void Action(ApplyResult &io) const;

    /**
     * @return const LLBC_LogControlItem & - the original configuration item.
     */
    const LLBC_LogControlItem &GetItem() const { return _item; }

private:
    // Adopts ownership of every rule in `rules` and of `action`. `rules`
    // is passed by value (consumed) so the caller's builder vector is
    // emptied on entry — preventing accidental double-delete.
    LLBC_LogControlExecutor(
        const LLBC_LogControlItem &item,
        std::vector<LLBC_BaseLogMatchRule *> rules,
        LLBC_BaseLogControlAction *action);

    // Failure-path helper for Create(): deletes any already-built
    // rules/action, sets LLBC_ERROR_INVALID, and returns nullptr.
    static LLBC_LogControlExecutor *FailCreate(
        std::vector<LLBC_BaseLogMatchRule *> &rules,
        LLBC_BaseLogControlAction *&action);

private:
    // Original DTO, kept for GetItem() round-trip.
    LLBC_LogControlItem _item;

    // AND-combined match rules; never empty. Owned (deleted in dtor).
    std::vector<LLBC_BaseLogMatchRule *> _rules;

    // Exactly one action; never null. Owned (deleted in dtor).
    LLBC_BaseLogControlAction *_action;
};

__LLBC_NS_END
