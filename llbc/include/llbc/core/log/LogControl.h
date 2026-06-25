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

/**
 * \brief Log control item match-rule kinds.
 *
 * Within one control item the configured match-rules are combined with AND
 * (all-of): the item matches a log record only if ALL of its enabled rules
 * match. Each rule's own value-set (e.g. matchFuncs, matchThreadIds,
 * matchLevels, matchLineRanges) is OR-combined (one-of) internally.
 *
 * Rationale: AND-between-rules matches the engineering intuition of a filter
 * (intersection of constraints) and is consistent with how iptables / log4j
 * filter / SQL WHERE / Prometheus matchers compose multi-field predicates.
 * To express OR between top-level dimensions, install multiple control items.
 *
 * Different control items are independent, applied by declaration order.
 */
class LLBC_LogControlMatchType
{
public:
    enum
    {
        Begin,

        File = Begin,   // match by file (basename) + optional line(s).
                        // matchLineRanges holds zero-or-more half-open ranges
                        // [begin, end); a single line N is stored as [N, N+1).
                        // Empty matchLineRanges means "any line of the file"
                        // (whole-file wildcard).
        Func,           // match by function name; matches if record's function
                        // equals ANY of matchFuncs (exact, no wildcard).
        ThreadId,       // match by native thread id; matches if record's tid
                        // equals ANY of matchThreadIds.
        Level,          // match by log level; matches if record's level equals
                        // ANY of matchLevels.

        End
    };

    /**
     * Check given match type is valid or not.
     */
    static constexpr bool IsValid(int matchType)
    {
        return matchType >= Begin && matchType < End;
    }
};

/**
 * \brief Log control item action kinds.
 *
 * - Mute    : drop the matched log record (only on the appenders specified by
 *             the control item; if appenderTypes is empty, drop on every appender).
 * - SetLevel: rewrite the matched log record's level to a new level. The
 *             rewrite is observable to subsequent control items in the chain
 *             (so a later item can match against the new level).
 */
class LLBC_LogControlAction
{
public:
    enum
    {
        Unset = 0,      // sentinel: action not explicitly set.
                        // LLBC_LogControlItem default-constructs `action` to
                        // Unset; SetControls() rejects it via IsValid(). This
                        // forces callers to set action explicitly instead of
                        // silently muting (Mute is destructive — losing logs
                        // on a typo / missed assignment is a silent failure).
        Begin,

        Mute = Begin,   // drop the matched record (on the configured appenders).
        SetLevel,       // rewrite log level to <newLevel>.

        End
    };

    static constexpr bool IsValid(int action)
    {
        return action >= Begin && action < End;
    }
};

/**
 * \brief A single log output control item.
 *
 * Three parts:
 *   a) Match rules: zero-or-more of {File, Func, ThreadId, Level}.
 *      The enabled rules are combined with **AND** (intersection); each rule's
 *      own value-set (matchLineRanges / matchFuncs / matchThreadIds /
 *      matchLevels) is OR-combined (one-of) internally.
 *      An empty rule set means "match everything" and is rejected on validation.
 *   b) Appender scope: a set of appender types (LLBC_LogAppenderType::*); empty means all.
 *   c) Action: Mute or SetLevel(+newLevel).
 */
struct LLBC_LogControlItem
{
public:
    // a) match rules. The enabled rules are AND-combined (all-of); each rule's
    //    value-set below is OR-combined (one-of).
    //    Each "haveXxx" flag enables the corresponding rule.
    bool haveFile;
    LLBC_String matchFile;     // file basename, valid when haveFile.
    std::vector<std::pair<int, int> > matchLineRanges; // half-open [begin, end) segments.
                               // A single line N is stored as [N, N+1). Empty
                               // means wildcard (any line of matchFile). Each
                               // segment must satisfy 0 <= begin && end > begin.
                               // The record's line matches if it lies in ANY
                               // segment (segments are OR-combined).

    bool haveFunc;
    std::vector<LLBC_String> matchFuncs; // valid when haveFunc; non-empty.
                                         // Matches if the record's function name
                                         // equals ANY entry (exact, OR-combined).

    bool haveThreadId;
    std::vector<LLBC_ThreadId> matchThreadIds; // valid when haveThreadId; non-empty.
                                               // Matches if the record's tid
                                               // equals ANY entry (OR-combined).

    bool haveLevel;
    std::vector<int> matchLevels; // valid when haveLevel; non-empty; each entry
                                  // in LLBC_LogLevel::Begin..<End. Matches if
                                  // the record's level equals ANY entry
                                  // (OR-combined).

    // b) appender scope; empty == all appenders.
    std::vector<int> appenderTypes;

    // c) action.
    int action;                // LLBC_LogControlAction::Mute / SetLevel.
                               // Default-constructed to Unset; SetControls()
                               // rejects an item whose action wasn't set.
    int newLevel;              // valid when action == SetLevel.

public:
    LLBC_LogControlItem();

    /**
     * Check at least one match rule is enabled (i.e. non-empty match).
     */
    bool HasAnyMatch() const
    {
        return haveFile || haveFunc || haveThreadId || haveLevel;
    }
};

__LLBC_NS_END

#include "llbc/core/log/LogControlInl.h"
