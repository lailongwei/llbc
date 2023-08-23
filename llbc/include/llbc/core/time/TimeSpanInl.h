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

#include "llbc/core/time/TimeConstant.h"

inline std::ostream &operator <<(std::ostream &stream, const LLBC_NS LLBC_TimeSpan &span)
{
    return stream <<span.ToString();
}

__LLBC_NS_BEGIN

inline LLBC_TimeSpan LLBC_TimeSpan::FromDays(int days, int hours, int minutes, int seconds, int millisecs, int microsecs)
{
    return LLBC_TimeSpan(days * LLBC_TimeConstant::NumOfMicrosecondsPerDay +
                         hours * LLBC_TimeConstant::NumOfMicrosecondsPerHour +
                         minutes * LLBC_TimeConstant::NumOfMicrosecondsPerMinute +
                         seconds * LLBC_TimeConstant::NumOfMicrosecondsPerSecond +
                         millisecs * LLBC_TimeConstant::NumOfMicrosecondsPerMillisecond +
                         microsecs);
}

inline LLBC_TimeSpan LLBC_TimeSpan::FromHours(int hours, int minutes, int seconds, int millisecs, int microsecs)
{
    return LLBC_TimeSpan(hours * LLBC_TimeConstant::NumOfMicrosecondsPerHour +
                         minutes * LLBC_TimeConstant::NumOfMicrosecondsPerMinute +
                         seconds * LLBC_TimeConstant::NumOfMicrosecondsPerSecond +
                         millisecs * LLBC_TimeConstant::NumOfMicrosecondsPerMillisecond +
                         microsecs);
}

inline LLBC_TimeSpan LLBC_TimeSpan::FromMinutes(int minutes, int seconds, int millisecs, int microsecs)
{
    return LLBC_TimeSpan(minutes * LLBC_TimeConstant::NumOfMicrosecondsPerMinute +
                         seconds * LLBC_TimeConstant::NumOfMicrosecondsPerSecond +
                         millisecs * LLBC_TimeConstant::NumOfMicrosecondsPerMillisecond +
                         microsecs);
}

inline LLBC_TimeSpan LLBC_TimeSpan::FromSeconds(int seconds, int millisecs, int microsecs)
{
    return LLBC_TimeSpan(seconds * LLBC_TimeConstant::NumOfMicrosecondsPerSecond +
                         millisecs * LLBC_TimeConstant::NumOfMicrosecondsPerMillisecond +
                         microsecs);
}

inline LLBC_TimeSpan LLBC_TimeSpan::FromMilliseconds(sint64 millisecs, sint64 microsecs)
{
    return LLBC_TimeSpan(millisecs * LLBC_TimeConstant::NumOfMicrosecondsPerMillisecond +
                         microsecs);
}

inline LLBC_TimeSpan LLBC_TimeSpan::FromMicroseconds(sint64 microsecs)
{
    return LLBC_TimeSpan(microsecs);
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
    return static_cast<int>(_span / LLBC_TimeConstant::NumOfMicrosecondsPerDay);
}

inline int LLBC_TimeSpan::GetHours() const
{
    return static_cast<int>((_span % LLBC_TimeConstant::NumOfMicrosecondsPerDay) / 
        LLBC_TimeConstant::NumOfMicrosecondsPerHour);
}

inline int LLBC_TimeSpan::GetMinutes() const
{
    return static_cast<int>((_span % LLBC_TimeConstant::NumOfMicrosecondsPerHour) /
        LLBC_TimeConstant::NumOfMicrosecondsPerMinute);
}

inline int LLBC_TimeSpan::GetSeconds() const
{
    return static_cast<int>((_span % LLBC_TimeConstant::NumOfMicrosecondsPerMinute) /
         LLBC_TimeConstant::NumOfMicrosecondsPerSecond);
}

inline int LLBC_TimeSpan::GetMilliseconds() const
{
    return static_cast<int>((_span % LLBC_TimeConstant::NumOfMicrosecondsPerSecond) /
        LLBC_TimeConstant::NumOfMicrosecondsPerMillisecond);
}

inline int LLBC_TimeSpan::GetMicroseconds() const
{
    return static_cast<int>(_span % LLBC_TimeConstant::NumOfMicrosecondsPerMillisecond);
}

inline int LLBC_TimeSpan::GetTotalDays() const
{
    return static_cast<int>(_span / LLBC_TimeConstant::NumOfMicrosecondsPerDay);
}

inline int LLBC_TimeSpan::GetTotalHours() const
{
    return static_cast<int>(_span / LLBC_TimeConstant::NumOfMicrosecondsPerHour);
}

inline int LLBC_TimeSpan::GetTotalMinutes() const
{
    return static_cast<int>(_span / LLBC_TimeConstant::NumOfMicrosecondsPerMinute);
}

inline int LLBC_TimeSpan::GetTotalSeconds() const
{
    return static_cast<int>(_span / LLBC_TimeConstant::NumOfMicrosecondsPerSecond);
}

