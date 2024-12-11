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

__LLBC_NS_BEGIN

class LLBC_Time;

__LLBC_NS_END

/**
 * Time class stream output operators previous declare.
 */
LLBC_EXPORT std::ostream &operator<<(std::ostream &stream, const LLBC_NS LLBC_Time &t);

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
    static const LLBC_Time utcBegin;

public:
    /**
     * Constructors.
     */
    LLBC_Time();
    LLBC_Time(const LLBC_Time &time);

    /**
     * Destructor.
     */
    ~LLBC_Time() = default;

public:
    /**
     * Get current time.
     */
    static LLBC_Time Now();

    /**
     * Get now timestamp, in seconds.
     * @return time_t - now timestamp, in seconds.
     */
    static time_t NowTimestampInSecs();

    /**
     * Get now timestamp, in milli-seconds.
     * @return time_t - now timestamp, in milli-seconds.
     */
    static sint64 NowTimestampInMillis();

    /**
     * Get now timestamp, in micro-seconds.
     * @return time_t - now timestamp, in micro-seconds.
     */
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
    static LLBC_Time FromMillis(sint64 clanderTimeInMillis);
    static LLBC_Time FromMicros(sint64 clanderTimeInMicros);
    static LLBC_Time FromTimeVal(const timeval &timeVal);
    static LLBC_Time FromTimeSpec(const timespec &timeSpec);
    template <size_t _StrArrLen>
    static LLBC_Time FromTimeStr(const char (&timeStr)[_StrArrLen]);
    static LLBC_Time FromTimeStr(const char *timeStr);
    template <typename _StrType>
    static typename std::enable_if<LLBC_IsTemplSpec<_StrType, std::basic_string>::value, LLBC_Time>::type
    FromTimeStr(const _StrType &timeStr);
    static LLBC_Time FromTimeStr(const LLBC_String &timeStr);
    static LLBC_Time FromTimeStruct(const tm &timeStruct,
                                    int milliSec = 0,
                                    int microSec = 0);
    static LLBC_Time FromTimeParts(int year,
                                   int month,
                                   int day,
                                   int hour,
                                   int minute,
                                   int second,
                                   int milliSec = 0,
                                   int microSec = 0);

public:
    /**
     * Get year.
     * @return int - year.
     */
    int GetYear() const;

    /**
     * Get month, started by 0.
     * \return int - month.
     */
    int GetMonth() const;

    /**
     * Get day of week, started by 0(sunday).
     * @return int - day of week.
     */
    int GetDayOfWeek() const;

    /**
     * Get day of month, started by 1.
     * @return int - day of month.
     */
     int GetDayOfMonth() const;

    /**
     * Get day of year, started by 0.
     * @return int - day of year.
     */
    int GetDayOfYear() const;

    /**
     * Get hour in day, range: [0, 23].
     * @return int - hour in day.
     */
    int GetHour() const;

    /**
     * Get minute in day, range: [0, 59].
     * @return int - minute in day.
     */
    int GetMinute() const;

    /**
     * Get second in day, range: [0, 59].
     * @return int - second in day.
     */
    int GetSecond() const;

    /**
     * Get milli-second in second, range: [0, 999]
     * @return int - milli-second in second.
     */
    int GetMillisecond() const;

    /**
     * Get micro-second in milli-second, range: [0, 999]
     * @return int - micro-second in milli-second.
     */
    int GetMicrosecond() const;

    /**
     * Get date part time.
     * @return LLBC_Time - the date part time.
     */
    LLBC_Time GetDate() const;
    /**
     * Get current time of hour.
     * @return LLBC_TimeSpan - the current time of hour.
     */
    LLBC_TimeSpan GetTimeOfHour() const;
    /**
     * Get current time of day.
     * @return LLBC_TimeSpan - the current time of day.
     */
    LLBC_TimeSpan GetTimeOfDay() const;
    /**
     * Get current time of week.
     * @return LLBC_TimeSpan - the current time of week.
     */
    LLBC_TimeSpan GetTimeOfWeek() const;
    /**
     * Get current time of month.
     * @return LLBC_TimeSpan - the current time of month.
     */
    LLBC_TimeSpan GetTimeOfMonth() const;

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
     * @return tm & - time struct object.
     */
    tm GetGmtTime() const;
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
    LLBC_Time AddMillis(int millis) const;
    LLBC_Time AddMicros(int micros) const;

