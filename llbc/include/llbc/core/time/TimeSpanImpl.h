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

#ifdef __LLBC_CORE_TIME_TIMESPAN_H__

#include "llbc/core/time/Time.h"

inline std::ostream &operator <<(
        std::ostream &stream, const LLBC_NS LLBC_TimeSpan &span)
{
    return stream <<span.ToString();
}

__LLBC_NS_BEGIN

inline LLBC_TimeSpan::LLBC_TimeSpan()
: _span()
{
}

inline LLBC_TimeSpan::LLBC_TimeSpan(const LLBC_TimeSpan &span)
: _span(span._span)
{
}

inline LLBC_TimeSpan::LLBC_TimeSpan(int seconds, int milliSeconds, int microSeconds)
{
    _span = seconds * LLBC_Time::NumOfMicroSecondsPerSecond +
        milliSeconds * LLBC_Time::NumOfMicroSecondsPerMilliSecond + microSeconds;
}

inline LLBC_TimeSpan::LLBC_TimeSpan(int days, int hours, int minutes, int seconds, int milliSeconds, int microSeconds)
{
    _span = static_cast<sint64>((((((days * 24) + hours) * 60) + minutes) * 60) + seconds) * 
        LLBC_Time::NumOfMicroSecondsPerSecond + milliSeconds * LLBC_Time::NumOfMicroSecondsPerMilliSecond + microSeconds;
}

inline LLBC_TimeSpan::~LLBC_TimeSpan()
{
}

inline int LLBC_TimeSpan::GetDays() const
{
    return static_cast<int>(_span / LLBC_Time::NumOfMicroSecondsPerDay);
}

inline int LLBC_TimeSpan::GetHours() const
{
    return static_cast<int>((_span % LLBC_Time::NumOfMicroSecondsPerDay) / 
        LLBC_Time::NumOfMicroSecondsPerHour);
}

inline int LLBC_TimeSpan::GetMinutes() const
{
    return static_cast<int>((_span % LLBC_Time::NumOfMicroSecondsPerHour) /
        LLBC_Time::NumOfMicroSecondsPerMinute);
}

inline int LLBC_TimeSpan::GetSeconds() const
{
    return static_cast<int>((_span % LLBC_Time::NumOfMicroSecondsPerMinute) /
         LLBC_Time::NumOfMicroSecondsPerSecond);
}

inline int LLBC_TimeSpan::GetMilliSeconds() const
{
    return static_cast<int>((_span % LLBC_Time::NumOfMicroSecondsPerSecond) /
        LLBC_Time::NumOfMicroSecondsPerMilliSecond);
}

inline int LLBC_TimeSpan::GetMicroSeconds() const
{
    return static_cast<int>(_span % LLBC_Time::NumOfMicroSecondsPerMilliSecond);
}

inline int LLBC_TimeSpan::GetTotalDays() const
{
    return static_cast<int>(_span / LLBC_Time::NumOfMicroSecondsPerDay);
}

inline int LLBC_TimeSpan::GetTotalHours() const
{
    return static_cast<int>(_span / LLBC_Time::NumOfMicroSecondsPerHour);
}

inline int LLBC_TimeSpan::GetTotalMinutes() const
{
    return static_cast<int>(_span / LLBC_Time::NumOfMicroSecondsPerMinute);
}

inline int LLBC_TimeSpan::GetTotalSeconds() const
{
    return static_cast<int>(_span / LLBC_Time::NumOfMicroSecondsPerSecond);
}

inline sint64 LLBC_TimeSpan::GetTotalMilliSeconds() const
{
    return static_cast<sint64>(_span / LLBC_Time::NumOfMicroSecondsPerMilliSecond);
}

inline const sint64 &LLBC_TimeSpan::GetTotalMicroSeconds() const
{
    return _span;
}

inline LLBC_TimeSpan LLBC_TimeSpan::operator +(const LLBC_TimeSpan &span) const
{
    return LLBC_TimeSpan(_span + span._span);
}

inline LLBC_TimeSpan LLBC_TimeSpan::operator -(const LLBC_TimeSpan &span) const
{
    return LLBC_TimeSpan(_span - span._span);
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
            days, GetHours(), GetMinutes(), GetSeconds(), GetMilliSeconds() * 1000 + GetMicroSeconds());
    else
        return LLBC_String().format("%02d:%02d:%02d.%06d",
            GetHours(), GetMinutes(), GetSeconds(), GetMilliSeconds() * 1000 + GetMicroSeconds());
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

inline void LLBC_TimeSpan::SerializeEx(LLBC_Stream &stream) const
{
    stream.WriteEx(_span);
}

inline bool LLBC_TimeSpan::DeSerializeEx(LLBC_Stream &stream)
{
    sint64 spanVal = 0;
    if (!stream.ReadEx(spanVal))
        return false;

    _span = spanVal;

    return true;
}

inline LLBC_TimeSpan::LLBC_TimeSpan(const sint64 &span)
: _span(span)
{
}

__LLBC_NS_END

#endif // __LLBC_CORE_TIME_TIMESPAN_H__
