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

#include "llbc/core/os/OS_Time.h"
#include "llbc/core/time/TimeConst.h"

__LLBC_NS_BEGIN

inline LLBC_Time::LLBC_Time()
: LLBC_Time(0)
{
}

inline LLBC_Time::LLBC_Time(const LLBC_Time &time)
: _time(time._time)
{
    memcpy(&_localTimeStruct, &time._localTimeStruct, sizeof(tm));
}

inline time_t LLBC_Time::NowTimestampInSecs()
{
    return time(nullptr);
}

inline sint64 LLBC_Time::NowTimestampInMillis()
{
    return LLBC_GetMilliseconds();
}

inline sint64 LLBC_Time::NowTimestampInMicros()
{
    return LLBC_GetMicroseconds();
}

inline LLBC_Time LLBC_Time::FromSeconds(time_t clanderTimeInSeconds)
{
    return LLBC_Time(clanderTimeInSeconds * LLBC_TimeConst::numOfMicrosPerSecond);
}

inline LLBC_Time LLBC_Time::FromMillis(sint64 clanderTimeInMillis)
{
    return LLBC_Time(clanderTimeInMillis * LLBC_TimeConst::numOfMicrosPerMillisecond);
}

inline LLBC_Time LLBC_Time::FromMicros(sint64 clanderTimeInMicros)
{
    return LLBC_Time(clanderTimeInMicros);
}

inline LLBC_Time LLBC_Time::FromTimeVal(const timeval &timeVal)
{
    return LLBC_Time(timeVal.tv_sec * LLBC_TimeConst::numOfMicrosPerSecond + timeVal.tv_usec);
}

inline LLBC_Time LLBC_Time::FromTimeSpec(const timespec &timeSpec)
{
    return LLBC_Time(timeSpec.tv_sec * LLBC_TimeConst::numOfMicrosPerSecond +
                     timeSpec.tv_nsec / LLBC_TimeConst::numOfNanosPerMicrosecond);
}

template <size_t _StrArrLen>
LLBC_Time LLBC_Time::FromTimeStr(const char (&timeStr)[_StrArrLen])
{
    return FromTimeStr(timeStr, _StrArrLen - 1);
}

inline LLBC_Time LLBC_Time::FromTimeStr(const char *timeStr)
{
    return FromTimeStr(timeStr, strlen(timeStr));
}

template <typename _StrType>
typename std::enable_if<LLBC_IsTemplSpec<_StrType, std::basic_string>::value, LLBC_Time>::type
LLBC_Time::FromTimeStr(const _StrType &timeStr)
{
    return FromTimeStr(timeStr.c_str(), timeStr.size());
}

inline LLBC_Time LLBC_Time::FromTimeStr(const LLBC_String &timeStr)
{
    return FromTimeStr(timeStr.c_str(), timeStr.size());
}

inline int LLBC_Time::GetYear() const
{
    return _localTimeStruct.tm_year + 1900; // scene 1900
}

inline int LLBC_Time::GetMonth() const
{
    return _localTimeStruct.tm_mon;
}

inline int LLBC_Time::GetDayOfWeek() const
{
    return _localTimeStruct.tm_wday;
}

inline int LLBC_Time::GetDayOfMonth() const
{
    return _localTimeStruct.tm_mday;
}

inline int LLBC_Time::GetDayOfYear() const
{
    return _localTimeStruct.tm_yday;
}

inline int LLBC_Time::GetHour() const
{
    return _localTimeStruct.tm_hour;
}

inline int LLBC_Time::GetMinute() const
{
    return _localTimeStruct.tm_min;
}

inline int LLBC_Time::GetSecond() const
{
    return _localTimeStruct.tm_sec;
}

inline time_t LLBC_Time::GetTimestampInSecs() const
{
    return static_cast<time_t>(_time / LLBC_TimeConst::numOfMicrosPerSecond);
}

inline sint64 LLBC_Time::GetTimestampInMillis() const
{
    return _time / LLBC_TimeConst::numOfMicrosPerMillisecond;
}

inline sint64 LLBC_Time::GetTimestampInMicros() const
{
    return _time;
}

inline tm LLBC_Time::GetGmtTime() const
{
    struct tm timeStruct;
    GetGmtTime(timeStruct);

    return timeStruct;
}

