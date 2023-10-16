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


#include "llbc/common/Export.h"

#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/time/TimeSpan.h"
#include "llbc/core/time/Time.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

std::ostream &operator<<(std::ostream &stream, const LLBC_NS LLBC_Time &t)
{
    return stream << t.ToString();
}

__LLBC_INTERNAL_NS_BEGIN

static constexpr LLBC_NS LLBC_TimeSpan (LLBC_NS LLBC_Time::*__g_GetTimeOfTimeCycleMeths[4])() const {
    &LLBC_NS LLBC_Time::GetTimeOfHour,
    &LLBC_NS LLBC_Time::GetTimeOfDay,
    &LLBC_NS LLBC_Time::GetTimeOfWeek,
    &LLBC_NS LLBC_Time::GetTimeOfMonth,
};

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

const LLBC_Time LLBC_Time::utcBegin;

LLBC_Time LLBC_Time::Now()
{
    return LLBC_Time(LLBC_GetMicroseconds());
}

LLBC_Time LLBC_Time::FromTimeStr(const LLBC_String &timeStr)
{
    if (UNLIKELY(timeStr.empty()))
        return LLBC_Time();

    // Split date, time
    LLBC_Strings dateTimes = timeStr.split(' ', 1, true);
    if (dateTimes.size() == 1) // Only has date part or time part(try guess).
    {
        if (dateTimes[0].find('-') != LLBC_String::npos) // Is date part, append default time part.
            dateTimes.push_back("0:0:0.000");
        else // Is time part, insert default date part.
            dateTimes.insert(dateTimes.begin(), "1970-1-1");
    }

    const LLBC_String &datePart = dateTimes[0];
    const LLBC_String &timePart = dateTimes[1];

    // Split year,month,day
    LLBC_Strings dateParts = datePart.split('-', 2, true);
    if (dateParts.size() == 1) // Only has day part.
    {
        dateParts.insert(dateParts.begin(), "1");
        dateParts.insert(dateParts.begin(), "1970");
    }
    else if (dateParts.size() == 2) // Only has day and month parts.
    {
        dateParts.insert(dateParts.begin(), "1970");
    }

    // Split hour,minute,second
    LLBC_Strings timeParts = timePart.split(':', 2, true);
    if (timeParts.size() == 1) // Only has hour part.
    {
        timeParts.push_back("0");
        timeParts.push_back("0");
    }
    else if (timeParts.size() == 2) // Only has hour and minute parts.
    {
        timeParts.push_back("0");
    }

    // Split second,millisecond
    LLBC_Strings secondParts = timeParts[2].split('.', 1);
    if (secondParts.size() == 1) // Only has second part.
        secondParts.push_back("0");

    // Convert it
    int year = LLBC_Str2Int32(dateParts[0].c_str());
    int month = LLBC_Str2Int32(dateParts[1].c_str());
    int day = LLBC_Str2Int32(dateParts[2].c_str());
    int hour = LLBC_Str2Int32(timeParts[0].c_str());
    int minute = LLBC_Str2Int32(timeParts[1].c_str());
    int second = LLBC_Str2Int32(secondParts[0].c_str());
    int microSeconds = LLBC_Str2Int32(secondParts[1].c_str());
    return FromTimeParts(year,
                         month,
                         day,
                         hour,
                         minute,
                         second,
                         microSeconds / LLBC_TimeConst::NumOfMicrosPerMillisecond,
                         microSeconds % LLBC_TimeConst::NumOfMicrosPerMillisecond);
}

LLBC_Time LLBC_Time::FromTimeStruct(const tm &timeStruct,
                                    int milliSec,
                                    int microSec)
{
    time_t calendarTimeInSecs = mktime(const_cast<tm *>(&timeStruct));
    return LLBC_Time(calendarTimeInSecs * LLBC_TimeConst::NumOfMicrosPerSecond +
                     milliSec * LLBC_TimeConst::NumOfMicrosPerMillisecond +
                     microSec);
}

