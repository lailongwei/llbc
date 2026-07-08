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
 * Within one control item, enabled rules are AND-combined (all-of); each
 * rule's own value-set (e.g. func.values, level.values, file.lineRanges)
 * is OR-combined (one-of) internally. To express OR between top-level
 * dimensions, install multiple control items.
 */
class LLBC_LogControlMatchType
{
public:
    enum
    {
        Begin,

        File = Begin,   // match by file (basename) + optional line(s).
                        // file.lineRanges holds zero-or-more half-open ranges
                        // [begin, end); a single line N is stored as [N, N+1).
                        // Empty lineRanges means "any line of file.file"
                        // (whole-file wildcard).
        Func,           // match by function name; matches if record's function
                        // equals ANY of func.values (exact, no wildcard).
        ThreadId,       // match by native thread id; matches if record's tid
                        // equals ANY of threadId.values.
        Level,          // match by log level; matches if record's level equals
                        // ANY of level.values.

        End
    };

    /**
     * @param[in] matchType - candidate match-rule kind.
     * @return bool - true iff `matchType` is in [Begin, End).
     */
    static constexpr bool IsValid(int matchType)
    {
        return matchType >= Begin && matchType < End;
    }
};

/**
 * \brief Log control item action kinds.
 *
 * - Mute    : drop the matched log record (on the configured appenders;
 *             if appenderTypes is empty, drop on every appender).
 * - SetLevel: rewrite the matched log record's level to `newLevel`. The
 *             rewrite is observable to subsequent control items in the
 *             chain (so a later item can match against the new level).
 */
class LLBC_LogControlAction
{
public:
    enum
    {
        Unset = 0,      // sentinel: action must be set explicitly. Items
                        // default-construct `action` to Unset and are
                        // rejected by SetControls() until overwritten,
                        // so a missed assignment fails loudly rather than
                        // silently defaulting to the destructive Mute.
        Begin,

        Mute = Begin,   // drop the matched record (on the configured appenders).
        SetLevel,       // rewrite log level to <newLevel>.

        End
    };

    /**
     * @param[in] action - candidate action kind.
     * @return bool - true iff `action` is in [Begin, End); Unset is invalid.
     */
    static constexpr bool IsValid(int action)
    {
        return action >= Begin && action < End;
    }
};

/**
 * \brief A single OR-combined value-set for a match rule.
 *
 * Pairs an enable flag with the rule's value-set. Values are OR-combined
 * inside the rule (record matches iff its field equals ANY entry of
 * `values`); different rules within an item are AND-combined at the
 * executor level.
 *
 * Invariants when `enabled == true`:
 *   - `values` is non-empty;
 *   - per-rule payload constraints hold (e.g. each level entry must be a
 *     valid LLBC_LogLevel; validated at executor construction).
 */
template <typename T>
struct LLBC_LogMatchSet
{
    bool enabled;
    std::vector<T> values;

    LLBC_LogMatchSet(): enabled(false) {}
};

/**
 * \brief Match clause for the File rule (file basename + line ranges).
 *
 * Each range is half-open [begin, end) with 0 <= begin < end; a single
 * line N is stored as [N, N+1). An empty `lineRanges` means "any line
 * of `file`" (whole-file wildcard). The record's line matches if it
 * lies in ANY segment (OR-combined inside the rule).
 */
struct LLBC_LogFileMatch
{
    bool enabled;
    LLBC_String file;                                  // file basename, valid when enabled.
    std::vector<std::pair<int, int> > lineRanges;      // half-open [begin, end) segments.

    LLBC_LogFileMatch(): enabled(false) {}
};

/**
 * \brief A single log output control item.
 *
 * Three parts:
 *   a) Match rules: zero-or-more of {File, Func, ThreadId, Level}. See
 *      LLBC_LogControlMatchType for AND/OR combination semantics. An empty
 *      rule set means "match everything" and is rejected on validation.
 *   b) Appender scope: a set of LLBC_LogAppenderType values; empty == all.
 *   c) Action: Mute or SetLevel(+newLevel). See LLBC_LogControlAction.
 */
struct LLBC_LogControlItem
{
public:
    // a) match rules; AND-combined when enabled. Each rule carries its own
    //    enable flag (file.enabled / func.enabled / threadId.enabled /
    //    level.enabled).
    LLBC_LogFileMatch                   file;       // File rule (basename + line ranges).
    LLBC_LogMatchSet<LLBC_String>       func;       // Func rule; values: function names (exact).
    LLBC_LogMatchSet<LLBC_ThreadId>     threadId;   // ThreadId rule; values: native tids.
    LLBC_LogMatchSet<int>               level;      // Level rule; values: each in LLBC_LogLevel::Begin..<End.

    // b) appender scope; empty == all appenders.
    std::vector<int> appenderTypes;

    // c) action; default-constructed to Unset (rejected by SetControls()
    //    until set explicitly — see LLBC_LogControlAction::Unset).
    int action;                // LLBC_LogControlAction::Mute / SetLevel.
    int newLevel;              // valid when action == SetLevel.

public:
    LLBC_LogControlItem();

    /**
     * @return bool - true iff at least one match rule is enabled (i.e. a
     *                non-empty match; empty match is a config error).
     */
    bool HasAnyMatch() const
    {
        return file.enabled || func.enabled || threadId.enabled || level.enabled;
    }
};

__LLBC_NS_END

#include "llbc/core/log/LogControlInl.h"
