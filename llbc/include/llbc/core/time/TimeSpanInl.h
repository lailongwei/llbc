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

#include "llbc/core/time/TimeConst.h"

inline std::ostream &operator<<(std::ostream &stream, const LLBC_NS LLBC_TimeSpan &span)
{
    return stream <<span.ToString();
}

__LLBC_NS_BEGIN

inline LLBC_TimeSpan LLBC_TimeSpan::FromDays(int days, int hours, int minutes, int seconds, int millis, int micros)
{
    return LLBC_TimeSpan(days * LLBC_TimeConst::numOfMicrosPerDay +
                         hours * LLBC_TimeConst::numOfMicrosPerHour +
                         minutes * LLBC_TimeConst::numOfMicrosPerMinute +
                         seconds * LLBC_TimeConst::numOfMicrosPerSecond +
                         millis * LLBC_TimeConst::numOfMicrosPerMillisecond +
                         micros);
}

inline LLBC_TimeSpan LLBC_TimeSpan::FromHours(int hours, int minutes, int seconds, int millis, int micros)
{
    return LLBC_TimeSpan(hours * LLBC_TimeConst::numOfMicrosPerHour +
                         minutes * LLBC_TimeConst::numOfMicrosPerMinute +
                         seconds * LLBC_TimeConst::numOfMicrosPerSecond +
                         millis * LLBC_TimeConst::numOfMicrosPerMillisecond +
                         micros);
}

inline LLBC_TimeSpan LLBC_TimeSpan::FromMinutes(int minutes, int seconds, int millis, int micros)
{
    return LLBC_TimeSpan(minutes * LLBC_TimeConst::numOfMicrosPerMinute +
                         seconds * LLBC_TimeConst::numOfMicrosPerSecond +
                         millis * LLBC_TimeConst::numOfMicrosPerMillisecond +
                         micros);
}

inline LLBC_TimeSpan LLBC_TimeSpan::FromSeconds(int seconds, int millis, int micros)
{
    return LLBC_TimeSpan(seconds * LLBC_TimeConst::numOfMicrosPerSecond +
                         millis * LLBC_TimeConst::numOfMicrosPerMillisecond +
                         micros);
}

inline LLBC_TimeSpan LLBC_TimeSpan::FromMillis(sint64 millis, sint64 micros)
{
    return LLBC_TimeSpan(millis * LLBC_TimeConst::numOfMicrosPerMillisecond +
                         micros);
}

inline LLBC_TimeSpan LLBC_TimeSpan::FromMicros(sint64 micros)
{
    return LLBC_TimeSpan(micros);
}

template <size_t _StrArrLen>
LLBC_FORCE_INLINE LLBC_TimeSpan LLBC_TimeSpan::FromSpanStr(const char (&spanStr)[_StrArrLen])
{
    return LLBC_TimeSpan(spanStr, _StrArrLen);
}

LLBC_FORCE_INLINE LLBC_TimeSpan LLBC_TimeSpan::FromSpanStr(const char *spanStr, size_t spanStrLen)
{
    return LLBC_TimeSpan(spanStr, spanStrLen);
}

template <typename _StrType>
LLBC_FORCE_INLINE typename std::enable_if<LLBC_IsTemplSpec<_StrType, std::basic_string>::value, LLBC_TimeSpan>::type
LLBC_TimeSpan::FromSpanStr(const _StrType &spanStr)
{
    return LLBC_TimeSpan(spanStr.c_str(), spanStr.size());
}

LLBC_FORCE_INLINE LLBC_TimeSpan::LLBC_TimeSpan(const sint64 &spanInMicros)
: _span(spanInMicros)
{
}

template <size_t _StrArrLen>
LLBC_FORCE_INLINE LLBC_TimeSpan::LLBC_TimeSpan(const char (&spanStr)[_StrArrLen])
: LLBC_TimeSpan(spanStr, _StrArrLen > 0 ? _StrArrLen - 1 : 0)
{
}