LLBC_Time LLBC_Time::FromTimeParts(int year,
                                   int month,
                                   int day,
                                   int hour,
                                   int minute,
                                   int second,
                                   int milliSec, 
                                   int microSec)
{
    tm timeStruct;
    timeStruct.tm_year = year - 1900;
    timeStruct.tm_mon = month - 1;
    timeStruct.tm_mday = day;
    timeStruct.tm_isdst = 0;

    if (year == 1970 && month == 1 && day == 1)
    {
        int tz = LLBC_GetTimezone();
        int totalSeconds = hour * LLBC_TimeConst::NumOfSecondsPerHour +
            minute * LLBC_TimeConst::NumOfSecondsPerMinute + second;
        if (tz < 0 && totalSeconds < -tz)
        {
            hour = -tz / LLBC_TimeConst::NumOfSecondsPerHour;
            minute = (-tz % LLBC_TimeConst::NumOfSecondsPerHour) /
                LLBC_TimeConst::NumOfSecondsPerMinute;
            second = -tz % LLBC_TimeConst::NumOfSecondsPerMinute;
        }
    }

    timeStruct.tm_hour = hour;
    timeStruct.tm_min = minute;
    timeStruct.tm_sec = second;

    return FromTimeStruct(timeStruct, milliSec, microSec);
}

int LLBC_Time::GetMillisecond() const
{
    sint64 nowLocalMillis = 
        _time / LLBC_TimeConst::NumOfMicrosPerMillisecond - 
        LLBC_GetTimezone() * LLBC_TimeConst::NumOfMillisPerSecond;
    return static_cast<int>(
        nowLocalMillis % LLBC_TimeConst::NumOfMillisPerSecond);
}

int LLBC_Time::GetMicrosecond() const
{
    sint64 nowLocalMicros =
        _time - LLBC_GetTimezone() * LLBC_TimeConst::NumOfMicrosPerSecond;
    return static_cast<int>(
        nowLocalMicros % LLBC_TimeConst::NumOfMicrosPerMillisecond);
}

LLBC_Time LLBC_Time::GetDate() const
{
    sint64 timeZone = LLBC_GetTimezone() * LLBC_TimeConst::NumOfMicrosPerSecond;

    sint64 localTime = _time - timeZone;
    sint64 datePart = localTime /
        LLBC_TimeConst::NumOfMicrosPerDay *
        LLBC_TimeConst::NumOfMicrosPerDay;

    datePart += timeZone;

    return LLBC_Time(datePart);
}

LLBC_TimeSpan LLBC_Time::GetTimeOfHour() const
{
    return LLBC_TimeSpan(
        (_time - (LLBC_GetTimezone() * LLBC_TimeConst::NumOfMicrosPerSecond)) %
            LLBC_TimeConst::NumOfMicrosPerHour);
}

LLBC_TimeSpan LLBC_Time::GetTimeOfDay() const
{
    return LLBC_TimeSpan(
        (_time - (LLBC_GetTimezone() * LLBC_TimeConst::NumOfMicrosPerSecond)) %
            LLBC_TimeConst::NumOfMicrosPerDay);
}

LLBC_TimeSpan LLBC_Time::GetTimeOfWeek() const
{
    return LLBC_TimeSpan::FromDays(GetDayOfWeek(),
                                   GetHour(),
                                   GetMinute(),
                                   GetSecond(),
                                   GetMillisecond(),
                                   GetMicrosecond());
}

LLBC_TimeSpan LLBC_Time::GetTimeOfMonth() const
{
    return LLBC_TimeSpan::FromDays(GetDayOfMonth() - 1,
                                   GetHour(),
                                   GetMinute(),
                                   GetSecond(),
                                   GetMillisecond(),
                                   GetMicrosecond());
}

LLBC_String LLBC_Time::Format(const char *format) const
{
    char buf[32];
    if (format)
        strftime(buf, sizeof(buf), format, &_localTimeStruct);
    else
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &_localTimeStruct);

    return buf;
}

LLBC_String LLBC_Time::FormatAsGmt(const char *format) const
{
    char buf[32];
    if (format)
        strftime(buf, sizeof(buf), format, &_gmtTimeStruct);
    else
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &_gmtTimeStruct);

    return buf;
}

LLBC_Time LLBC_Time::AddYears(int years) const
{
    if (years == 0)
        return *this;

    tm newTimeStruct;
    GetLocalTime(newTimeStruct);

    newTimeStruct.tm_year += years;
    bool isLeap = IsLeapYear(GetYear());
    if (isLeap && 
        GetMonth() == 2 && GetDay() == 29)
    {
        if (!IsLeapYear(GetYear() + years))
            newTimeStruct.tm_mday -= 1;
    }

    return FromTimeStruct(newTimeStruct, GetMillisecond(), GetMicrosecond());
}

