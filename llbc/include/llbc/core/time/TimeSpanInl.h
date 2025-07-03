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

__LLBC_NS_BEGIN

constexpr LLBC_TimeSpan LLBC_TimeSpan::FromDays(int days, int hours, int minutes, int seconds, int millis, int micros)
{
    return LLBC_TimeSpan(days * LLBC_TimeConst::numOfMicrosPerDay +
                         hours * LLBC_TimeConst::numOfMicrosPerHour +
                         minutes * LLBC_TimeConst::numOfMicrosPerMinute +
                         seconds * LLBC_TimeConst::numOfMicrosPerSecond +
                         millis * LLBC_TimeConst::numOfMicrosPerMillisecond +
                         micros);
}

constexpr LLBC_TimeSpan LLBC_TimeSpan::FromHours(int hours, int minutes, int seconds, int millis, int micros)
{
    return LLBC_TimeSpan(hours * LLBC_TimeConst::numOfMicrosPerHour +
                         minutes * LLBC_TimeConst::numOfMicrosPerMinute +
                         seconds * LLBC_TimeConst::numOfMicrosPerSecond +
                         millis * LLBC_TimeConst::numOfMicrosPerMillisecond +
                         micros);
}

constexpr LLBC_TimeSpan LLBC_TimeSpan::FromMinutes(int minutes, int seconds, int millis, int micros)
{
    return LLBC_TimeSpan(minutes * LLBC_TimeConst::numOfMicrosPerMinute +
                         seconds * LLBC_TimeConst::numOfMicrosPerSecond +
                         millis * LLBC_TimeConst::numOfMicrosPerMillisecond +
                         micros);
}

constexpr LLBC_TimeSpan LLBC_TimeSpan::FromSeconds(int seconds, int millis, int micros)
{
    return LLBC_TimeSpan(seconds * LLBC_TimeConst::numOfMicrosPerSecond +
                         millis * LLBC_TimeConst::numOfMicrosPerMillisecond +
                         micros);
}

constexpr LLBC_TimeSpan LLBC_TimeSpan::FromMillis(sint64 millis, sint64 micros)
{
    return LLBC_TimeSpan(millis * LLBC_TimeConst::numOfMicrosPerMillisecond +
                         micros);
}

constexpr LLBC_TimeSpan LLBC_TimeSpan::FromMicros(sint64 micros)
{
    return LLBC_TimeSpan(micros);
}

template <size_t _StrArrLen>
LLBC_TimeSpan LLBC_TimeSpan::FromSpanStr(const char (&spanStr)[_StrArrLen],
                                         const LLBC_TimeSep &timeSep)
{
    return LLBC_TimeSpan(spanStr, _StrArrLen - 1, timeSep);
}

inline LLBC_TimeSpan LLBC_TimeSpan::FromSpanStr(const char *spanStr,
                                                const LLBC_TimeSep &timeSep)
{
    return LLBC_TimeSpan(spanStr, strlen(spanStr), timeSep);
}

template <typename _StrType>
typename std::enable_if<LLBC_IsTemplSpec<_StrType, std::basic_string>::value, LLBC_TimeSpan>::type
LLBC_TimeSpan::FromSpanStr(const _StrType &spanStr,
                           const LLBC_TimeSep &timeSep)
{
    return LLBC_TimeSpan(spanStr.c_str(), spanStr.size(), timeSep);
}

inline LLBC_TimeSpan LLBC_TimeSpan::FromSpanStr(const LLBC_String &spanStr,
                                                const LLBC_TimeSep &timeSep)
{
    return LLBC_TimeSpan(spanStr.c_str(), spanStr.size(), timeSep);
}

constexpr LLBC_TimeSpan::LLBC_TimeSpan(const sint64 &spanInMicros)
: _span(spanInMicros)
{
}

template <size_t _StrArrLen>
LLBC_TimeSpan::LLBC_TimeSpan(const char (&spanStr)[_StrArrLen],
                             const LLBC_TimeSep &timeSep)
: LLBC_TimeSpan(spanStr, _StrArrLen -1, timeSep)
{
}

inline LLBC_TimeSpan::LLBC_TimeSpan(const char *spanStr,
                                    const LLBC_TimeSep &timeSep)
: LLBC_TimeSpan(spanStr, strlen(spanStr), timeSep)
{
}

template <typename _StrType, typename>
LLBC_TimeSpan::LLBC_TimeSpan(const _StrType &spanStr,
                             const LLBC_TimeSep &timeSep)
: LLBC_TimeSpan(spanStr.c_str(), spanStr.size(), timeSep)
{
}

inline LLBC_TimeSpan::LLBC_TimeSpan(const LLBC_String &spanStr,
    const LLBC_TimeSep &timeSep)
: LLBC_TimeSpan(spanStr.c_str(), spanStr.size(), timeSep)
{
}

constexpr int LLBC_TimeSpan::GetDays() const
{
    return static_cast<int>(_span / LLBC_TimeConst::numOfMicrosPerDay);
}

constexpr int LLBC_TimeSpan::GetHours() const
{
    return static_cast<int>((_span % LLBC_TimeConst::numOfMicrosPerDay) / 
        LLBC_TimeConst::numOfMicrosPerHour);
}