#define __LLBC_INL_TIME_SPAN_STR_PART_TO_SPAN(partStr, partLen, spanFactor) \
    if (UNLIKELY(partLen >= sizeof(numFmtBuf)))                             \
    {                                                                       \
        _span = 0;                                                          \
        return;                                                             \
    }                                                                       \
    memcpy(numFmtBuf, partStr, partLen);                                    \
    numFmtBuf[partLen] = '\0';                                              \
    _span += atoi(numFmtBuf) * spanFactor;                                  \

LLBC_FORCE_INLINE LLBC_TimeSpan::LLBC_TimeSpan(const char *spanStr, size_t spanStrLen)
: _span(0)
{
    // Argument check.
    if (UNLIKELY(!spanStr || spanStrLen == 0))
        return;

    // Remove leading whitespace characters.
    const char *origSpanStr = spanStr;
    while (*spanStr != '\0')
    {
        LLBC_BreakIf(LIKELY(!LLBC_IsSpace(*spanStr)));
        ++spanStr;
    }

    if (*spanStr == '\0')
        return;
    spanStrLen -= static_cast<size_t>(spanStr - origSpanStr);

    // Remove trailing whitespace characters.
    const char *spanStrEnd = spanStr + spanStrLen - 1;
    while (spanStrEnd != spanStr && LLBC_IsSpace(*spanStrEnd))
        --spanStrEnd;

    // spanStrEnd == spanStr is always false.
    // if (spanStrEnd == spanStr)
    //     return;
    spanStrLen = static_cast<size_t>(spanStrEnd + 1 - spanStr);

    // Parse time span string, format:
    // [DD][ [[[HH:]MM:]SS[.micro_sec]]
    // eg:               DD       HH       MM   SS   micro_sec
    // 03 =>                                     3 +     0      seconds
    // 02:03 =>                           2*60 + 3 +     0      seconds
    // 01:02:03 =>               1*3600 + 2*60 + 3 +     0      seconds
    // 8 01:02:03 =>   8*86400 + 1*3600 + 2*60 + 3 +     0      seconds
    // 8 01:02:03.4 => 8*86400 + 1*3600 + 2*60 + 3 + 4*0.000001 seconds

    // - Parse <DD> part.
    char numFmtBuf[12];
    const char *dayPartEnd = spanStr;
    while (++dayPartEnd != spanStrEnd)
        LLBC_BreakIf(LLBC_IsSpace(*dayPartEnd));
    if (dayPartEnd != spanStrEnd)
    {
        size_t dayPartLen = static_cast<size_t>(dayPartEnd - spanStr);
        __LLBC_INL_TIME_SPAN_STR_PART_TO_SPAN(spanStr, dayPartLen, LLBC_TimeConst::numOfMicrosPerDay);

        spanStr = dayPartEnd + 1;
        // spanStrLen -= (dayPartLen + 1); // <spanStrLen> variable is no longer used after this point.
    }

    // - Search all colons pos.
    size_t colonSize = 0;
    const char *colonPoses[2]{nullptr, nullptr};
    const char *strIt = spanStr;
    while (strIt != spanStrEnd)
    {
        if (*strIt == ':')
        {
            colonPoses[colonSize++] = strIt;
            if (colonSize == 2)
                break;
        }

        ++strIt;
    }

    // - Parse <HH:MM> part.
    const char *secPart;
    if (colonSize == 2)
    {
        size_t partLen = static_cast<size_t>(colonPoses[0] - spanStr);
        __LLBC_INL_TIME_SPAN_STR_PART_TO_SPAN(spanStr, partLen, LLBC_TimeConst::numOfMicrosPerHour);

        partLen = static_cast<size_t>(colonPoses[1] - colonPoses[0] - 1);
        __LLBC_INL_TIME_SPAN_STR_PART_TO_SPAN(colonPoses[0] + 1, partLen, LLBC_TimeConst::numOfMicrosPerMinute);

        secPart = colonPoses[1] + 1;
    }
    else if (colonSize == 1)
    {
        size_t partLen = static_cast<size_t>(colonPoses[0] - spanStr);
        __LLBC_INL_TIME_SPAN_STR_PART_TO_SPAN(spanStr, partLen, LLBC_TimeConst::numOfMicrosPerMinute);

        secPart = colonPoses[0] + 1;
    }
    else
    {
        secPart = spanStr;
    }

    // - Parse <SS.micro_sec> part.
    const char *dotPos = secPart;
    while (dotPos != spanStrEnd)
    {
        if (*dotPos == '.')
            break;

        ++dotPos;
    }

    const char *secPartEnd = dotPos != spanStrEnd ? dotPos : spanStrEnd;
    size_t secPartLen = secPartEnd - secPart;
    __LLBC_INL_TIME_SPAN_STR_PART_TO_SPAN(secPart, secPartLen, LLBC_TimeConst::numOfMicrosPerSecond);
    if (dotPos != spanStrEnd)
    {
        size_t microPartLen = spanStrEnd - dotPos - 1;
        __LLBC_INL_TIME_SPAN_STR_PART_TO_SPAN(dotPos + 1, microPartLen, 1);
    }
}

