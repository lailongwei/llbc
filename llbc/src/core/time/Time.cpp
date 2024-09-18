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

LLBC_Time LLBC_Time::FromTimeStruct(const tm &timeStruct,
                                    int milliSec,
                                    int microSec)
{
    time_t calendarTimeInSecs = mktime(const_cast<tm *>(&timeStruct));
    return LLBC_Time(calendarTimeInSecs * LLBC_TimeConst::numOfMicrosPerSecond +
                     milliSec * LLBC_TimeConst::numOfMicrosPerMillisecond +
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
        int totalSeconds = hour * LLBC_TimeConst::numOfSecondsPerHour +
            minute * LLBC_TimeConst::numOfSecondsPerMinute + second;
        if (tz < 0 && totalSeconds < -tz)
        {
            hour = -tz / LLBC_TimeConst::numOfSecondsPerHour;
            minute = (-tz % LLBC_TimeConst::numOfSecondsPerHour) /
                LLBC_TimeConst::numOfSecondsPerMinute;
            second = -tz % LLBC_TimeConst::numOfSecondsPerMinute;
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
        _time / LLBC_TimeConst::numOfMicrosPerMillisecond - 
        LLBC_GetTimezone() * LLBC_TimeConst::numOfMillisPerSecond;
    return static_cast<int>(
        nowLocalMillis % LLBC_TimeConst::numOfMillisPerSecond);
}

int LLBC_Time::GetMicrosecond() const
{
    sint64 nowLocalMicros =
        _time - LLBC_GetTimezone() * LLBC_TimeConst::numOfMicrosPerSecond;
    return static_cast<int>(
        nowLocalMicros % LLBC_TimeConst::numOfMicrosPerMillisecond);
}

LLBC_Time LLBC_Time::GetDate() const
{
    sint64 timeZone = LLBC_GetTimezone() * LLBC_TimeConst::numOfMicrosPerSecond;

    sint64 localTime = _time - timeZone;
    sint64 datePart = localTime /
        LLBC_TimeConst::numOfMicrosPerDay *
        LLBC_TimeConst::numOfMicrosPerDay;

    datePart += timeZone;

    return LLBC_Time(datePart);
}

LLBC_TimeSpan LLBC_Time::GetTimeOfHour() const
{
    return LLBC_TimeSpan(
        (_time - (LLBC_GetTimezone() * LLBC_TimeConst::numOfMicrosPerSecond)) %
            LLBC_TimeConst::numOfMicrosPerHour);
}

LLBC_TimeSpan LLBC_Time::GetTimeOfDay() const
{
    return LLBC_TimeSpan(
        (_time - (LLBC_GetTimezone() * LLBC_TimeConst::numOfMicrosPerSecond)) %
            LLBC_TimeConst::numOfMicrosPerDay);
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
    return *this + LLBC_TimeSpan(days * LLBC_TimeConst::numOfMicrosPerDay);
}

LLBC_Time LLBC_Time::AddHours(int hours) const
{
    return *this + LLBC_TimeSpan(hours * LLBC_TimeConst::numOfMicrosPerHour);
}

LLBC_Time LLBC_Time::AddMinutes(int minutes) const
{
    return *this + LLBC_TimeSpan(minutes * LLBC_TimeConst::numOfMicrosPerMinute);
}

LLBC_Time LLBC_Time::AddSeconds(int seconds) const
{
    return *this + LLBC_TimeSpan(seconds * LLBC_TimeConst::numOfMicrosPerSecond);
}

LLBC_Time LLBC_Time::AddMillis(int millis) const
{
    return *this + LLBC_TimeSpan(millis * LLBC_TimeConst::numOfMicrosPerMillisecond);
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
                           LLBC_TimeConst::numOfMicrosPerMillisecond + GetMicrosecond());
}

bool LLBC_Time::Deserialize(LLBC_Stream &stream)
{
    sint64 timeVal = 0;
    if (!stream.Read(timeVal))
        return false;

    _time = timeVal;
    UpdateTimeStructs();

    return true;
}

#define __LLBC_INL_TIME_STR_PART_TO_VAL(partStr, partLen, timePartVar) \
    if (UNLIKELY(partLen >= sizeof(numFmtBuf)))                        \
        return utcBegin;                                               \
                                                                       \
    if (LIKELY(partLen > 0))                                           \
    {                                                                  \
        memcpy(numFmtBuf, partStr, partLen);                           \
        numFmtBuf[partLen] = '\0';                                     \
        timePartVar = atoi(numFmtBuf);                                 \
    }                                                                  \

#define __LLBC_INL_TIME_PARSE_SEPARATORS(beg, end, sep) \
    strIt = beg;                                        \
    sepSize = 0;                                        \
    while (strIt != end)                                \
    {                                                   \
        if (*strIt == sep)                              \
        {                                               \
            sepPoses[sepSize++] = strIt;                \
            if (sepSize == 2)                           \
                break;                                  \
        }                                               \
                                                        \
        ++strIt;                                        \
    }                                                   \

LLBC_Time LLBC_Time::FromTimeStr(const char *timeStr, size_t timeStrLen)
{
    // Time string format:
    // - yy-mm-dd HH:MM:SS[.micro_sec]
    // 
    // eg:
    //                                 yyyy mm dd  HH MM SS micro_sec
    // 03 =>                                             03  000000
    // 02:03 =>                                       02 03  000000
    // 01:02:03 =>                                 01 02 03  000000
    // 08 01:02:03 =>                          08  01 02 03  000000
    // 07-08 01:02:03 =>                    07 08  01 02 03  000000
    // 06-07-08 01:02:03 =>              06 07 08  01 02 03  000000
    // 2006-07-08 01:02:03 =>          2006 07 08  01 02 03  000000
    // 2006-07-08 01:02:03.123456 =>   2006 07 08  01 02 03  123456

    // Argument check.
    if (UNLIKELY(!timeStr || timeStrLen == 0))
        return utcBegin;

    // Remove leading whitespace characters.
    const char *originTimeStr = timeStr;
    while (*timeStr != '\0')
    {
        if (LIKELY(!LLBC_IsSpace(*timeStr)))
            break;
        ++timeStr;
    }

    if (*timeStr == '\0')
        return utcBegin;
    timeStrLen -= static_cast<size_t>(timeStr - originTimeStr);

    // Remove trailing whitespace characters.
    const char *timeStrEnd = timeStr + timeStrLen - 1;
    while (timeStrEnd != timeStr && LLBC_IsSpace(*timeStrEnd))
        --timeStrEnd;
    ++timeStrEnd;

    // Define time string parse helper variables.
    size_t sepSize;
    const char *strIt;
    const char *sepPoses[2]{nullptr, nullptr};

    // Parse <Date> part.
    char numFmtBuf[12];
    int year = 1970, month = 1, day = 1;
    const char *datePartEnd = timeStr;
    while (++datePartEnd != timeStrEnd)
        LLBC_BreakIf(LLBC_IsSpace(*datePartEnd));
    if (datePartEnd != timeStrEnd)
    {
        __LLBC_INL_TIME_PARSE_SEPARATORS(timeStr, datePartEnd, '-')
        if (sepSize == 2)
        {
            __LLBC_INL_TIME_STR_PART_TO_VAL(
                timeStr, static_cast<size_t>(sepPoses[0] - timeStr), year);
            __LLBC_INL_TIME_STR_PART_TO_VAL(
                sepPoses[0] + 1, static_cast<size_t>(sepPoses[1] - sepPoses[0] - 1), month);
            __LLBC_INL_TIME_STR_PART_TO_VAL(
                sepPoses[1] + 1, static_cast<size_t>(datePartEnd - sepPoses[1] - 1), day);
        }
        else if (sepSize == 1)
        {
            __LLBC_INL_TIME_STR_PART_TO_VAL(
                timeStr, static_cast<size_t>(sepPoses[0] - timeStr), month);
            __LLBC_INL_TIME_STR_PART_TO_VAL(
                sepPoses[0] + 1, static_cast<size_t>(datePartEnd - sepPoses[0] - 1), day);
        }
        else
        {
            __LLBC_INL_TIME_STR_PART_TO_VAL(
                timeStr, static_cast<size_t>(datePartEnd - timeStr), day);
        }
    }

    // Parse <TimeOfDay> part.
    int hour = 0, minute = 0, second = 0, microSec = 0;
    if (datePartEnd != timeStrEnd)
    {
        timeStr = datePartEnd + 1;
        while (*timeStr != '\0')
        {
            if (LIKELY(!LLBC_IsSpace(*timeStr)))
                break;
            ++timeStr;
        }
    }

    __LLBC_INL_TIME_PARSE_SEPARATORS(timeStr, timeStrEnd, ':')

    const char *secPart;
    if (sepSize == 2)
    {
        __LLBC_INL_TIME_STR_PART_TO_VAL(
            timeStr, static_cast<size_t>(sepPoses[0] - timeStr), hour);
        __LLBC_INL_TIME_STR_PART_TO_VAL(
            sepPoses[0] + 1, static_cast<size_t>(sepPoses[1] - sepPoses[0] - 1), minute);
        secPart = sepPoses[1] + 1;
    }
    else if (sepSize == 1)
    {
        __LLBC_INL_TIME_STR_PART_TO_VAL(
            timeStr, static_cast<size_t>(sepPoses[0] - timeStr), minute);
        secPart = sepPoses[0] + 1;
    }
    else
    {
        secPart = timeStr;
    }

    const char *dotPos = secPart;
    while (dotPos != timeStrEnd)
    {
        if (*dotPos == '.')
            break;

        ++dotPos;
    }

    const char *secPartEnd = dotPos != timeStrEnd ? dotPos : timeStrEnd;
    __LLBC_INL_TIME_STR_PART_TO_VAL(secPart, static_cast<size_t>(secPartEnd - secPart), second);
    if (dotPos != timeStrEnd)
    {
        __LLBC_INL_TIME_STR_PART_TO_VAL(dotPos + 1, static_cast<size_t>(timeStrEnd - dotPos - 1), microSec);
    }

    return FromTimeParts(year, month, day, hour, minute, second, microSec / 1000, microSec % 1000);
}

#undef __LLBC_INL_TIME_STR_PART_TO_VAL
#undef __LLBC_INL_TIME_PARSE_SEPARATORS

void LLBC_Time::UpdateTimeStructs()
{
    time_t calendarTime = static_cast<time_t>(_time / LLBC_TimeConst::numOfMicrosPerSecond);
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
