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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Previous declare Time & TimeSpan class.
 */
class LLBC_Time;
class LLBC_TimeSpan;

__LLBC_NS_END

/**
 * TimeSpan class stream output operators previous declare.
 */
std::ostream & operator<<(std::ostream &stream, const LLBC_NS LLBC_TimeSpan &span);

__LLBC_NS_BEGIN

/**
 * \brief Time span class encapsulation.
 */
class LLBC_EXPORT LLBC_TimeSpan final
{
public:
    /**
     * Convenience span construct methods.
     */
    static LLBC_TimeSpan FromDays(int days, int hours = 0, int minutes = 0, int seconds = 0, int millis = 0, int micros = 0);
    static LLBC_TimeSpan FromHours(int hours, int minutes = 0, int seconds = 0, int millis = 0, int micros = 0);
    static LLBC_TimeSpan FromMinutes(int minutes, int seconds = 0, int millis = 0, int micros = 0);
    static LLBC_TimeSpan FromSeconds(int seconds, int millis = 0, int micros = 0);
    static LLBC_TimeSpan FromMillis(sint64 millis, sint64 micros = 0);
    static LLBC_TimeSpan FromMicros(sint64 micros);
    template <size_t _StrArrLen>
    static LLBC_TimeSpan FromSpanStr(const char (&spanStr)[_StrArrLen]);
    static LLBC_TimeSpan FromSpanStr(const char *spanStr);
    template <typename _StrType>
    static typename std::enable_if<LLBC_IsTemplSpec<_StrType, std::basic_string>::value, LLBC_TimeSpan>::type
    FromSpanStr(const _StrType &spanStr);
    static LLBC_TimeSpan FromSpanStr(const LLBC_String &spanStr);

public:
    /**
     * Converience constant variables.
     */
    static const LLBC_TimeSpan zero;
    static const LLBC_TimeSpan oneMicrosec;
    static const LLBC_TimeSpan oneMillisec;
    static const LLBC_TimeSpan oneSec;
    static const LLBC_TimeSpan oneMin;
    static const LLBC_TimeSpan oneHour;
    static const LLBC_TimeSpan oneDay;
    static const LLBC_TimeSpan oneWeek;
    static const LLBC_TimeSpan negOneSec;
    static const LLBC_TimeSpan negOneMillisec;
    static const LLBC_TimeSpan negOneMicrosec;
    static const LLBC_TimeSpan negOneMin;
    static const LLBC_TimeSpan negOneHour;
    static const LLBC_TimeSpan negOneDay;
    static const LLBC_TimeSpan negOneWeek;

public:
    /**
     * Construct by time tick, in micro-seconds.
     * @param[in] spanInMicros - the span value, in micro-seconds.
     */
    explicit LLBC_TimeSpan(const sint64 &spanInMicros = 0);

    /**
     * Construct by span string representation, fmt: [DD ][HH:]MM:SS[.Micros].
     * @param[in] spanStr - the span value string representation.
     */
    template <size_t _StrArrLen>
    explicit LLBC_TimeSpan(const char (&spanStr)[_StrArrLen]);
    explicit LLBC_TimeSpan(const char *spanStr);
    template <typename _StrType,
              typename = typename std::enable_if<LLBC_IsTemplSpec<_StrType, std::basic_string>::value, _StrType>::type>
    explicit LLBC_TimeSpan(const _StrType &spanStr);
    explicit LLBC_TimeSpan(const LLBC_String &spanStr);

public:
    /**
     * Get days/hours/minutes/seconds/milli-seconds/micro-seconds.
     * @return int - the time span parts value.
     */
    int GetDays() const;
    int GetHours() const;
    int GetMinutes() const;
    int GetSeconds() const;
    int GetMillis() const;
    int GetMicros() const;

    int GetTotalDays() const;
    int GetTotalHours() const;
    int GetTotalMinutes() const;
    int GetTotalSeconds() const;
    sint64 GetTotalMillis() const;
    sint64 GetTotalMicros() const;

    LLBC_TimeSpan AddDays(int days);
    LLBC_TimeSpan AddHours(int hours);
    LLBC_TimeSpan AddMinutes(int minutes);
    LLBC_TimeSpan AddSeconds(int seconds);
    LLBC_TimeSpan AddMillis(sint64 millis);
    LLBC_TimeSpan AddMicros(sint64 micros);

    LLBC_TimeSpan operator+(const LLBC_TimeSpan &span) const;
    LLBC_TimeSpan operator-(const LLBC_TimeSpan &span) const;
    LLBC_TimeSpan operator*(double scale) const;
    LLBC_TimeSpan operator/(double scale) const;
    LLBC_TimeSpan operator%(sint64 span) const;
    LLBC_TimeSpan operator%(const LLBC_TimeSpan &span) const;

    LLBC_TimeSpan &operator+=(const LLBC_TimeSpan &span);
    LLBC_TimeSpan &operator-=(const LLBC_TimeSpan &span);
    LLBC_TimeSpan &operator*=(double scale);
    LLBC_TimeSpan &operator/=(double scale);
    LLBC_TimeSpan &operator%=(sint64 span);
    LLBC_TimeSpan &operator%=(const LLBC_TimeSpan &span);

    bool operator==(const LLBC_TimeSpan &span) const;
    bool operator!=(const LLBC_TimeSpan &span) const;
    bool operator<(const LLBC_TimeSpan &span)const;
    bool operator>(const LLBC_TimeSpan &span) const;
    bool operator<=(const LLBC_TimeSpan &span) const;
    bool operator>=(const LLBC_TimeSpan &span) const;

    LLBC_TimeSpan &operator=(const LLBC_TimeSpan &span);

public:
    /**
     * Get the timespan object string representation.
     * @return LLBC_String - the object string representation.
     */
    LLBC_String ToString() const;

public:
    /**
     * Stream output operator support.
     */
    friend std::ostream & ::operator<<(std::ostream &o, const LLBC_TimeSpan &s);

public:
    /**
     * Serialize&Deserialize support.
     */
    void Serialize(LLBC_Stream &stream) const;
    bool Deserialize(LLBC_Stream &stream);

private:
    /**
     * Internal constructor.
     * 
     * @param spanStr    - the span string.
     * @param spanStrLen - the span string length.
     */
    explicit LLBC_TimeSpan(const char *spanStr, size_t spanStrLen);

private:
    // Declare friend class:LLBC_Time.
    // Access members:
    // - LLBC_TimeSpan::LLBC_TimeSpan(const sint64 &span)
    friend class LLBC_Time;

private:
    sint64 _span;
};

__LLBC_NS_END

#include "llbc/core/time/TimeSpanInl.h"


