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
 * Pre-declare.
 */
struct LLBC_LogData;

/**
 * \brief One predicate over (LogData, currentLevel).
 *
 * Polymorphic counterpart of a single enabled match-rule inside a
 * LLBC_LogControlItem; an executor owns one rule per enabled dimension
 * (file / func / threadId / level) and AND-combines their Match() results.
 * The whole hierarchy is `LLBC_HIDDEN`: only reachable from outside via
 * LLBC_LogControlExecutor::Create.
 *
 * Two-phase construction: default-construct the subclass, then call
 * Initialize(item) which reads only the rule's own slice of the DTO and
 * validates it. Dispatch (matchType -> subclass) is centralized in the
 * static Create() factory below so the executor never type-switches by hand.
 *
 * Immutable after a successful Initialize(); Match() is const and reentrant.
 * Rules are owned as raw pointers by the executor, which deletes them in
 * its destructor.
 */
class LLBC_HIDDEN LLBC_BaseLogMatchRule
{
public:
    /**
     * Virtual dtor for polymorphic deletion by the owning executor.
     */
    virtual ~LLBC_BaseLogMatchRule() = default;

    /**
     * Maps `matchType` to the corresponding per-dimension `.enabled` flag on
     * `item` (File -> item.file.enabled, and so on). Lets the executor
     * factory iterate LLBC_LogControlMatchType uniformly.
     *
     * @param[in] matchType - one of LLBC_LogControlMatchType::*.
     * @param[in] item      - the source control item.
     * @return bool - value of the corresponding `.enabled` flag; false for
     *                unknown matchType.
     */
    static bool IsEnabled(int matchType, const LLBC_LogControlItem &item);

    /**
     * Build the polymorphic match-rule object for one dimension.
     *
     * A single switch on `matchType` picks the concrete subclass, which is
     * then driven through Initialize(item) for payload validation.
     *
     * Pre-condition: `IsEnabled(matchType, item) == true` (the caller
     * iterates enabled dimensions only); nullptr therefore unambiguously
     * means "configured but invalid" at the call site.
     *
     * @param[in] matchType - one of LLBC_LogControlMatchType::*.
     * @param[in] item      - the source control item.
     * @return LLBC_BaseLogMatchRule * - a newly-allocated, fully-initialized
     *                                   rule (ownership transferred to caller)
     *                                   on success; nullptr on unknown
     *                                   matchType or Initialize() failure (the
     *                                   partially-constructed object is
     *                                   deleted internally). Does NOT set
     *                                   LLBC_ERROR; the executor factory
     *                                   aggregates the failure.
     */
    static LLBC_BaseLogMatchRule *Create(int matchType, const LLBC_LogControlItem &item);

    /**
     * @return int - which match dimension this rule implements
     *               (LLBC_LogControlMatchType::{File, Func, ThreadId, Level}).
     *               On the base class so callers can identify a rule without
     *               RTTI.
     */
    virtual int MatchType() const = 0;

    /**
     * Read the rule's own slice of the source control item and validate it.
     * The corresponding `.enabled` flag is NOT re-checked — that's the
     * factory's precondition (see Create()).
     *
     * @param[in] item - the source control item.
     * @return int - LLBC_OK on success, LLBC_FAILED on payload validation
     *               failure.
     */
    virtual int Initialize(const LLBC_LogControlItem &item) = 0;

    /**
     * Evaluate this rule against a record.
     *
     * @param[in] data         - the log record (file / func / line / threadId
     *                           consumed by concrete rules).
     * @param[in] currentLevel - effective level so far in the chain; the
     *                           Level rule consumes this (NOT data.level),
     *                           so "SetLevel then match by level" is
     *                           composable — see LLBC_LogControlExecutor::Match.
     * @return bool - true iff the rule matches.
     */
    virtual bool Match(const LLBC_LogData &data, int currentLevel) const = 0;

protected:
    /**
     * Default ctor; subclass fields are filled by Initialize().
     */
    LLBC_BaseLogMatchRule() = default;

    // Non-copyable: rules are owned as raw pointers by the executor;
    // copying would bypass the factory contract and confuse ownership.
    LLBC_DISABLE_ASSIGNMENT(LLBC_BaseLogMatchRule);
};

/**
 * \brief Match by source file basename plus optional line-range set.
 * See LLBC_LogFileMatch (LogControl.h) for the range/wildcard semantics.
 */
class LLBC_HIDDEN LLBC_LogFileMatchRule final : public LLBC_BaseLogMatchRule
{
public:
    /**
     * Default ctor; matched file + line-ranges are filled by Initialize().
     */
    LLBC_LogFileMatchRule() = default;

    /**
     * @return int - LLBC_LogControlMatchType::File.
     */
    int MatchType() const override { return LLBC_LogControlMatchType::File; }

