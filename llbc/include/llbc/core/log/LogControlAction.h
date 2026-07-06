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

__LLBC_NS_BEGIN

// Pre-declare.
struct LLBC_LogControlApplyResult;
struct LLBC_LogControlItem;

/**
 * \brief Log control action base class.
 *
 * Polymorphic counterpart of LLBC_LogControlItem::action. Immutable after
 * Initialize(); Apply() is const and reentrant. Owned by the executor.
 */
class LLBC_HIDDEN LLBC_BaseLogControlAction
{
public:
    using ApplyResult = LLBC_LogControlApplyResult;

    virtual ~LLBC_BaseLogControlAction() = default;

    /**
     * Create & initialize the action for the given control item.
     * @param[in] item - the source control item.
     * @return LLBC_BaseLogControlAction * - the new action, or nullptr on
     *                                       unknown/unset action or init
     *                                       failure. Does NOT set LLBC_ERROR.
     */
    static LLBC_BaseLogControlAction *Create(const LLBC_LogControlItem &item);

    /**
     * Initialize from the control item (reads only the fields relevant to
     * the concrete subclass).
     * @param[in] item - the source control item.
     * @return int - LLBC_OK / LLBC_FAILED.
     */
    virtual int Initialize(const LLBC_LogControlItem &item) = 0;

    /**
     * Apply this action onto `io`. Caller has already passed Match().
     * @param[in,out] io - the cross-executor apply result to mutate
     *                     (e.g. set `muted`, rewrite effective level).
     */
    virtual void Apply(ApplyResult &io) const = 0;

protected:
    LLBC_BaseLogControlAction() = default;

    // Disable assignment.
    LLBC_DISABLE_ASSIGNMENT(LLBC_BaseLogControlAction);
};

/**
 * \brief Mute action: drop the matched record.
 *
 * Signals cut-off via `io.muted = true`; LLBC_LogControlMgr breaks the
 * executor loop and bumps the suppression counter.
 */
class LLBC_HIDDEN LLBC_LogMuteAction final : public LLBC_BaseLogControlAction
{
public:
    LLBC_LogMuteAction() = default;

    /**
     * No-op: mute action carries no payload.
     * @param[in] item - unused.
     * @return int - always LLBC_OK.
     */
    int Initialize(const LLBC_LogControlItem &item) override;

    /**
     * @param[in,out] io - sets `io.muted = true`.
     */
    void Apply(ApplyResult &io) const override;
};

/**
 * \brief SetLevel action: rewrite the chain's effective level.
 *
 * Later executors in the same chain see the rewritten level.
 */
class LLBC_HIDDEN LLBC_LogSetLevelAction final : public LLBC_BaseLogControlAction
{
public:
    LLBC_LogSetLevelAction();

    /**
     * @param[in] item - the source control item; only `newLevel` is read.
     * @return int - LLBC_FAILED if `item.newLevel` is not a valid LLBC_LogLevel.
     */
    int Initialize(const LLBC_LogControlItem &item) override;

    /**
     * @param[in,out] io - rewrites `io.effectiveLevel` to the configured newLevel.
     */
    void Apply(ApplyResult &io) const override;

private:
    int _newLevel;
};

__LLBC_NS_END