inline void LLBC_Time::GetGmtTime(tm &timeStruct) const
{
    const time_t calendarTime = static_cast<time_t>(_time / LLBC_TimeConst::numOfMicrosPerSecond);
    #if LLBC_TARGET_PLATFORM_WIN32
    gmtime_s(&timeStruct, &calendarTime);
    #else
    gmtime_r(&calendarTime, &timeStruct);
    #endif
}

inline const tm &LLBC_Time::GetLocalTime() const
{
    return _localTimeStruct;
}

inline void LLBC_Time::GetLocalTime(tm &timeStruct) const
{
    memcpy(&timeStruct, &_localTimeStruct, sizeof(tm));
}

inline LLBC_String LLBC_Time::Format(const time_t &clanderTimeInSeconds, const char *format)
{
    return FromSeconds(clanderTimeInSeconds).Format(format);
}

inline LLBC_String LLBC_Time::FormatAsGmt(const time_t &clanderTimeInSeconds, const char *format)
{
    return FromSeconds(clanderTimeInSeconds).FormatAsGmt(format);
}

inline LLBC_TimeSpan LLBC_Time::GetIntervalToTimeOfHour(const LLBC_TimeSpan &toTimeOfHour) const
{
    return GetIntervalTo(LLBC_TimeSpan::oneHour, toTimeOfHour);
}

inline LLBC_TimeSpan LLBC_Time::GetIntervalToTimeOfDay(const LLBC_TimeSpan &toTimeOfDay) const
{
    return GetIntervalTo(LLBC_TimeSpan::oneDay, toTimeOfDay);
}

inline LLBC_TimeSpan LLBC_Time::GetIntervalToTimeOfWeek(const LLBC_TimeSpan &toTimeOfWeek) const
{
    return GetIntervalTo(LLBC_TimeSpan::oneWeek, toTimeOfWeek);
}

inline bool LLBC_Time::IsCrossedHour(const LLBC_Time &from,
                                     const LLBC_Time &to,
                                     const LLBC_TimeSpan &timeOfHour)
{
    return GetCrossTime(from, to, LLBC_TimeSpan::oneHour, timeOfHour).GetTotalHours() != 0;
}

inline bool LLBC_Time::IsCrossedDay(const LLBC_Time &from,
                                    const LLBC_Time &to,
                                    const LLBC_TimeSpan &timeOfDay)
{
    return GetCrossTime(from, to, LLBC_TimeSpan::oneDay, timeOfDay).GetTotalDays() != 0;
}

inline bool LLBC_Time::IsCrossedWeek(const LLBC_Time &from,
                                     const LLBC_Time &to,
                                     const LLBC_TimeSpan &timeOfWeek)
{
    return GetCrossTime(from, to, LLBC_TimeSpan::oneWeek, timeOfWeek).GetTotalDays() >= 7;
}

inline bool LLBC_Time::operator==(const LLBC_Time &time) const
{
    return _time == time._time;
}

inline bool LLBC_Time::operator!=(const LLBC_Time &time) const
{
    return _time != time._time;
}

inline bool LLBC_Time::operator<(const LLBC_Time &time) const
{
    return _time < time._time;
}

inline bool LLBC_Time::operator>(const LLBC_Time &time) const
{
    return _time > time._time;
}

inline bool LLBC_Time::operator>=(const LLBC_Time &time) const
{
    return _time >= time._time;
}

inline bool LLBC_Time::operator<=(const LLBC_Time &time) const
{
    return _time <= time._time;
}

inline void LLBC_Time::Serialize(LLBC_Stream &stream) const
{
    stream.Write(_time);
}

inline LLBC_Time::LLBC_Time(const sint64 &clendarTimeInMicroseconds)
: _time(clendarTimeInMicroseconds)
{
    FillTimeStruct();
}

inline void LLBC_Time::FillTimeStruct()
{
    time_t calendarTime = static_cast<time_t>(_time / LLBC_TimeConst::numOfMicrosPerSecond);
    #if LLBC_TARGET_PLATFORM_WIN32
    localtime_s(&_localTimeStruct, &calendarTime);
    #else
    localtime_r(&calendarTime, &_localTimeStruct);
    #endif
}

__LLBC_NS_END

inline std::ostream &operator<<(std::ostream &stream, const LLBC_NS LLBC_Time &t)
{
    return stream << t.ToString();
}

