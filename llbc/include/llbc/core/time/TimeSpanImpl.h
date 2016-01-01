/**
 * @file    TimeSpanImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/12/26
 * @version 1.0
 *
 * @brief
 */
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

inline LLBC_TimeSpan::LLBC_TimeSpan(double time)
{
    _span = time;
}

inline LLBC_TimeSpan::LLBC_TimeSpan(int days, int hours, int minutes, int seconds, int milliSeconds, int microSeconds)
{
    _span = (((((days * 24) + hours) * 60) + minutes) * 60) + seconds +
        static_cast<double>(milliSeconds) / LLBC_Time::NumOfMilliSecondsPerSecond +
        static_cast<double>(microSeconds) / LLBC_Time::NumOfMicroSecondsPerSecond;
}

inline LLBC_TimeSpan::~LLBC_TimeSpan()
{
}

inline int LLBC_TimeSpan::GetDays() const
{
    return (int)(_span / LLBC_Time::NumOfSecondsPerDay);
}

inline int LLBC_TimeSpan::GetHours() const
{
    return (int)((static_cast<sint64>(_span) % LLBC_Time::NumOfSecondsPerDay) / 
        LLBC_Time::NumOfSecondsPerHour);
}

inline int LLBC_TimeSpan::GetMinutes() const
{
    return (int)((static_cast<sint64>(_span) % LLBC_Time::NumOfSecondsPerHour) /
        LLBC_Time::NumOfSecondsPerMinute);
}

inline int LLBC_TimeSpan::GetSeconds() const
{
    return (int)(static_cast<sint64>(_span) % LLBC_Time::NumOfSecondsPerMinute);
}

inline int LLBC_TimeSpan::GetMilliSeconds() const
{
    return (int)(static_cast<sint64>(
        _span * LLBC_Time::NumOfMilliSecondsPerSecond) % LLBC_Time::NumOfMilliSecondsPerSecond);
}

inline int LLBC_TimeSpan::GetMicroSeconds() const
{
    return (int)(static_cast<sint64>(_span * 
        LLBC_Time::NumOfMicroSecondsPerSecond) % LLBC_Time::NumOfMicroSecondsPerMilliSecond);
}

inline double LLBC_TimeSpan::GetTotalDays() const
{
    return _span / LLBC_Time::NumOfSecondsPerDay;
}

inline double LLBC_TimeSpan::GetTotalHours() const
{
    return _span / LLBC_Time::NumOfSecondsPerHour;
}

inline double LLBC_TimeSpan::GetTotalMinutes() const
{
    return _span / LLBC_Time::NumOfSecondsPerMinute;
}

inline double LLBC_TimeSpan::GetTotalSeconds() const
{
    return _span;
}

inline double LLBC_TimeSpan::GetTotalMilliSeconds() const
{
    return _span * LLBC_Time::NumOfMilliSecondsPerSecond;
}

inline double LLBC_TimeSpan::GetTotalMicroSeconds() const
{
    return _span * LLBC_Time::NumOfMicroSecondsPerSecond;
}

inline LLBC_TimeSpan LLBC_TimeSpan::operator +(const LLBC_TimeSpan &span) const
{
    return LLBC_TimeSpan(_span + span._span);
}

inline LLBC_TimeSpan LLBC_TimeSpan::operator -(const LLBC_TimeSpan &span) const
{
    return LLBC_TimeSpan(_span - span._span);
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

inline LLBC_TimeSpan &LLBC_TimeSpan::operator =(double span)
{
    _span = span;
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
    double spanVal = 0;
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
    double spanVal = 0;
    if (!stream.ReadEx(spanVal))
        return false;

    _span = spanVal;

    return true;
}

__LLBC_NS_END

#endif // __LLBC_CORE_TIME_TIMESPAN_H__