    /**
     * Read `item.file` and validate it (non-empty file; each line-range is a
     * valid half-open [begin, end) with 0 <= begin < end).
     * @param[in] item - the source control item.
     * @return int - LLBC_OK on success, LLBC_FAILED on payload validation
     *               failure.
     */
    int Initialize(const LLBC_LogControlItem &item) override;

    /**
     * Match by source file basename (equal to `_matchFile`) plus optional
     * line-ranges (if `_matchLineRanges` is non-empty, `data.line` must lie
     * in ANY half-open [begin, end) segment).
     * @param[in] data         - the log record; consumes `data.file` and `data.line`.
     * @param[in] currentLevel - the chain-rewritten level (unused by this rule).
     * @return bool - true iff the rule matches.
     */
    bool Match(const LLBC_LogData &data, int currentLevel) const override;

private:
    LLBC_String _matchFile;
    std::vector<std::pair<int, int> > _matchLineRanges;
};

/**
 * \brief Match by function name (exact, OR-set).
 */
class LLBC_HIDDEN LLBC_LogFuncMatchRule final : public LLBC_BaseLogMatchRule
{
public:
    /**
     * Default ctor; matched func names are filled by Initialize().
     */
    LLBC_LogFuncMatchRule() = default;

    /**
     * @return int - LLBC_LogControlMatchType::Func.
     */
    int MatchType() const override { return LLBC_LogControlMatchType::Func; }

    /**
     * Read `item.func` and validate it (non-empty value list; each entry
     * non-empty).
     * @param[in] item - the source control item.
     * @return int - LLBC_OK on success, LLBC_FAILED on payload validation
     *               failure.
     */
    int Initialize(const LLBC_LogControlItem &item) override;

    /**
     * Match iff `data.func` equals ANY entry of `_matchFuncs` (exact,
     * case-sensitive).
     * @param[in] data         - the log record; consumes `data.func`.
     * @param[in] currentLevel - the chain-rewritten level (unused by this rule).
     * @return bool - true iff the rule matches.
     */
    bool Match(const LLBC_LogData &data, int currentLevel) const override;

private:
    std::vector<LLBC_String> _matchFuncs;
};

/**
 * \brief Match by native thread id (OR-set).
 */
class LLBC_HIDDEN LLBC_LogThreadIdMatchRule final : public LLBC_BaseLogMatchRule
{
public:
    /**
     * Default ctor; matched thread ids are filled by Initialize().
     */
    LLBC_LogThreadIdMatchRule() = default;

    /**
     * @return int - LLBC_LogControlMatchType::ThreadId.
     */
    int MatchType() const override { return LLBC_LogControlMatchType::ThreadId; }

    /**
     * Read `item.threadId` and validate it (non-empty value list).
     * @param[in] item - the source control item.
     * @return int - LLBC_OK on success, LLBC_FAILED on payload validation
     *               failure.
     */
    int Initialize(const LLBC_LogControlItem &item) override;

    /**
     * Match iff `data.threadId` equals ANY entry of `_matchThreadIds`.
     * @param[in] data         - the log record; consumes `data.threadId`.
     * @param[in] currentLevel - the chain-rewritten level (unused by this rule).
     * @return bool - true iff the rule matches.
     */
    bool Match(const LLBC_LogData &data, int currentLevel) const override;

private:
    std::vector<LLBC_ThreadId> _matchThreadIds;
};

/**
 * \brief Match by current effective level (OR-set).
 */
class LLBC_HIDDEN LLBC_LogLevelMatchRule final : public LLBC_BaseLogMatchRule
{
public:
    /**
     * Default ctor; matched levels are filled by Initialize().
     */
    LLBC_LogLevelMatchRule() = default;

    /**
     * @return int - LLBC_LogControlMatchType::Level.
     */
    int MatchType() const override { return LLBC_LogControlMatchType::Level; }

    /**
     * Read `item.level` and validate it (non-empty value list; each entry
     * a valid log level).
     * @param[in] item - the source control item.
     * @return int - LLBC_OK on success, LLBC_FAILED on payload validation
     *               failure.
     */
    int Initialize(const LLBC_LogControlItem &item) override;

    /**
     * Match iff `currentLevel` equals ANY entry of `_matchLevels`. Evaluates
     * against `currentLevel` (the chain-rewritten level), NOT `data.level`,
     * so "SetLevel then match by level" composes.
     * @param[in] data         - the log record (unused by this rule).
     * @param[in] currentLevel - the chain-rewritten level; consumed here.
     * @return bool - true iff the rule matches.
     */
    bool Match(const LLBC_LogData &data, int currentLevel) const override;

private:
    std::vector<int> _matchLevels;
};

__LLBC_NS_END