constexpr int LLBC_TimeSpan::GetMinutes() const
{
    return static_cast<int>((_span % LLBC_TimeConst::numOfMicrosPerHour) /
        LLBC_TimeConst::numOfMicrosPerMinute);
}

constexpr int LLBC_TimeSpan::GetSeconds() const
{
    return static_cast<int>((_span % LLBC_TimeConst::numOfMicrosPerMinute) /
         LLBC_TimeConst::numOfMicrosPerSecond);
}

constexpr int LLBC_TimeSpan::GetMillis() const
{
    return static_cast<int>((_span % LLBC_TimeConst::numOfMicrosPerSecond) /
        LLBC_TimeConst::numOfMicrosPerMillisecond);
}

constexpr int LLBC_TimeSpan::GetMicros() const
{
    return static_cast<int>(_span % LLBC_TimeConst::numOfMicrosPerMillisecond);
}

constexpr int LLBC_TimeSpan::GetTotalDays() const
{
    return static_cast<int>(_span / LLBC_TimeConst::numOfMicrosPerDay);
}

constexpr int LLBC_TimeSpan::GetTotalHours() const
{
    return static_cast<int>(_span / LLBC_TimeConst::numOfMicrosPerHour);
}

constexpr int LLBC_TimeSpan::GetTotalMinutes() const
{
    return static_cast<int>(_span / LLBC_TimeConst::numOfMicrosPerMinute);
}

constexpr int LLBC_TimeSpan::GetTotalSeconds() const
{
    return static_cast<int>(_span / LLBC_TimeConst::numOfMicrosPerSecond);
}

constexpr sint64 LLBC_TimeSpan::GetTotalMillis() const
{
    return static_cast<sint64>(_span / LLBC_TimeConst::numOfMicrosPerMillisecond);
}

constexpr sint64 LLBC_TimeSpan::GetTotalMicros() const
{
    return _span;
}

constexpr LLBC_TimeSpan LLBC_TimeSpan::AddDays(int days) const
{
    return AddMicros(LLBC_TimeConst::numOfMicrosPerDay * days);
}

constexpr LLBC_TimeSpan LLBC_TimeSpan::AddHours(int hours) const
{
    return AddMicros(LLBC_TimeConst::numOfMicrosPerHour * hours);
}

constexpr LLBC_TimeSpan LLBC_TimeSpan::AddMinutes(int minutes) const
{
    return AddMicros(LLBC_TimeConst::numOfMicrosPerMinute * minutes);
}

constexpr LLBC_TimeSpan LLBC_TimeSpan::AddSeconds(int seconds) const
{
    return AddMicros(LLBC_TimeConst::numOfMicrosPerSecond * seconds);
}

constexpr LLBC_TimeSpan LLBC_TimeSpan::AddMillis(sint64 millis) const
{
    return AddMicros(LLBC_TimeConst::numOfMicrosPerMillisecond * millis);
}

constexpr LLBC_TimeSpan LLBC_TimeSpan::AddMicros(sint64 micros) const
{
    LLBC_TimeSpan span;
    span._span = _span + micros;

    return span;
}

constexpr LLBC_TimeSpan LLBC_TimeSpan::operator+(const LLBC_TimeSpan &span) const
{
    return LLBC_TimeSpan(_span + span._span);
}

constexpr LLBC_TimeSpan LLBC_TimeSpan::operator-(const LLBC_TimeSpan &span) const
{
    return LLBC_TimeSpan(_span - span._span);
}

constexpr LLBC_TimeSpan LLBC_TimeSpan::operator*(double scale) const
{
    return LLBC_TimeSpan(static_cast<sint64>(_span * scale));
}

constexpr LLBC_TimeSpan LLBC_TimeSpan::operator/(double scale) const
{
    return LLBC_TimeSpan(static_cast<sint64>(_span / scale));
}

constexpr LLBC_TimeSpan LLBC_TimeSpan::operator%(sint64 span) const
{
    return LLBC_TimeSpan(_span % span);
}

constexpr LLBC_TimeSpan LLBC_TimeSpan::operator%(const LLBC_TimeSpan &span) const
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

constexpr bool LLBC_TimeSpan::operator==(const LLBC_TimeSpan &span) const
{
    return _span == span._span;
}

constexpr bool LLBC_TimeSpan::operator!=(const LLBC_TimeSpan &span) const
{
    return !(*this == span);
}

constexpr bool LLBC_TimeSpan::operator<(const LLBC_TimeSpan &span) const
{
    return _span < span._span;
}

constexpr bool LLBC_TimeSpan::operator>(const LLBC_TimeSpan &span) const
{
    return _span > span._span;
}

constexpr bool LLBC_TimeSpan::operator<=(const LLBC_TimeSpan &span) const
{
    return _span<= span._span;
}

constexpr bool LLBC_TimeSpan::operator>=(const LLBC_TimeSpan &span) const
{
    return _span >= span._span;
}

inline LLBC_String LLBC_TimeSpan::ToString() const
{
    return LLBC_String().format("%02d %02d:%02d:%02d.%06d", 
    GetDays(), GetHours(), GetMinutes(), GetSeconds(), GetMillis() * 1000 + GetMicros());
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

inline std::ostream &operator<<(std::ostream &stream, const LLBC_NS LLBC_TimeSpan &span)
{
    return stream <<span.ToString();
}