inline sint64 LLBC_TimeSpan::GetTotalMilliseconds() const
{
    return static_cast<sint64>(_span / LLBC_TimeConstant::NumOfMicrosecondsPerMillisecond);
}

inline sint64 LLBC_TimeSpan::GetTotalMicroseconds() const
{
    return _span;
}

inline LLBC_TimeSpan LLBC_TimeSpan::AddDays(int days)
{
    return AddMicroseconds(LLBC_TimeConstant::NumOfMicrosecondsPerDay * days);
}

inline LLBC_TimeSpan LLBC_TimeSpan::AddHours(int hours)
{
    return AddMicroseconds(LLBC_TimeConstant::NumOfMicrosecondsPerHour * hours);
}

inline LLBC_TimeSpan LLBC_TimeSpan::AddMinutes(int minutes)
{
    return AddMicroseconds(LLBC_TimeConstant::NumOfMicrosecondsPerMinute * minutes);
}

inline LLBC_TimeSpan LLBC_TimeSpan::AddSeconds(int seconds)
{
    return AddMicroseconds(LLBC_TimeConstant::NumOfMicrosecondsPerSecond * seconds);
}

inline LLBC_TimeSpan LLBC_TimeSpan::AddMilliseconds(sint64 milliSeconds)
{
    return AddMicroseconds(LLBC_TimeConstant::NumOfMicrosecondsPerMillisecond * milliSeconds);
}

inline LLBC_TimeSpan LLBC_TimeSpan::AddMicroseconds(sint64 microSeconds)
{
    LLBC_TimeSpan span;
    span._span = _span + microSeconds;

    return span;
}

inline LLBC_TimeSpan LLBC_TimeSpan::operator +(const LLBC_TimeSpan &span) const
{
    return LLBC_TimeSpan(_span + span._span);
}

inline LLBC_TimeSpan LLBC_TimeSpan::operator -(const LLBC_TimeSpan &span) const
{
    return LLBC_TimeSpan(_span - span._span);
}

inline LLBC_TimeSpan LLBC_TimeSpan::operator *(double scale) const
{
    return LLBC_TimeSpan(static_cast<sint64>(_span * scale));
}

inline LLBC_TimeSpan LLBC_TimeSpan::operator /(double scale) const
{
    return LLBC_TimeSpan(static_cast<sint64>(_span / scale));
}

inline LLBC_TimeSpan LLBC_TimeSpan::operator %(sint64 scale) const
{
    return LLBC_TimeSpan(_span % scale);
}

inline LLBC_TimeSpan &LLBC_TimeSpan::operator +=(const LLBC_TimeSpan &span)
{
    _span += span._span;
    return *this;
}

inline LLBC_TimeSpan &LLBC_TimeSpan::operator -=(const LLBC_TimeSpan &span)
{
    _span -= span._span;
    return *this;
}

inline LLBC_TimeSpan &LLBC_TimeSpan::operator *=(double scale)
{
    _span = static_cast<sint64>(_span * scale);
    return *this;
}

inline LLBC_TimeSpan &LLBC_TimeSpan::operator /=(double scale)
{
    _span = static_cast<sint64>(_span / scale);
    return *this;
}

inline LLBC_TimeSpan &LLBC_TimeSpan::operator %=(sint64 scale)
{
    _span %= scale;
    return *this;
}

inline bool LLBC_TimeSpan::operator ==(const LLBC_TimeSpan &span) const
{
    return _span == span._span;
}

inline bool LLBC_TimeSpan::operator !=(const LLBC_TimeSpan &span) const
{
    return !(*this == span);
}

inline bool LLBC_TimeSpan::operator <(const LLBC_TimeSpan &span) const
{
    return _span < span._span;
}

inline bool LLBC_TimeSpan::operator >(const LLBC_TimeSpan &span) const
{
    return _span > span._span;
}

inline bool LLBC_TimeSpan::operator <=(const LLBC_TimeSpan &span) const
{
    return _span<= span._span;
}

inline bool LLBC_TimeSpan::operator >=(const LLBC_TimeSpan &span) const
{
    return _span >= span._span;
}

inline LLBC_TimeSpan &LLBC_TimeSpan::operator =(const LLBC_TimeSpan &span)
{
    _span = span._span;
    return *this;
}

inline LLBC_String LLBC_TimeSpan::ToString() const
{
    int days = GetDays();
    if (days != 0)
        return LLBC_String().format("%d %02d:%02d:%02d.%06d", 
            days, GetHours(), GetMinutes(), GetSeconds(), GetMilliseconds() * 1000 + GetMicroseconds());
    else
        return LLBC_String().format("%02d:%02d:%02d.%06d",
            GetHours(), GetMinutes(), GetSeconds(), GetMilliseconds() * 1000 + GetMicroseconds());
}

inline void LLBC_TimeSpan::Serialize(LLBC_Stream &stream) const
{
    stream.Write(_span);
}

inline bool LLBC_TimeSpan::DeSerialize(LLBC_Stream &stream)
{
    sint64 spanVal = 0;
    if (!stream.Read(spanVal))
        return false;

    _span = spanVal;

    return true;
}

__LLBC_NS_END