template <typename _StrType, typename>
LLBC_FORCE_INLINE LLBC_TimeSpan::LLBC_TimeSpan(const _StrType &spanStr)
: LLBC_TimeSpan(spanStr.c_str(), spanStr.size())
{
}

inline int LLBC_TimeSpan::GetDays() const
{
    return static_cast<int>(_span / LLBC_TimeConst::numOfMicrosPerDay);
}

inline int LLBC_TimeSpan::GetHours() const
{
    return static_cast<int>((_span % LLBC_TimeConst::numOfMicrosPerDay) / 
        LLBC_TimeConst::numOfMicrosPerHour);
}

inline int LLBC_TimeSpan::GetMinutes() const
{
    return static_cast<int>((_span % LLBC_TimeConst::numOfMicrosPerHour) /
        LLBC_TimeConst::numOfMicrosPerMinute);
}

inline int LLBC_TimeSpan::GetSeconds() const
{
    return static_cast<int>((_span % LLBC_TimeConst::numOfMicrosPerMinute) /
         LLBC_TimeConst::numOfMicrosPerSecond);
}

inline int LLBC_TimeSpan::GetMillis() const
{
    return static_cast<int>((_span % LLBC_TimeConst::numOfMicrosPerSecond) /
        LLBC_TimeConst::numOfMicrosPerMillisecond);
}

inline int LLBC_TimeSpan::GetMicros() const
{
    return static_cast<int>(_span % LLBC_TimeConst::numOfMicrosPerMillisecond);
}

inline int LLBC_TimeSpan::GetTotalDays() const
{
    return static_cast<int>(_span / LLBC_TimeConst::numOfMicrosPerDay);
}

inline int LLBC_TimeSpan::GetTotalHours() const
{
    return static_cast<int>(_span / LLBC_TimeConst::numOfMicrosPerHour);
}

inline int LLBC_TimeSpan::GetTotalMinutes() const
{
    return static_cast<int>(_span / LLBC_TimeConst::numOfMicrosPerMinute);
}

inline int LLBC_TimeSpan::GetTotalSeconds() const
{
    return static_cast<int>(_span / LLBC_TimeConst::numOfMicrosPerSecond);
}

inline sint64 LLBC_TimeSpan::GetTotalMillis() const
{
    return static_cast<sint64>(_span / LLBC_TimeConst::numOfMicrosPerMillisecond);
}

inline sint64 LLBC_TimeSpan::GetTotalMicros() const
{
    return _span;
}

inline LLBC_TimeSpan LLBC_TimeSpan::AddDays(int days)
{
    return AddMicros(LLBC_TimeConst::numOfMicrosPerDay * days);
}

inline LLBC_TimeSpan LLBC_TimeSpan::AddHours(int hours)
{
    return AddMicros(LLBC_TimeConst::numOfMicrosPerHour * hours);
}

inline LLBC_TimeSpan LLBC_TimeSpan::AddMinutes(int minutes)
{
    return AddMicros(LLBC_TimeConst::numOfMicrosPerMinute * minutes);
}

inline LLBC_TimeSpan LLBC_TimeSpan::AddSeconds(int seconds)
{
    return AddMicros(LLBC_TimeConst::numOfMicrosPerSecond * seconds);
}

