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

#include "llbc/core/time/Time.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_TimeSpan &s)
{
    LLBC_NS LLBC_String str;
    str.format("%d seconds", s.GetTotalSeconds());

    return o <<str;
}

std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_Time &t)
{
    return o <<t.Format();
}

__LLBC_NS_BEGIN

LLBC_TimeSpan::LLBC_TimeSpan():m_span(0)
{
}

LLBC_TimeSpan::LLBC_TimeSpan(const LLBC_TimeSpan &span)
{
    m_span = span.m_span;
}

LLBC_TimeSpan::LLBC_TimeSpan(sint64 time)
{
    m_span = time;
}

LLBC_TimeSpan::LLBC_TimeSpan(int days, int hours, int mins, int secs)
{
    m_span = (days * 24 + hours) * 3600 + mins * 60 + secs;
}

LLBC_TimeSpan::~LLBC_TimeSpan()
{
}

int LLBC_TimeSpan::GetDays() const
{
    return (int)(m_span / (3600 * 24));
}

int LLBC_TimeSpan::GetTotalHours() const
{
    return (int)(m_span / 3600);
}

int LLBC_TimeSpan::GetHours() const
{
    int ret = (int)m_span;

    ret %= (3600 * 24);
    ret /= 3600;

    return ret;
}

int LLBC_TimeSpan::GetTotalMinutes() const
{
    int ret = (int)m_span / 60;
    return ret;
}

int LLBC_TimeSpan::GetMinutes() const
{
    int ret = (int)m_span;

    ret %= 3600;
    ret /= 60;

    return ret;
}

int LLBC_TimeSpan::GetTotalSeconds() const
{
    return (int)m_span;
}

int LLBC_TimeSpan::GetSeconds() const
{
    int ret = (int)m_span;

    ret %= 60;
    ret /= 1;

    return ret;
}

sint64 LLBC_TimeSpan::GetTimeSpan() const
{
    return m_span;
}

void LLBC_TimeSpan::SetTimeSpan(sint64 span)
{
    m_span = span;
}

LLBC_String LLBC_TimeSpan::Format(const char *format) const
{
    //! Not implement.
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return "";
}

LLBC_TimeSpan LLBC_TimeSpan::operator +(const LLBC_TimeSpan &span) const
{
    LLBC_TimeSpan spanRet(this->m_span + span.m_span);
    return spanRet;
}

LLBC_TimeSpan LLBC_TimeSpan::operator -(const LLBC_TimeSpan &span) const
{
    LLBC_TimeSpan spanRet(this->m_span - span.m_span);
    return spanRet;
}

LLBC_TimeSpan &LLBC_TimeSpan::operator +=(const LLBC_TimeSpan &span)
{
    this->m_span += span.m_span;
    return *this;
}

LLBC_TimeSpan &LLBC_TimeSpan::operator -=(const LLBC_TimeSpan &span)
{
    this->m_span -= span.m_span;
    return *this;
}

bool LLBC_TimeSpan::operator ==(const LLBC_TimeSpan &span) const
{
    return this->m_span == span.m_span;
}

bool LLBC_TimeSpan::operator !=(const LLBC_TimeSpan &span) const
{
    return !(*this == span);
}

bool LLBC_TimeSpan::operator <(const LLBC_TimeSpan &span) const
{
    return this->m_span < span.m_span;
}

bool LLBC_TimeSpan::operator >(const LLBC_TimeSpan &span) const
{
    return this->m_span > span.m_span;
}

bool LLBC_TimeSpan::operator <=(const LLBC_TimeSpan &span) const
{
    return this->m_span<= span.m_span;
}

bool LLBC_TimeSpan::operator >=(const LLBC_TimeSpan &span) const
{
    return this->m_span >= span.m_span;
}

LLBC_TimeSpan &LLBC_TimeSpan::operator =(const LLBC_TimeSpan &span)
{
    this->m_span = span.m_span;
    return *this;
}

LLBC_TimeSpan &LLBC_TimeSpan::operator =(sint64 span)
{
    this->m_span = span;
    return *this;
}

