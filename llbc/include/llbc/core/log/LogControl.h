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
 * Within one control item the configured match-rules are combined with OR
 * (one-of): the item matches a log record if ANY of its enabled rules matches.
 * Different control items are independent, applied by declaration order.
 */
class LLBC_LogControlMatchType
{
public:
    enum
    {
        Begin,

        File = Begin,   // match by file (basename) + optional line / line range.
                        // - line == 0 (and lineEnd == 0) : any line of the file.
                        // - lineEnd == 0 : single line, match line == matchLine.
                        // - lineEnd  > 0 : half-open range [matchLine, matchLineEnd),
                        //                  match matchLine <= line < matchLineEnd.
        Func,           // match by function name (exact match).
        ThreadId,       // match by native thread id.
        Level,          // match by log level (==).

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
 *   a) Match rules: zero-or-more of {File, Func, ThreadId, Level}, combined by OR.
 *      An empty rule set means "match everything" and is rejected on validation.
 *   b) Appender scope: a set of appender types (LLBC_LogAppenderType::*); empty means all.
 *   c) Action: Mute or SetLevel(+newLevel).
 */
struct LLBC_LogControlItem
{
public:
    // a) match rules (one-of). Each "haveXxx" flag enables the corresponding rule.
    bool haveFile;
    LLBC_String matchFile;     // file basename, valid when haveFile.
    int matchLine;             // line number (or range begin); 0 means wildcard. Valid when haveFile.
    int matchLineEnd;          // optional half-open range end, exclusive (i.e. line < matchLineEnd).
                               //   0 == disabled (single-line / wildcard mode, see matchLine).
                               //   > 0 with matchLine >= 0: range mode, match matchLine <= line < matchLineEnd.

    bool haveFunc;
    LLBC_String matchFunc;     // valid when haveFunc.

    bool haveThreadId;
    LLBC_ThreadId matchThreadId; // valid when haveThreadId.

    bool haveLevel;
    int matchLevel;            // valid when haveLevel; LLBC_LogLevel::Begin..<End.

    // b) appender scope; empty == all appenders.
    std::vector<int> appenderTypes;

    // c) action.
    int action;                // LLBC_LogControlAction::Mute / SetLevel.
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
