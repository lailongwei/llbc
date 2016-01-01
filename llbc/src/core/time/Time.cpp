/**
 * @file    LLBC_Time.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/05
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Time.h"
#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/time/TimeSpan.h"
#include "llbc/core/time/Time.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

std::ostream &operator <<(std::ostream &stream, const LLBC_NS LLBC_Time &span)
{
    return stream <<span.Format();
}

namespace
{
    typedef LLBC_NS LLBC_Time This;
}

__LLBC_NS_BEGIN

const int LLBC_Time::NumOfSecondsPerDay = 86400;
const int LLBC_Time::NumOfMilliSecondsPerDay = 86400000;
#if LLBC_TARGET_PLATFORM_WIN32
const sint64 LLBC_Time::NumOfMicroSecondsPerDay = 86400000000I64;
#else
const sint64 LLBC_Time::NumOfMicroSecondsPerDay = 86400000000L64;
#endif

const int LLBC_Time::NumOfSecondsPerHour = 3600;
const int LLBC_Time::NumOfMilliSecondsPerHour = 3600000;
const sint64 LLBC_Time::NumOfMicroSecondsPerHour = 3600000000;

const int LLBC_Time::NumOfSecondsPerMinute = 60;
const int LLBC_Time::NumOfMilliSecondsPerMinute = 60000;
const sint64 LLBC_Time::NumOfMicroSecondsPerMinute = 60000000;

const int LLBC_Time::NumOfMilliSecondsPerSecond = 1000;
const sint64 LLBC_Time::NumOfMicroSecondsPerSecond = 1000000;

const sint64 LLBC_Time::NumOfMicroSecondsPerMilliSecond = 1000;

LLBC_Time::LLBC_Time()
: _time()
{
    UpdateTimeStructs();
}

LLBC_Time::LLBC_Time(const LLBC_Time &time)
: _time(time._time)
{
    memcpy(&_localTimeStruct, &time._localTimeStruct, sizeof(struct tm));
    memcpy(&_gmtTimeStruct, &time._gmtTimeStruct, sizeof(struct tm));
}

LLBC_Time::LLBC_Time(double clanderTime)
: _time(clanderTime)
{
    UpdateTimeStructs();
}

LLBC_Time::~LLBC_Time()
{
}

LLBC_Time LLBC_Time::Now()
{
    return LLBC_Time(static_cast<double>(LLBC_GetMicroSeconds()) / NumOfMicroSecondsPerSecond);
}

LLBC_Time LLBC_Time::Current()
{
    return LLBC_Time(static_cast<double>(LLBC_GetMicroSeconds()) / NumOfMicroSecondsPerSecond);
}

int LLBC_Time::GetYear() const
{
    return _localTimeStruct.tm_year + 1900; // scene 1900
}

int LLBC_Time::GetMonth() const
{
    return _localTimeStruct.tm_mon + 1; // start by 1
}

int LLBC_Time::GetDay() const
{
    return _localTimeStruct.tm_mday;
}

int LLBC_Time::GetDayOfWeek() const
{
    return _localTimeStruct.tm_wday;
}

int LLBC_Time::GetDayOfYear() const
{
    return _localTimeStruct.tm_yday + 1; // start by 1
}

int LLBC_Time::GetHour() const
{
    return _localTimeStruct.tm_hour;
}

int LLBC_Time::GetMinute() const
{
    return _localTimeStruct.tm_min;
}

int LLBC_Time::GetSecond() const
{
    return _localTimeStruct.tm_sec;
}

int LLBC_Time::GetMilliSecond() const
{
    sint64 nowLocal = static_cast<sint64>((_time - LLBC_GetTimezone()) * NumOfMilliSecondsPerSecond);
    return static_cast<int>(nowLocal % NumOfMilliSecondsPerSecond);
}

int LLBC_Time::GetMicroSecond() const
{
    sint64 nowLocal = static_cast<sint64>((_time - LLBC_GetTimezone()) * NumOfMicroSecondsPerSecond);
    return static_cast<int>(nowLocal % NumOfMicroSecondsPerMilliSecond);
}

LLBC_Time LLBC_Time::GetDate() const
{
    int timeZone = LLBC_GetTimezone();

    double localTime = _time - timeZone;
    sint64 datePart = static_cast<sint64>(
        localTime) / NumOfSecondsPerDay * NumOfSecondsPerDay;

    datePart += timeZone;

    return LLBC_Time(static_cast<double>(datePart));
}

LLBC_TimeSpan LLBC_Time::GetTimeOfDay() const
{
    double localTime = _time - LLBC_GetTimezone();
    sint64 localTimeInMicroSecond = static_cast<
        sint64>(localTime * NumOfMicroSecondsPerSecond);

    sint64 timePart = localTimeInMicroSecond % NumOfMicroSecondsPerDay;
    return LLBC_TimeSpan(static_cast<double>(timePart) / NumOfMicroSecondsPerSecond);
}

sint64 LLBC_Time::GetTimeTicks() const
{
    return static_cast<sint64>(_time * NumOfMicroSecondsPerSecond);
}

const struct tm &LLBC_Time::GetGmtTime() const
{
    return _gmtTimeStruct;
}

void LLBC_Time::GetGmtTime(struct tm &timeStruct) const
{
    memcpy(&timeStruct, &_gmtTimeStruct, sizeof(struct tm));
}

const struct tm &LLBC_Time::GetLocalTime() const
{
    return _localTimeStruct;
}

void LLBC_Time::GetLocalTime(struct tm &timeStruct) const
{
    memcpy(&timeStruct, &_localTimeStruct, sizeof(struct tm));
}

LLBC_String LLBC_Time::Format(const char *format) const
{
    char buf[32];
    memset(buf, 0, sizeof(buf));

    if (format)
        strftime(buf, sizeof(buf), format, &_localTimeStruct);
    else
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &_localTimeStruct);

    return buf;
}

LLBC_String LLBC_Time::Format(double time, const char *format)
{
    return FromSeconds(time).Format(format);
}

LLBC_String LLBC_Time::FormatAsGmt(const char *format) const
{
    char buf[32];
    memset(buf, 0, sizeof(buf));

    if (format)
        strftime(buf, sizeof(buf), format, &_gmtTimeStruct);
    else
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &_gmtTimeStruct);

    return buf;
}

LLBC_String LLBC_Time::FormatAsGmt(double time, const char *format)
{
    return FromSeconds(time).FormatAsGmt(format);
}

LLBC_Time LLBC_Time::FromSeconds(double time)
{
    return LLBC_Time(time);
}

LLBC_Time LLBC_Time::FromMilliSeconds(sint64 time)
{
    return LLBC_Time(static_cast<double>(time) / NumOfMilliSecondsPerSecond);
}

LLBC_Time LLBC_Time::FromMicroSeconds(sint64 time)
{
    return LLBC_Time(static_cast<double>(time) / NumOfMicroSecondsPerSecond);
}

LLBC_Time LLBC_Time::FromTimeVal(const struct timeval &timeVal)
{
    return LLBC_Time(timeVal.tv_sec + 
        static_cast<double>(timeVal.tv_usec) / NumOfMicroSecondsPerSecond);
}

LLBC_Time LLBC_Time::FromTimeRepr(LLBC_String timeRepr)
{
    if (UNLIKELY(timeRepr.empty()))
        return LLBC_Time();

    // Split date, time
    std::vector<LLBC_String> dateTimes;
    LLBC_SplitString(timeRepr, " ", dateTimes, true);
    if (dateTimes.size() != 2)
        dateTimes.push_back("0:0:0.000");

    const std::string &datePart = dateTimes[0];
    const std::string &timePart = dateTimes[1];

    // Split year,month,day hour,minute,second
    std::vector<LLBC_String> dateParts;
    LLBC_SplitString(datePart.c_str(), "-", dateParts);
    for (size_t i = dateParts.size(); i < 3; i++)
        dateParts.push_back("0");

    std::vector<LLBC_String> timeParts;
    LLBC_SplitString(timePart.c_str(), ":", timeParts);
    for (size_t i = timeParts.size(); i < 3; i++)
        timeParts.push_back("0");

    // Split time, microseconds 
    std::vector<LLBC_String> secondParts;
    LLBC_SplitString(timeParts[2], ".", secondParts, true);
    if (secondParts.size() == 1)
        secondParts.push_back("0");

    // Convert it
    int year = LLBC_Str2Int32(dateParts[0].c_str());
    int month = LLBC_Str2Int32(dateParts[1].c_str());
    int day = LLBC_Str2Int32(dateParts[2].c_str());

    int hour = LLBC_Str2Int32(timeParts[0].c_str());
    int minute = LLBC_Str2Int32(timeParts[1].c_str());
    int second = LLBC_Str2Int32(timeParts[2].c_str());
    int microSecond = LLBC_Str2Int32(secondParts[1].c_str());

    return FromTimeParts(year,
                         month,
                         day,
                         hour,
                         minute,
                         second,
                         static_cast<int>(microSecond / NumOfMicroSecondsPerMilliSecond),
                         static_cast<int>(microSecond % NumOfMicroSecondsPerMilliSecond));
}

LLBC_Time LLBC_Time::FromTimeStruct(const struct tm &timeStruct, int milliSecond, int microSecond)
{
    double calendarTime = static_cast<double>(::mktime(const_cast<struct tm *>(&timeStruct)));

    double timeVal = calendarTime + 
        static_cast<double>(milliSecond) / NumOfMilliSecondsPerSecond +
        static_cast<double>(microSecond) / NumOfMicroSecondsPerSecond;

    return LLBC_Time(timeVal);
}

LLBC_Time LLBC_Time::FromTimeParts(int year, int month, int day, int hour, int minute, int second, int milliSecond, int microSecond)
{
    struct tm timeStruct;
    timeStruct.tm_year = year - 1900;
    timeStruct.tm_mon = month - 1;
    timeStruct.tm_mday = day;

    timeStruct.tm_hour = hour;
    timeStruct.tm_min = minute;
    timeStruct.tm_sec = second;

    return FromTimeStruct(timeStruct, milliSecond, microSecond);
}

LLBC_Time LLBC_Time::AddYears(int years) const
{
    if (years == 0)
        return *this;

    struct tm newTimeStruct;
    GetLocalTime(newTimeStruct);

    newTimeStruct.tm_year += years;
    bool isLeap = IsLeapYear(GetYear());
    if (isLeap && 
        GetMonth() == 2 && GetDay() == 29)
    {
        if (!IsLeapYear(GetYear() + years))
            newTimeStruct.tm_mday -= 1;
    }

    return FromTimeStruct(newTimeStruct, GetMilliSecond(), GetMicroSecond());
}

LLBC_Time LLBC_Time::AddMonths(int months) const
{
    LLBC_Time yearAddedTime = AddYears(months / 12);

    months %= 12;
    struct tm newTimeStruct;
    yearAddedTime.GetLocalTime(newTimeStruct);

    int remainingMonth = 11 - newTimeStruct.tm_mon;
    if (months > remainingMonth)
    {
        newTimeStruct.tm_year += 1;
        newTimeStruct.tm_mon = months - (12 - newTimeStruct.tm_mon);
    }
    else
    {
        newTimeStruct.tm_mon += months;
    }
    newTimeStruct.tm_mday = MIN(newTimeStruct.tm_mday, 
        GetMonthMaxDays(yearAddedTime.GetYear(), newTimeStruct.tm_mon + 1));

    return FromTimeStruct(newTimeStruct, GetMilliSecond(), GetMicroSecond());
}

LLBC_Time LLBC_Time::AddDays(int days) const
{
    return *this + LLBC_TimeSpan(days * NumOfSecondsPerDay);
}

LLBC_Time LLBC_Time::AddHours(int hours) const
{
    return *this + LLBC_TimeSpan(hours * NumOfSecondsPerHour);
}

LLBC_Time LLBC_Time::AddMinutes(int minutes) const
{
    return *this + LLBC_TimeSpan(minutes * NumOfSecondsPerMinute);
}

LLBC_Time LLBC_Time::AddSeconds(int seconds) const
{
    return *this + LLBC_TimeSpan(seconds);
}

LLBC_Time LLBC_Time::AddMilliSeconds(int milliSeconds) const
{
    return *this + LLBC_TimeSpan(static_cast<double>(milliSeconds) / NumOfMilliSecondsPerSecond);
}

LLBC_Time LLBC_Time::AddMicroSeconds(int microSeconds) const
{
    return *this + LLBC_TimeSpan(static_cast<double>(microSeconds) / NumOfMicroSecondsPerSecond);
}

bool LLBC_Time::IsLeapYear(int year)
{
    return LLBC_IsLeapYear(year);
}

int LLBC_Time::GetMonthMaxDays(int year, int month)
{
    return LLBC_GetMonthMaxDays(year, month);
}

LLBC_TimeSpan LLBC_Time::GetIntervalTo(double secondsInDay)
{
    return LLBC_Time::GetIntervalTo(LLBC_Time::Current(), secondsInDay);
}

LLBC_TimeSpan LLBC_Time::GetIntervalTo(int hour, int minute, int second, int milliSecond, int microSecond)
{
    return LLBC_Time::GetIntervalTo(LLBC_Time::Current(), hour, minute, second, milliSecond, microSecond);
}

LLBC_TimeSpan LLBC_Time::GetIntervalTo(const LLBC_Time &fromTime, double secondsInDay)
{
    // Get past time(local time zone).
    double localTime = fromTime._time - LLBC_GetTimezone();
    double secondsPart, nonSecondsPart;
    nonSecondsPart = modf(localTime, &secondsPart);

    double pastTimeLocal = 
        static_cast<sint64>(secondsPart) % NumOfSecondsPerDay + nonSecondsPart;

    // Calculate span value.
    double spanVal = secondsInDay - pastTimeLocal;
    if (spanVal < 0)
        spanVal = NumOfSecondsPerDay + spanVal;
    else
        spanVal = NumOfSecondsPerDay - spanVal;

    return LLBC_TimeSpan(spanVal);
}

LLBC_TimeSpan LLBC_Time::GetIntervalTo(const LLBC_Time &fromTime, int hour, int minute, int second, int milliSecond, int microSecond)
{
    LLBC_TimeSpan span(0, hour, minute, second, milliSecond, microSecond);
    return GetIntervalTo(fromTime, span.GetTotalSeconds());
}

LLBC_TimeSpan LLBC_Time::operator -(const LLBC_Time &time) const
{
    LLBC_TimeSpan ts(_time - time._time);
    return ts;
}

LLBC_TimeSpan LLBC_Time::operator +(const LLBC_Time &time) const
{
    LLBC_TimeSpan ts(_time - time._time);
    return ts;
}

LLBC_Time LLBC_Time::operator +(const LLBC_TimeSpan &span) const
{
    return LLBC_Time(_time + span.GetTotalSeconds());
}

LLBC_Time LLBC_Time::operator -(const LLBC_TimeSpan &span) const
{
    return LLBC_Time(_time - span.GetTotalSeconds());
}

bool LLBC_Time::operator ==(const LLBC_Time &time) const
{
    return _time == time._time;
}

bool LLBC_Time::operator !=(const LLBC_Time &time) const
{
    return _time != time._time;
}

bool LLBC_Time::operator <(const LLBC_Time &time) const
{
    return _time < time._time;
}

bool LLBC_Time::operator >(const LLBC_Time &time) const
{
    return _time > time._time;
}

bool LLBC_Time::operator >=(const LLBC_Time &time) const
{
    return _time >= time._time;
}

bool LLBC_Time::operator <=(const LLBC_Time &time) const
{
    return _time <= time._time;
}

LLBC_Time &LLBC_Time::operator =(const LLBC_Time &time)
{
    if (this == &time)
        return *this;
    else if (*this == time)
        return *this;

    _time = time._time;
    memcpy(&_localTimeStruct, &time._localTimeStruct, sizeof(struct tm));
    memcpy(&_gmtTimeStruct, &time._gmtTimeStruct, sizeof(struct tm));

    return *this;
}

void LLBC_Time::Serialize(LLBC_Stream &stream) const
{
    stream.Write(_time);
}

bool LLBC_Time::DeSerialize(LLBC_Stream &stream)
{
    double timeVal = 0;
    if (!stream.Read(timeVal))
        return false;

    _time = timeVal;
    UpdateTimeStructs();

    return true;
}

void LLBC_Time::SerializeEx(LLBC_Stream &stream) const
{
    stream.WriteEx(_time);
}

bool LLBC_Time::DeSerializeEx(LLBC_Stream &stream)
{
    double timeVal = 0;
    if (!stream.ReadEx(timeVal))
        return false;

    _time = timeVal;
    UpdateTimeStructs();

    return true;
}

void LLBC_Time::UpdateTimeStructs()
{
    time_t calendarTime = (time_t)_time;
    ::localtime_s(&_localTimeStruct, &calendarTime);
    ::gmtime_s(&_gmtTimeStruct, &calendarTime);
}

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4996)
#endif

#include "llbc/common/AfterIncl.h"