void LLBC_TimeSpan::Serialize(LLBC_Stream &stream) const
{
    stream.Write((int)m_span);
}

bool LLBC_TimeSpan::DeSerialize(LLBC_Stream &stream)
{
    sint32 spanVal = 0;
    if (!stream.Read(spanVal))
    {
        return false;
    }

    m_span = spanVal;

    return true;
}

void LLBC_TimeSpan::SerializeEx(LLBC_Stream &stream) const
{
    stream.WriteEx((int)m_span);
}

bool LLBC_TimeSpan::DeSerializeEx(LLBC_Stream &stream)
{
    int spanVal = 0;
    if (!stream.ReadEx(spanVal))
    {
        return false;
    }

    m_span = spanVal;

    return true;
}

const uint32 LLBC_Time::_numOfSecondsPerDay = 86400;
const uint32 LLBC_Time::_numOfMilliSecondsPerDay = 86400000;
const uint32 LLBC_Time::_numOfMilliSecondsPerSecond = 1000;

LLBC_Time::LLBC_Time()
{
    _time = (sint64)(time(NULL));
}

LLBC_Time::LLBC_Time(const LLBC_Time &time)
{
    _time = time._time;
}

LLBC_Time::LLBC_Time(time_t time)
{
    _time = time;
}

LLBC_Time::LLBC_Time(int year, int month, int day, int hour, int min, int sec, int tz)
{
    this->SetByPart(year, month, day, hour, min, sec, tz);
}

LLBC_Time::LLBC_Time(const char *timeStr,
                     char dateTimeSpliter,
                     char dateSpliter,
                     char timeSpliter,
                     int tz)
{
    if (!timeStr || strlen(timeStr) == 0)
    {
        timeStr = "1970-1-1 0:0:0";
        dateTimeSpliter = ' ';
        dateSpliter = '-';
        timeSpliter = ':';
    }

    char tempBuf[2] = {0};

    tempBuf[0] = dateTimeSpliter;
    std::vector<LLBC_String> dateTimes;
    LLBC_SplitString(timeStr, tempBuf, dateTimes, true);
    if (dateTimes.size() != 2)
    {
        dateTimes.push_back("0:0:0");
        timeSpliter = ':';
    }

    const std::string &datePart = dateTimes[0];
    const std::string &timePart = dateTimes[1];

    tempBuf[0] = dateSpliter;
    std::vector<LLBC_String> dateParts;
    LLBC_SplitString(datePart.c_str(), tempBuf, dateParts);
    for (size_t i = dateParts.size(); i < 3; i++)
    {
        dateParts.push_back("0");
    }
    tempBuf[0] = timeSpliter;
    std::vector<LLBC_String> timeParts;
    LLBC_SplitString(timePart.c_str(), tempBuf, timeParts);
    for (size_t i = timeParts.size(); i < 3; i++)
    {
        timeParts.push_back("0");
    }

    this->SetByPart(
        LLBC_Str2Int32(dateParts[0].c_str()),
        LLBC_Str2Int32(dateParts[1].c_str()),
        LLBC_Str2Int32(dateParts[2].c_str()),
        LLBC_Str2Int32(timeParts[0].c_str()),
        LLBC_Str2Int32(timeParts[1].c_str()),
        LLBC_Str2Int32(timeParts[2].c_str()),
        tz);
}

LLBC_Time::~LLBC_Time()
{
}

LLBC_Time LLBC_Time::GetCurrentTime()
{
    LLBC_Time time;
    return time;
}

uint32 LLBC_Time::GetNumOfSecondsInDay(uint32 days)
{
    return _numOfSecondsPerDay * days;
}

uint32 LLBC_Time::GetNumOfMilliSecondsInDay(uint32 days)
{
    return _numOfMilliSecondsPerDay * days;
}

uint32 LLBC_Time::GetNumOfMilliSecondsInSecond(uint32 seconds)
{
    return _numOfMilliSecondsPerSecond * seconds;
}