LLBC_Time LLBC_Time::AddMonths(int months) const
{
    LLBC_Time yearAddedTime = AddYears(months / 12);

    months %= 12;
    tm newTimeStruct;
    yearAddedTime.GetLocalTime(newTimeStruct);

    if (months >= 0)
    {
        int remainingMonths = 11 - newTimeStruct.tm_mon;
        if (months > remainingMonths)
        {
            newTimeStruct.tm_year += 1;
            newTimeStruct.tm_mon = months - (12 - newTimeStruct.tm_mon);
        }
        else
        {
            newTimeStruct.tm_mon += months;
        }
    }
    else
    {
        months = -months;
        int elapsedMonths = newTimeStruct.tm_mon + 1;
        if (months >= elapsedMonths)
        {
            newTimeStruct.tm_year -= 1;
            newTimeStruct.tm_mon = 12 - (months - elapsedMonths) - 1;
        }
        else
        {
            newTimeStruct.tm_mon -= months;
        }
    }

    newTimeStruct.tm_mday = MIN(newTimeStruct.tm_mday, 
        GetMonthMaxDays(yearAddedTime.GetYear(), newTimeStruct.tm_mon + 1));

    return FromTimeStruct(newTimeStruct, GetMillisecond(), GetMicrosecond());
}

LLBC_Time LLBC_Time::AddDays(int days) const
{
    return *this + LLBC_TimeSpan(days * LLBC_TimeConst::NumOfMicrosPerDay);
}

LLBC_Time LLBC_Time::AddHours(int hours) const
{
    return *this + LLBC_TimeSpan(hours * LLBC_TimeConst::NumOfMicrosPerHour);
}

LLBC_Time LLBC_Time::AddMinutes(int minutes) const
{
    return *this + LLBC_TimeSpan(minutes * LLBC_TimeConst::NumOfMicrosPerMinute);
}

LLBC_Time LLBC_Time::AddSeconds(int seconds) const
{
    return *this + LLBC_TimeSpan(seconds * LLBC_TimeConst::NumOfMicrosPerSecond);
}

LLBC_Time LLBC_Time::AddMillis(int millis) const
{
    return *this + LLBC_TimeSpan(millis * LLBC_TimeConst::NumOfMicrosPerMillisecond);
}

LLBC_Time LLBC_Time::AddMicros(int micros) const
{
    return *this + LLBC_TimeSpan(static_cast<sint64>(micros));
}

bool LLBC_Time::IsLeapYear(int year)
{
    return LLBC_IsLeapYear(year);
}

int LLBC_Time::GetMonthMaxDays(int year, int month)
{
    return LLBC_GetMonthMaxDays(year, month);
}

int LLBC_Time::GetMonthSpanDays(int year, int month)
{
    return LLBC_GetMonthSpanDays(year, month);
}

LLBC_TimeSpan LLBC_Time::operator-(const LLBC_Time &time) const
{
    return LLBC_TimeSpan(_time - time._time);
}

LLBC_TimeSpan LLBC_Time::operator+(const LLBC_Time &time) const
{
    return LLBC_TimeSpan(_time - time._time);
}

LLBC_Time LLBC_Time::operator+(const LLBC_TimeSpan &span) const
{
    return LLBC_Time(_time + span.GetTotalMicros());
}

LLBC_Time LLBC_Time::operator-(const LLBC_TimeSpan &span) const
{
    return LLBC_Time(_time - span.GetTotalMicros());
}

LLBC_Time &LLBC_Time::operator=(const LLBC_Time &time)
{
    if (this == &time ||
        *this == time)
        return *this;

    _time = time._time;
    memcpy(&_localTimeStruct, &time._localTimeStruct, sizeof(tm));
    memcpy(&_gmtTimeStruct, &time._gmtTimeStruct, sizeof(tm));

    return *this;
}

LLBC_String LLBC_Time::ToString() const
{
    LLBC_String repr;
    return repr.format("%s.%06lld",
                       Format().c_str(),
                       GetMillisecond() *
                           LLBC_TimeConst::NumOfMicrosPerMillisecond + GetMicrosecond());
}

bool LLBC_Time::DeSerialize(LLBC_Stream &stream)
{
    sint64 timeVal = 0;
    if (!stream.Read(timeVal))
        return false;

    _time = timeVal;
    UpdateTimeStructs();

    return true;
}

