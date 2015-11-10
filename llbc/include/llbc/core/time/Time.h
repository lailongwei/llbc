/**
 * @file    Time.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/05
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_TIME_TIME_H__
#define __LLBC_CORE_TIME_TIME_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN
class LLBC_Time;
class LLBC_TimeSpan;
__LLBC_NS_END

/**
 * Time, TimeSpan class stream output operators previous declare.
 */
LLBC_EXTERN LLBC_EXPORT std::ostream & operator <<(std::ostream &o, const LLBC_NS LLBC_Time &s);
LLBC_EXTERN LLBC_EXPORT std::ostream & operator <<(std::ostream &o, const LLBC_NS LLBC_TimeSpan &s);

__LLBC_NS_BEGIN

/**
 * \brief Time span class encapsulation.
 */
class LLBC_EXPORT LLBC_TimeSpan
{
public:
    LLBC_TimeSpan();

    LLBC_TimeSpan(const LLBC_TimeSpan &span);

    explicit LLBC_TimeSpan(sint64 span);

    LLBC_TimeSpan(int days, int hours, int mins, int secs);

    ~LLBC_TimeSpan();

public:
    int GetDays() const;
    int GetTotalHours() const;
    int GetHours() const;
    int GetTotalMinutes() const;
    int GetMinutes() const;
    int GetTotalSeconds() const;
    int GetSeconds() const;

    sint64 GetTimeSpan() const;
    void SetTimeSpan(sint64 span);

    LLBC_String Format(const char *format) const;

    LLBC_TimeSpan operator +(const LLBC_TimeSpan &span) const;
    LLBC_TimeSpan operator -(const LLBC_TimeSpan &span) const;
    LLBC_TimeSpan& operator +=(const LLBC_TimeSpan &span);
    LLBC_TimeSpan& operator -=(const LLBC_TimeSpan &span);

    bool operator ==(const LLBC_TimeSpan &span) const;
    bool operator !=(const LLBC_TimeSpan &span) const;
    bool operator <(const LLBC_TimeSpan &span)const;
    bool operator >(const LLBC_TimeSpan &span) const;
    bool operator <=(const LLBC_TimeSpan &span) const;
    bool operator >=(const LLBC_TimeSpan &span) const;

    LLBC_TimeSpan &operator =(sint64 span);
    LLBC_TimeSpan &operator =(const LLBC_TimeSpan &span);

public:
    /**
     * Stream output operator support.
     */
    friend std::ostream & ::operator <<(std::ostream &o, const LLBC_TimeSpan &s);

public:
    void Serialize(LLBC_Stream &stream) const;
    bool DeSerialize(LLBC_Stream &stream);

    void SerializeEx(LLBC_Stream &stream) const;
    bool DeSerializeEx(LLBC_Stream &stream);

private:
    sint64 m_span;
};

/**
 * \brief LLBC library Time class encapsulation.
 */
class LLBC_EXPORT LLBC_Time
{
public:
    /**
     * Constructors.
     */
    LLBC_Time();

    LLBC_Time(const LLBC_Time &time);

    explicit LLBC_Time(time_t time);

    LLBC_Time(int year,
              int month,
              int day,
              int hour,
              int min,
              int sec,
              int tz = 8);

    LLBC_Time(const char *timestr, 
              char dateTimeSpliter = ' ', 
              char dateSpliter = '-',
              char timeSpliter = ':',
              int tz = 8);

    /**
     * Destructor.
     */
    ~LLBC_Time();

    /**
     * Get current time.
     */
    static LLBC_Time GetCurrentTime();

    /**
     * Get number of seconds in day.
     * @param[in] days - days, default is 1.
     * @return uint32 - number of seconds in given days.
     */
    static uint32 GetNumOfSecondsInDay(uint32 days = 1);

    /**
     * Get number of milli-seconds in day.
     * @param[in] days - days, default is 1.
     * @return uint32 - number of milli-seconds in given days.
     */
    static uint32 GetNumOfMilliSecondsInDay(uint32 days = 1);

