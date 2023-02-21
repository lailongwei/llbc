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

__LLBC_NS_BEGIN

inline LLBC_Time::LLBC_Time()
: _time(0)
{
    UpdateTimeStructs();
}

inline LLBC_Time::LLBC_Time(const LLBC_Time &time)
: _time(time._time)
{
    memcpy(&_localTimeStruct, &time._localTimeStruct, sizeof(tm));
    memcpy(&_gmtTimeStruct, &time._gmtTimeStruct, sizeof(tm));
}

inline LLBC_Time::~LLBC_Time()
{
}

inline time_t LLBC_Time::NowTimestampInSecs()
{
    return time(nullptr);
}

inline sint64 LLBC_Time::NowTimestampInMillis()
{
    return LLBC_GetMilliSeconds();
}

inline sint64 LLBC_Time::NowTimestampInMicros()
{
    return LLBC_GetMicroSeconds();
}

inline LLBC_Time LLBC_Time::FromSeconds(time_t clanderTimeInSeconds)
{
    return LLBC_Time(clanderTimeInSeconds * NumOfMicroSecondsPerSecond);
}

inline LLBC_Time LLBC_Time::FromMillis(sint64 clanderTimeInMilliSeconds)
{
    return LLBC_Time(clanderTimeInMilliSeconds * NumOfMicroSecondsPerMilliSecond);
}

inline LLBC_Time LLBC_Time::FromMicros(sint64 clanderTimeInMicroSeconds)
{
    return LLBC_Time(clanderTimeInMicroSeconds);
}

inline LLBC_Time LLBC_Time::FromTimeVal(const timeval &timeVal)
{
    return LLBC_Time(timeVal.tv_sec * NumOfMicroSecondsPerSecond + timeVal.tv_usec);
}

inline LLBC_Time LLBC_Time::FromTimeSpec(const timespec &timeSpec)
{
    return LLBC_Time(timeSpec.tv_sec * NumOfMicroSecondsPerSecond + timeSpec.tv_nsec / NumOfNanoSecondsPerMicroSecond);
}

inline int LLBC_Time::GetYear() const
{
    return _localTimeStruct.tm_year + 1900; // scene 1900
}

inline int LLBC_Time::GetMonth() const
{
    return _localTimeStruct.tm_mon + 1; // start by 1
}

inline int LLBC_Time::GetDay() const
{
    return _localTimeStruct.tm_mday;
}

inline int LLBC_Time::GetDayOfWeek() const
{
    return _localTimeStruct.tm_wday;
}

inline int LLBC_Time::GetDayOfMonth() const
{
        return GetDayOfYear() - GetMonthSpanDays(GetYear(), GetMonth() - 1);
}

inline int LLBC_Time::GetDayOfYear() const
{
    return _localTimeStruct.tm_yday + 1; // start by 1
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
    return static_cast<time_t>(_time / LLBC_TimeConstant::NumOfMicroSecondsPerSecond);
}

inline sint64 LLBC_Time::GetTimestampInMillis() const
{
    return _time / LLBC_TimeConstant::NumOfMicroSecondsPerMilliSecond;
}

inline sint64 LLBC_Time::GetTimestampInMicros() const
{
    return _time;
}

inline const tm &LLBC_Time::GetGmtTime() const
{
    return _gmtTimeStruct;
}

inline void LLBC_Time::GetGmtTime(tm &timeStruct) const
{
    memcpy(&timeStruct, &_gmtTimeStruct, sizeof(tm));
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

inline bool LLBC_Time::operator ==(const LLBC_Time &time) const
{
    return _time == time._time;
}

inline bool LLBC_Time::operator !=(const LLBC_Time &time) const
{
    return _time != time._time;
}

inline bool LLBC_Time::operator <(const LLBC_Time &time) const
{
    return _time < time._time;
}

inline bool LLBC_Time::operator >(const LLBC_Time &time) const
{
    return _time > time._time;
}

inline bool LLBC_Time::operator >=(const LLBC_Time &time) const
{
    return _time >= time._time;
}

inline bool LLBC_Time::operator <=(const LLBC_Time &time) const
{
    return _time <= time._time;
}

inline void LLBC_Time::Serialize(LLBC_Stream &stream) const
{
    stream.Write(_time);
}

inline LLBC_Time::LLBC_Time(const sint64 &clanderTimeInMicroSeconds)
: _time(clanderTimeInMicroSeconds)
{
    UpdateTimeStructs();
}

__LLBC_NS_END