void LLBC_Time::UpdateTimeStructs()
{
    time_t calendarTime = static_cast<time_t>(_time / LLBC_TimeConst::NumOfMicrosPerSecond);
    #if LLBC_TARGET_PLATFORM_WIN32
    localtime_s(&_localTimeStruct, &calendarTime);
    gmtime_s(&_gmtTimeStruct, &calendarTime);
    #else
    localtime_r(&calendarTime, &_localTimeStruct);
    gmtime_r(&calendarTime, &_gmtTimeStruct);
    #endif
}

LLBC_TimeSpan LLBC_Time::GetIntervalTo(const LLBC_TimeSpan &timeCycle,
                                       LLBC_TimeSpan toTimeOfTimeCycle) const
{
    // !For now, GetIntervalToTimeOfMonth() is not supported.

    // Calc fromTimeOfTimeCycle.
    LLBC_TimeSpan fromTimeOfTimeCycle;
    if (timeCycle == LLBC_TimeSpan::oneHour)
        fromTimeOfTimeCycle = (this->*LLBC_INL_NS __g_GetTimeOfTimeCycleMeths[0])();
    else if (timeCycle == LLBC_TimeSpan::oneDay)
        fromTimeOfTimeCycle = (this->*LLBC_INL_NS __g_GetTimeOfTimeCycleMeths[1])();
    else // oneWeek
        fromTimeOfTimeCycle = (this->*LLBC_INL_NS __g_GetTimeOfTimeCycleMeths[2])();

    // Normalize toTimeOfTimeCycle.
    if (UNLIKELY(toTimeOfTimeCycle < LLBC_TimeSpan::zero))
        toTimeOfTimeCycle = toTimeOfTimeCycle % timeCycle + timeCycle;
    else if (toTimeOfTimeCycle >= timeCycle)
        toTimeOfTimeCycle %= timeCycle;

    // Debug assertion.
    #if LLBC_DEBUG
    ASSERT((toTimeOfTimeCycle >= LLBC_TimeSpan::zero && toTimeOfTimeCycle < timeCycle) &&
           "llbc framework internal error");
    #endif

    // Calc interval-to.
    const LLBC_TimeSpan diff = toTimeOfTimeCycle - fromTimeOfTimeCycle;
    return diff < LLBC_TimeSpan::zero ? diff + timeCycle : diff;
}

bool LLBC_Time::IsCrossed(const LLBC_Time &from,
                          const LLBC_Time &to,
                          const LLBC_TimeSpan &timeCycle,
                          LLBC_TimeSpan timeOfTimeCycle)
{
    // If span < 0, return false.
    const LLBC_TimeSpan diff = to - from;
    if (UNLIKELY(diff <= LLBC_TimeSpan::zero))
        return false;

    // If span >= timeCycle, return true.
    if (diff >= timeCycle)
        return true;

    // Normalize timeOfTimeCycle[0, timeCycle)
    if (UNLIKELY(timeOfTimeCycle < LLBC_TimeSpan::zero))
        timeOfTimeCycle = timeOfTimeCycle % timeCycle + timeCycle;
    else if (UNLIKELY(timeOfTimeCycle >= timeCycle))
        timeOfTimeCycle %= timeCycle;

    // Crossed timeCycle judge: toTimeOfTimeCycle judge.
    // => toTimeOfTimeCycle must be >= timeOfTimeCycle.
    int methIdx;
    LLBC_TimeSpan toTimeOfTimeCycle;
    if (timeCycle == LLBC_TimeSpan::oneHour)
        toTimeOfTimeCycle = (to.*LLBC_INL_NS __g_GetTimeOfTimeCycleMeths[methIdx = 0])();
    else if (timeCycle == LLBC_TimeSpan::oneDay)
        toTimeOfTimeCycle = (to.*LLBC_INL_NS __g_GetTimeOfTimeCycleMeths[methIdx = 1])();
    else // oneWeek
        toTimeOfTimeCycle = (to.*LLBC_INL_NS __g_GetTimeOfTimeCycleMeths[methIdx = 2])();

    if (toTimeOfTimeCycle < timeOfTimeCycle)
        return false;

    // Crossed timeCycle judge: fromTimeOfTimeCycle judge.
    const LLBC_TimeSpan fromTimeOfTimeCycle =
        (from.*LLBC_INL_NS __g_GetTimeOfTimeCycleMeths[methIdx])();
    return (fromTimeOfTimeCycle < timeOfTimeCycle) ||
         (fromTimeOfTimeCycle > timeOfTimeCycle &&
          toTimeOfTimeCycle < fromTimeOfTimeCycle);
}

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4996)
#endif