LLBC_TimeSpan LLBC_Time::GetIntervalTo(int secondsInDay)
{
    return LLBC_Time::GetIntervalTo(LLBC_Time::GetCurrentTime(), secondsInDay);
}

LLBC_TimeSpan LLBC_Time::GetIntervalTo(int hour, int min, int sec)
{
    return LLBC_Time::GetIntervalTo(LLBC_Time::GetCurrentTime(), hour, min, sec);
}

LLBC_TimeSpan LLBC_Time::GetIntervalTo(const LLBC_Time &fromTime, int secondsInDay)
{
    // Get past time(local time zone).
    time_t pastTimeLocal = fromTime.GetLocalTime();

    time_t spanVal = (secondsInDay - pastTimeLocal % LLBC_Time::GetNumOfSecondsInDay() + 
        LLBC_Time::GetNumOfSecondsInDay()) % LLBC_Time::GetNumOfSecondsInDay();

    return LLBC_TimeSpan(spanVal);
}

LLBC_TimeSpan LLBC_Time::GetIntervalTo(const LLBC_Time &fromTime, int hour, int min, int sec)
{
    LLBC_TimeSpan targetTime = LLBC_TimeSpan(0, hour, min, sec);
    return LLBC_Time::GetIntervalTo(fromTime, targetTime.GetTotalSeconds());
}

time_t LLBC_Time::GetGmtTime() const
{
    return static_cast<time_t>(_time);
}

time_t LLBC_Time::GetLocalTime() const
{
    return static_cast<time_t>(_time - LLBC_GetTimezone());
}

struct LLBC_TimeStruct *LLBC_Time::GetGmtTm(struct LLBC_TimeStruct *tm) const
{
    static struct LLBC_TimeStruct retTm;
    struct LLBC_TimeStruct tempTm;
    LLBC_GMTime(static_cast<time_t>(_time), tempTm);

    if (tm)
    {
        memcpy(tm, &tempTm, sizeof(struct LLBC_TimeStruct));
    }

    memcpy(&retTm, &tempTm, sizeof(struct LLBC_TimeStruct));

    return &retTm;
}

struct LLBC_TimeStruct *LLBC_Time::GetLocalTm(struct LLBC_TimeStruct *tm) const
{
    static struct LLBC_TimeStruct retTm;
    struct LLBC_TimeStruct tempTm;

    LLBC_LocalTime(static_cast<time_t>(_time), tempTm);

    if (tm)
    {
        memcpy(tm, &tempTm, sizeof(struct LLBC_TimeStruct));
    }

    memcpy(&retTm, &tempTm, sizeof(struct LLBC_TimeStruct));

    return &retTm;
}

int LLBC_Time::GetYear() const
{
    struct LLBC_TimeStruct tm;
    this->GetLocalTm(&tm);

    return tm.tm_year + 1900; // scene 1900
}

int LLBC_Time::GetMonth() const
{
    struct LLBC_TimeStruct tm;
    this->GetLocalTm(&tm);

    return tm.tm_mon + 1; // start by 0
}

int LLBC_Time::GetDay() const
{
    struct LLBC_TimeStruct tm;
    this->GetLocalTm(&tm);

    return tm.tm_mday;
}

int LLBC_Time::GetHour() const
{
    struct LLBC_TimeStruct tm;
    this->GetLocalTm(&tm);

    return tm.tm_hour;
}

int LLBC_Time::GetMinute() const
{
    struct LLBC_TimeStruct tm;
    this->GetLocalTm(&tm);

    return tm.tm_min;
}

int LLBC_Time::GetSecond() const
{
    struct LLBC_TimeStruct tm;
    this->GetLocalTm(&tm);

    return tm.tm_sec;
}

int LLBC_Time::GetDayOfWeek() const
{
    struct LLBC_TimeStruct tm;
    this->GetLocalTm(&tm);

    return tm.tm_wday;
}

LLBC_String LLBC_Time::Format(const char *format) const
{
    struct LLBC_TimeStruct tm;
    char buf[32];

    memset(buf, 0, sizeof(buf));

    this->GetLocalTm(&tm);
    if (!format)
    {
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", (struct tm *)&tm);
    }
    else
    {
        strftime(buf, sizeof(buf), format, (struct tm *)&tm);
    }

    return buf;
}