    /**
     * Get number of milli-seconds in second.
     * @param[in] seconds - seconds, default is 1.
     * @return uint32 - number of milli-seconds in given seconds.
     */
    static uint32 GetNumOfMilliSecondsInSecond(uint32 seconds = 1);

    /**
     * Get interval to specify hour,minute,second every day.
     * @param[in] fromTime - from time.
     * @param[in] hour - hour.
     * @param[in] min  - minute.
     * @param[in] sec  - second.
     * @param[in] secondsInDay - seconds in day.
     * @return LLBC_TimeSpan - timespan value.
     */
    static LLBC_TimeSpan GetIntervalTo(int secondsInDay);
    static LLBC_TimeSpan GetIntervalTo(int hour, int min, int sec);
    static LLBC_TimeSpan GetIntervalTo(const LLBC_Time &fromTime, int secondsInDay);
    static LLBC_TimeSpan GetIntervalTo(const LLBC_Time &fromTime, int hour, int min, int sec);

    /**
     * Get time_t format time(Calendar time).
     * @return time_t - UTC time, in seconds.
     */
    time_t GetGmtTime() const;
    /**
     * Get local time.
     * @return time_t - local time, in seconds.
     */
    time_t GetLocalTime() const;

    /**
     * Get GMT time struct.
     * @param[out] tm - time struct pointer, if not null, will fill value.
     * @return struct LLBC_TimeStruct * - time struct pointer[thread unsafe].
     */
    struct LLBC_TimeStruct *GetGmtTm(struct LLBC_TimeStruct *tm) const;

    /**
     * Get local time struct.
     * @param[out] tm - time struct pointer, if not null, will fill value.
     * @return struct LLBC_TimeStruct * - time struct pointer[thread unsafe].
     */
    struct LLBC_TimeStruct *GetLocalTm(struct LLBC_TimeStruct *tm) const;

    /**
     * Get local time parts(year, month, day, hour, minute, second, dayofweek).
     * @return int - time part.
     */
    int GetYear() const;
    int GetMonth() const;
    int GetDay() const;
    int GetHour() const;
    int GetMinute() const;
    int GetSecond() const;
    int GetDayOfWeek() const;

    /**
     * Format local time, see strftime() api.
     */
    LLBC_String Format(const char *format = NULL) const;

    /**
     * Format gmt time, see strftime() api.
     */
    LLBC_String FormatGmt(const char *format = NULL) const;

    /**
     * Time span operations.
     */
    LLBC_TimeSpan operator -(const LLBC_Time &time) const;
    LLBC_TimeSpan operator +(const LLBC_Time &time) const;

    LLBC_Time operator +(const LLBC_TimeSpan &span) const;
    LLBC_Time operator -(const LLBC_TimeSpan &span) const;
    LLBC_Time &operator +=(const LLBC_TimeSpan &span);
    LLBC_Time &operator -=(const LLBC_TimeSpan &span);

    bool operator ==(const LLBC_Time &time) const;
    bool operator !=(const LLBC_Time &time) const;
    bool operator <(const LLBC_Time &time) const;
    bool operator >(const LLBC_Time &time) const;
    bool operator <=(const LLBC_Time &time) const;
    bool operator >=(const LLBC_Time &time) const;

    LLBC_Time &operator =(time_t time);
    LLBC_Time &operator =(const LLBC_Time &time);

public:
    /**
     * Stream output operator support.
     */
    friend std::ostream & ::operator <<(std::ostream &o, const LLBC_Time &t);

public:
    /**
     * Serialize / DeSerialize support.
     */
    void Serialize(LLBC_Stream &stream) const;
    bool DeSerialize(LLBC_Stream &stream);

    void SerializeEx(LLBC_Stream &stream) const;
    bool DeSerializeEx(LLBC_Stream &stream);

private:
    void SetByPart(int year, int month, 
        int day, int hour, int min, int sec, int tz);

private:
    sint64 _time;
    static const uint32 _numOfSecondsPerDay;
    static const uint32 _numOfMilliSecondsPerDay;
    static const uint32 _numOfMilliSecondsPerSecond;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_TIME_TIME_H__