public:
    /**
     * Make sure given year is leap year or not.
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
     * Get interval to time of hour.
     * @param[in] toTimeOfHour - to time of hour.
     * @return LLBC_TimeSpan - interval value.
     */
    LLBC_TimeSpan GetIntervalToTimeOfHour(const LLBC_TimeSpan &toTimeOfHour) const;

    /**
     * Get interval to time of day.
     * @param[in] toTimeOfDay - to time of day.
     * @return LLBC_TimeSpan - interval value.
     */
    LLBC_TimeSpan GetIntervalToTimeOfDay(const LLBC_TimeSpan &toTimeOfDay) const;

    /**
     * Get interval to time of week.
     * @param[in] toTimeOfWeek - to time of week.
     * @return LLBC_TimeSpan - interval value.
     */
    LLBC_TimeSpan GetIntervalToTimeOfWeek(const LLBC_TimeSpan &toTimeOfWeek) const;

public:
    /**
     * Verify that the given time(to) has been crossed hour or not.
     * @param[in] from       - from time.
     * @param[in] to         - to time.
     * @param[in] timeOfHour - cross time of hour point.
     * @return bool - return true if crossed day, otherwise return false.
     */
    static bool IsCrossedHour(const LLBC_Time &from,
                              const LLBC_Time &to,
                              const LLBC_TimeSpan &timeOfHour = LLBC_TimeSpan::zero);

    /**
     * Verify that the given time(to) has been crossed day or not.
     * @param[in] from      - from time.
     * @param[in] to        - to time.
     * @param[in] timeOfDay - cross time of day point.
     * @return bool - return true if crossed day, otherwise return false.
     */
    static bool IsCrossedDay(const LLBC_Time &from,
                             const LLBC_Time &to,
                             const LLBC_TimeSpan &timeOfDay = LLBC_TimeSpan::zero);

    /**
     * Verify that the given time(to) has been crossed week or not.
     * @param[in] from       - from time.
     * @param[in] to         - to time.
     * @param[in] timeOfWeek - cross time of week point.
     * @return bool - return true if crossed week, otherwise return false.
     */
    static bool IsCrossedWeek(const LLBC_Time &from,
                              const LLBC_Time &to,
                              const LLBC_TimeSpan &timeOfWeek = LLBC_TimeSpan::zero);

public:
    /**
     * Time span operations.
     */
    LLBC_TimeSpan operator-(const LLBC_Time &time) const;
    LLBC_TimeSpan operator+(const LLBC_Time &time) const;

    LLBC_Time operator+(const LLBC_TimeSpan &span) const;
    LLBC_Time operator-(const LLBC_TimeSpan &span) const;

    bool operator==(const LLBC_Time &time) const;
    bool operator!=(const LLBC_Time &time) const;
    bool operator<(const LLBC_Time &time) const;
    bool operator>(const LLBC_Time &time) const;
    bool operator<=(const LLBC_Time &time) const;
    bool operator>=(const LLBC_Time &time) const;

    LLBC_Time &operator=(const LLBC_Time &time);

public:
    /**
     * Get the time object string representation.
     * @return LLBC_String - the object string representation.
     */
    LLBC_String ToString() const;

public:
    /**
     * Stream output operatorsupport.
     */
    friend std::ostream & ::operator<<(std::ostream &stream, const LLBC_Time &t);

public:
    /**
     * Serialize / Deserialize support.
     */
    void Serialize(LLBC_Stream &stream) const;
    bool Deserialize(LLBC_Stream &stream);

private:
    /**
     * Construct Time object from time string representation.
     * 
     * @param[in] timeStr    - the time string representatin, eg: 2024-09-10 18:19:30.123456
     * @param[in] timeStrLen - the time string length, not included '\0'.
     * @return LLBC_Time - time obuect.
     */
    static LLBC_Time FromTimeStr(const char *timeStr, size_t timeStrLen);

    /**
     * Internal constructor.
     * @param clendarTimeInMicroseconds - calendar time in microseconds.
     */
    explicit LLBC_Time(const sint64 &clendarTimeInMicroseconds);

    /**
     * Update time structs(local&gmt)
     */
    void FillTimeStruct();

    /**
     * Get interval to time internal implement.
     */
    LLBC_TimeSpan GetIntervalTo(const LLBC_TimeSpan &timeCycle,
                                LLBC_TimeSpan toTimeOfTimeCycle) const;

    /**
     * Crossed time-cycle internal implement. 
     */
    static bool IsCrossed(const LLBC_Time &from,
                          const LLBC_Time &to,
                          const LLBC_TimeSpan &timeCycle,
                          LLBC_TimeSpan timeOfTimeCycle);

private:
    sint64 _time;
    tm _localTimeStruct;
};

__LLBC_NS_END

#include "llbc/core/time/TimeInl.h"


