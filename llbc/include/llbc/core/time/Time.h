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

#include "llbc/core/time/TimeSpan.h"
#include "llbc/core/time/TimeConstant.h"

__LLBC_NS_BEGIN

class LLBC_Time;

__LLBC_NS_END

/**
 * Time class stream output operators previous declare.
 */
LLBC_EXPORT std::ostream &operator <<(std::ostream &stream, const LLBC_NS LLBC_Time &t);

__LLBC_NS_BEGIN

/**
 * \brief LLBC library Time class encapsulation.
 */
class LLBC_EXPORT LLBC_Time final
{
public:
    /**
     * Some constants variables define.
     */

     // UTC begin time.
    static const LLBC_Time UTCBegin;

    // Number of xxxxx per-day.
    static constexpr int NumOfSecondsPerDay = LLBC_TimeConstant::NumOfSecondsPerDay;
    static constexpr int NumOfMilliSecondsPerDay = LLBC_TimeConstant::NumOfMilliSecondsPerDay;
    static constexpr sint64 NumOfMicroSecondsPerDay = LLBC_TimeConstant::NumOfMicroSecondsPerDay;
    static constexpr sint64 NumOfNanoSecondsPerDay = LLBC_TimeConstant::NumOfNanoSecondsPerDay;

    // Number of xxx per-hour.
    static constexpr int NumOfSecondsPerHour = LLBC_TimeConstant::NumOfSecondsPerHour;
    static constexpr int NumOfMilliSecondsPerHour = LLBC_TimeConstant::NumOfMilliSecondsPerHour;
    static constexpr sint64 NumOfMicroSecondsPerHour = LLBC_TimeConstant::NumOfMicroSecondsPerHour;
    static constexpr sint64 NumOfNanoSecondsPerHour = LLBC_TimeConstant::NumOfNanoSecondsPerHour;

    // Number of xxx per-minute.
    static constexpr int NumOfSecondsPerMinute = LLBC_TimeConstant::NumOfSecondsPerMinute;
    static constexpr int NumOfMilliSecondsPerMinute = LLBC_TimeConstant::NumOfMilliSecondsPerMinute;
    static constexpr sint64 NumOfMicroSecondsPerMinute = LLBC_TimeConstant::NumOfMicroSecondsPerMinute;
    static constexpr sint64 NumOfNanoSecondsPerMinute = LLBC_TimeConstant::NumOfNanoSecondsPerMinute;

    // Number of xxx per-second.
    static constexpr int NumOfMilliSecondsPerSecond = LLBC_TimeConstant::NumOfMilliSecondsPerSecond;
    static constexpr sint64 NumOfMicroSecondsPerSecond = LLBC_TimeConstant::NumOfMicroSecondsPerSecond;
    static constexpr sint64 NumOfNanoSecondsPerSecond = LLBC_TimeConstant::NumOfNanoSecondsPerSecond;

    // Number of xxx per-millisecond.
    static constexpr sint64 NumOfMicroSecondsPerMilliSecond = LLBC_TimeConstant::NumOfMicroSecondsPerMilliSecond;
    static constexpr sint64 NumOfNanoSecondsPerMilliSecond = LLBC_TimeConstant::NumOfNanoSecondsPerMilliSecond;

    // Number of xxx per-microsecond.
    static constexpr sint64 NumOfNanoSecondsPerMicroSecond = LLBC_TimeConstant::NumOfNanoSecondsPerMicroSecond;

public:
    /**
     * Constructors.
     */
    LLBC_Time();
    LLBC_Time(const LLBC_Time &time);

    /**
     * Destructor.
     */
    ~LLBC_Time();

public:
    /**
     * Get current time.
     */
    static LLBC_Time Now();

    /**
     * Get now timestamp, in specific precision.
     * @return time_t/sint64 - now timestamp.
     */
    static time_t NowTimestampInSecs();
    static sint64 NowTimestampInMillis();
    static sint64 NowTimestampInMicros();

public:
    /**
     * Create time object from some ways.
     * @param[in] clanderTimeInXXX - calendar time, in seconds/milli-seconds/micro-seconds.
     * @param[in] timeVal          - calendar time in milliseconds representation by timeval struct.
     * @param[in] timeSpec         - calendar time in milliseconds representation by timespec struct.
     * @param[in] timeStr          - time string representation, for example: "2015-12-25 00:00:00.000".
     * @param[in] <time parts>     - the all time parts(year, month, day, ...). 
     * @return LLBC_Time - Time object.
     */
    static LLBC_Time FromSeconds(time_t clanderTimeInSeconds);
    static LLBC_Time FromMillis(sint64 clanderTimeInMilliSeconds);
    static LLBC_Time FromMicros(sint64 clanderTimeInMicroSeconds);
    static LLBC_Time FromTimeVal(const timeval &timeVal);
    static LLBC_Time FromTimeSpec(const timespec &timeSpec);
    static LLBC_Time FromTimeStr(const LLBC_String &timeStr);
    static LLBC_Time FromTimeStruct(const tm &timeStruct,
                                    int milliSecond = 0,
                                    int microSecond = 0);
    static LLBC_Time FromTimeParts(int year,
                                   int month,
                                   int day,
                                   int hour,
                                   int minute,
                                   int second,
                                   int milliSecond = 0,
                                   int microSecond = 0);

public:
    /**
     * Get local time parts(year, month, day, hour, minute, second, dayofweek).
     * Notes: GetMonth() start by 1, GetDayOfWeek() start by 0(sunday), GetDayOfYear() start by 1.
     * @return int - the specified time parts.
     */
    int GetYear() const;
    int GetMonth() const;
    int GetDay() const;
    int GetDayOfWeek() const;
    int GetDayOfMonth() const;
    int GetDayOfYear() const;
    int GetHour() const;
    int GetMinute() const;
    int GetSecond() const;
    int GetMilliSecond() const;
    int GetMicroSecond() const;

