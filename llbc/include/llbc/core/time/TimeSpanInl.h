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

inline LLBC_TimeSpan LLBC_TimeSpan::FromSpanStr(const LLBC_String &spanStr)
{
    return LLBC_TimeSpan(spanStr);
}

inline LLBC_TimeSpan::LLBC_TimeSpan(const sint64 &span)
: _span(span)
{
}

inline LLBC_TimeSpan::~LLBC_TimeSpan()
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