LLBC_String LLBC_Time::FormatGmt(const char *format) const
{
    struct LLBC_TimeStruct tm;
    char buf[32];

    memset(buf, 0, sizeof(buf));

    this->GetGmtTm(&tm);
    if (!format)
    {
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", (struct tm *)&tm);
    }
    else
    {
        strftime(buf, sizeof(buf), format, (struct tm *)&tm);
    }

    return buf;
}

LLBC_TimeSpan LLBC_Time::operator -(const LLBC_Time &time) const
{
    LLBC_TimeSpan ts(this->_time - time._time);
    return ts;
}

LLBC_TimeSpan LLBC_Time::operator +(const LLBC_Time &time) const
{
    LLBC_TimeSpan ts(this->_time - time._time);
    return ts;
}

LLBC_Time LLBC_Time::operator +(const LLBC_TimeSpan &span) const
{
    LLBC_Time retTime(*this);
    retTime._time += span.GetTotalSeconds();

    return retTime;
}

LLBC_Time LLBC_Time::operator -(const LLBC_TimeSpan &span) const
{
    LLBC_Time retTime(*this);
    retTime._time -= span.GetTotalSeconds();

    return retTime;
}

LLBC_Time &LLBC_Time::operator +=(const LLBC_TimeSpan &span)
{
    this->_time += span.GetTotalSeconds();
    return *this;
}

LLBC_Time &LLBC_Time::operator -=(const LLBC_TimeSpan &span)
{
    this->_time -= span.GetTotalSeconds();
    return *this;
}

bool LLBC_Time::operator ==(const LLBC_Time &time) const
{
    return this->_time == time._time;
}

bool LLBC_Time::operator !=(const LLBC_Time &time) const
{
    return !(*this == time);
}

bool LLBC_Time::operator <(const LLBC_Time &time) const
{
    return this->_time < time._time;
}

bool LLBC_Time::operator >(const LLBC_Time &time) const
{
    return this->_time > time._time;
}

bool LLBC_Time::operator >=(const LLBC_Time &time) const
{
    return this->_time >= time._time;
}

bool LLBC_Time::operator <=(const LLBC_Time &time) const
{
    return this->_time <= time._time;
}

LLBC_Time &LLBC_Time::operator =(time_t time)
{
    _time = time;
    return *this;
}

LLBC_Time &LLBC_Time::operator =(const LLBC_Time &time)
{
    if (this == &time)
    {
        return *this;
    }

    this->_time = time._time;
    return *this;
}

void LLBC_Time::Serialize(LLBC_Stream &stream) const
{
    stream.Write((unsigned int)_time);
}

bool LLBC_Time::DeSerialize(LLBC_Stream &stream)
{
    sint32 timeVal = 0;
    if (!stream.Read(timeVal))
    {
        return false;
    }

    _time = timeVal;

    return true;
}

void LLBC_Time::SerializeEx(LLBC_Stream &stream) const
{
    stream.Write(_time);
}

bool LLBC_Time::DeSerializeEx(LLBC_Stream &stream)
{
    if (!stream.Read(_time))
    {
        return false;
    }

    return true;
}

void LLBC_Time::SetByPart(int year, int month, int day, int hour, int min, int sec, int tz)
{
    static const bool leapYear[4] = {false, false, true, false};
    static const time_t mons[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

    struct LLBC_TimeStruct tm;
    memset(&tm, 0, sizeof(tm));

    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;
    tm.tm_yday = tm.tm_mday - 1 + (int)mons[tm.tm_mon];
    if ((tm.tm_mon + 1 > 2) && (leapYear[(tm.tm_year - 70) % 4]))
    {
        tm.tm_yday += 1;
    }

    // convert to gmt time.
    tm.tm_gmtoff = tz * 3600;
    _time = (sint64)LLBC_MkTime(tm);
}

__LLBC_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4996)
#endif

#include "llbc/common/AfterIncl.h"