inline LLBC_TimeSpan LLBC_TimeSpan::AddMillis(sint64 millis)
{
    return AddMicros(LLBC_TimeConst::numOfMicrosPerMillisecond * millis);
}

inline LLBC_TimeSpan LLBC_TimeSpan::AddMicros(sint64 micros)
{
    LLBC_TimeSpan span;
    span._span = _span + micros;

    return span;
}

inline LLBC_TimeSpan LLBC_TimeSpan::operator+(const LLBC_TimeSpan &span) const
{
    return LLBC_TimeSpan(_span + span._span);
}

inline LLBC_TimeSpan LLBC_TimeSpan::operator-(const LLBC_TimeSpan &span) const
{
    return LLBC_TimeSpan(_span - span._span);
}

inline LLBC_TimeSpan LLBC_TimeSpan::operator*(double scale) const
{
    return LLBC_TimeSpan(static_cast<sint64>(_span * scale));
}

inline LLBC_TimeSpan LLBC_TimeSpan::operator/(double scale) const
{
    return LLBC_TimeSpan(static_cast<sint64>(_span / scale));
}

inline LLBC_TimeSpan LLBC_TimeSpan::operator%(sint64 span) const
{
    return LLBC_TimeSpan(_span % span);
}

inline LLBC_TimeSpan LLBC_TimeSpan::operator%(const LLBC_TimeSpan &span) const
{
    return LLBC_TimeSpan(_span % span._span);
}

inline LLBC_TimeSpan &LLBC_TimeSpan::operator+=(const LLBC_TimeSpan &span)
{
    _span += span._span;
    return *this;
}

inline LLBC_TimeSpan &LLBC_TimeSpan::operator-=(const LLBC_TimeSpan &span)
{
    _span -= span._span;
    return *this;
}

inline LLBC_TimeSpan &LLBC_TimeSpan::operator*=(double scale)
{
    _span = static_cast<sint64>(_span * scale);
    return *this;
}

inline LLBC_TimeSpan &LLBC_TimeSpan::operator/=(double scale)
{
    _span = static_cast<sint64>(_span / scale);
    return *this;
}

inline LLBC_TimeSpan &LLBC_TimeSpan::operator%=(sint64 span)
{
    _span %= span;
    return *this;
}

inline LLBC_TimeSpan &LLBC_TimeSpan::operator%=(const LLBC_TimeSpan &span)
{
    _span %= span._span;
    return *this;
}

inline bool LLBC_TimeSpan::operator==(const LLBC_TimeSpan &span) const
{
    return _span == span._span;
}

inline bool LLBC_TimeSpan::operator!=(const LLBC_TimeSpan &span) const
{
    return !(*this == span);
}

inline bool LLBC_TimeSpan::operator<(const LLBC_TimeSpan &span) const
{
    return _span < span._span;
}

inline bool LLBC_TimeSpan::operator>(const LLBC_TimeSpan &span) const
{
    return _span > span._span;
}

inline bool LLBC_TimeSpan::operator<=(const LLBC_TimeSpan &span) const
{
    return _span<= span._span;
}

inline bool LLBC_TimeSpan::operator>=(const LLBC_TimeSpan &span) const
{
    return _span >= span._span;
}

inline LLBC_TimeSpan &LLBC_TimeSpan::operator=(const LLBC_TimeSpan &span)
{
    _span = span._span;
    return *this;
}

inline LLBC_String LLBC_TimeSpan::ToString() const
{
    int days = GetDays();
    if (days != 0)
        return LLBC_String().format("%d %02d:%02d:%02d.%06d", 
            days, GetHours(), GetMinutes(), GetSeconds(), GetMillis() * 1000 + GetMicros());
    else
        return LLBC_String().format("%02d:%02d:%02d.%06d",
            GetHours(), GetMinutes(), GetSeconds(), GetMillis() * 1000 + GetMicros());
}

inline void LLBC_TimeSpan::Serialize(LLBC_Stream &stream) const
{
    stream.Write(_span);
}

inline bool LLBC_TimeSpan::Deserialize(LLBC_Stream &stream)
{
    sint64 spanVal = 0;
    if (!stream.Read(spanVal))
        return false;

    _span = spanVal;

    return true;
}

__LLBC_NS_END