    /**
     * Get date part time.
     * @return LLBC_Time - the date part time.
     */
    LLBC_Time GetDate() const;
    /**
     * Get current time of day.
     * @return LLBC_TimeSpan - the current time of day.
     */
    LLBC_TimeSpan GetTimeOfDay() const;

public:
    /**
     * Get timetamp, in specific precision.
     * @return time_t/sint64 - the timestamp.
     */
    time_t GetTimestampInSecs() const;
    sint64 GetTimestampInMillis() const;
    sint64 GetTimestampInMicros() const;

public:
    /**
     * Get GMT time struct.
     * @param[out] timeStruct - time struct object reference.
     * @return const tm & - time struct object.
     */
    const tm &GetGmtTime() const;
    void GetGmtTime(tm &timeStruct) const;

    /**
     * Get local time struct.
     * @param[out] timeStruct - time struct reference.
     * @return const tm & - time struct object.
     */
    const tm &GetLocalTime() const;
    void GetLocalTime(tm &timeStruct) const;

public:
    /**
     * Format local time, see strftime() api.
     */
    LLBC_String Format(const char *format = nullptr) const;
    static LLBC_String Format(const time_t &clanderTimeInSeconds, const char *format);

    /**
     * Format gmt time, see strftime() api.
     */
    LLBC_String FormatAsGmt(const char *format = nullptr) const;
    static LLBC_String FormatAsGmt(const time_t &clanderTimeInSeconds, const char *format);

public:
    /**
     * Add specified time parts values.
     * Notes: These operations are thread-safe, all add parts added to new LLBC_Time object.
     * @param[in] <time parts> - the all time parts(year, month, day, ...).
     * @return LLBC_Time - the new time object.
     */
    LLBC_Time AddYears(int years) const;
    LLBC_Time AddMonths(int months) const;
    LLBC_Time AddDays(int days) const;
    LLBC_Time AddHours(int hours) const;
    LLBC_Time AddMinutes(int minutes) const;
    LLBC_Time AddSeconds(int seconds) const;
    LLBC_Time AddMilliSeconds(int milliSeconds) const;
    LLBC_Time AddMicroSeconds(int microSeconds) const;

public:
    /**
     * Makesure given year is leap year or not.
     * @param[in] year - the given year.
      *@return bool - return true it means given year is leap year, otherwise not.
     */
    static bool IsLeapYear(int year);

    /**
     * Get specific month max days.
     * @param[in] year  - the year.
     * @param[in] month - the month.
     * @return int - the specific month max days, if failed, return 0.
     */
    static int GetMonthMaxDays(int year, int month);

    /**
     * Get specific month span days.
     * @param[in] year  - the year.
     * @param[in] month - the month - [1, 12].
     * @return int - the month span days(not included giving month).
     */
    static int GetMonthSpanDays(int year, int month);

public:
    /**
     * Get remaining seconds to nearest day special monent.
     * @param[in] span        - span value.
     * @param[in] hour        - hour.
     * @param[in] minute      - minute.
     * @param[in] second      - second.
     * @param[in] milliSecond - milli-second.
     * @param[in] microSecond - micro-second.
     * @param[in] from        - from time.
     * @return LLBC_TimeSpan - timespan value.
     */
    LLBC_TimeSpan GetIntervalTo(const LLBC_TimeSpan &span) const;
    LLBC_TimeSpan GetIntervalTo(int hour,
                                int minute,
                                int second,
                                int milliSecond = 0,
                                int microSecond = 0) const;
    static LLBC_TimeSpan GetIntervalTo(const LLBC_Time &from, const LLBC_TimeSpan &span);
    static LLBC_TimeSpan GetIntervalTo(const LLBC_Time &from,
                                       int hour, int minute,
                                       int second,
                                       int milliSecond = 0,
                                       int microSecond = 0);

public:
    /**
     * Time span operations.
     */
    LLBC_TimeSpan operator -(const LLBC_Time &time) const;
    LLBC_TimeSpan operator +(const LLBC_Time &time) const;

    LLBC_Time operator +(const LLBC_TimeSpan &span) const;
    LLBC_Time operator -(const LLBC_TimeSpan &span) const;

    bool operator ==(const LLBC_Time &time) const;
    bool operator !=(const LLBC_Time &time) const;
    bool operator <(const LLBC_Time &time) const;
    bool operator >(const LLBC_Time &time) const;
    bool operator <=(const LLBC_Time &time) const;
    bool operator >=(const LLBC_Time &time) const;

    LLBC_Time &operator =(const LLBC_Time &time);

public:
    /**
     * Get the time object string representation.
     * @return LLBC_String - the object string representation.
     */
    LLBC_String ToString() const;

public:
    /**
     * Stream output operator support.
     */
    friend std::ostream & ::operator <<(std::ostream &stream, const LLBC_Time &t);

public:
    /**
     * Serialize / DeSerialize support.
     */
    void Serialize(LLBC_Stream &stream) const;
    bool DeSerialize(LLBC_Stream &stream);

private:
    explicit LLBC_Time(const sint64 &clanderTimeInMicroSeconds);

    void UpdateTimeStructs();

private:
    sint64 _time;
    tm _gmtTimeStruct;
    tm _localTimeStruct;
};

__LLBC_NS_END

#include "llbc/core/time/TimeInl.h"


