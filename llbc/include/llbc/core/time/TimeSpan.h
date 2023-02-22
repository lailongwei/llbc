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
std::ostream & operator <<(std::ostream &stream, const LLBC_NS LLBC_TimeSpan &span);

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
    static LLBC_TimeSpan FromDays(int days, int hours = 0, int minutes = 0, int seconds = 0, int millisecs = 0, int microsecs = 0);
    static LLBC_TimeSpan FromHours(int hours, int minutes = 0, int seconds = 0, int millisecs = 0, int microsecs = 0);
    static LLBC_TimeSpan FromMinutes(int minutes, int seconds = 0, int millisecs = 0, int microsecs = 0);
    static LLBC_TimeSpan FromSeconds(int seconds, int millisecs = 0, int microsecs = 0);
    static LLBC_TimeSpan FromMillis(sint64 millisecs, sint64 microsecs = 0);
    static LLBC_TimeSpan FromMicros(sint64 microsecs);

    static LLBC_TimeSpan FromSpanStr(const LLBC_String &spanStr);

public:
    /**
     * Converience constant variables.
     */
    static const LLBC_TimeSpan zero;
    static const LLBC_TimeSpan oneSec;
    static const LLBC_TimeSpan oneMin;
    static const LLBC_TimeSpan oneHour;
    static const LLBC_TimeSpan oneDay;
    static const LLBC_TimeSpan negOneSec;
    static const LLBC_TimeSpan negOneMin;
    static const LLBC_TimeSpan negOneHour;
    static const LLBC_TimeSpan negOneDay;

public:
    /**
     * Construct by time tick, in micro-seconds.
     * @param[in] span - the span tick.
     */
    explicit LLBC_TimeSpan(const sint64 &span = 0);

    /**
     * Construct by span string representation, fmt: [DD ][HH:]MM:SS[.Micros].
     * @param[in] span - the span value string representation.
     */
    LLBC_TimeSpan(const LLBC_String &span);

    /**
     * Destructor.
     */
    ~LLBC_TimeSpan();

public:
    /**
     * Get days/hours/minutes/seconds/milli-seconds/micro-seconds.
     * @return int - the time span parts value.
     */
    int GetDays() const;
    int GetHours() const;
    int GetMinutes() const;
    int GetSeconds() const;
    int GetMilliSeconds() const;
    int GetMicroSeconds() const;

    int GetTotalDays() const;
    int GetTotalHours() const;
    int GetTotalMinutes() const;
    int GetTotalSeconds() const;
    sint64 GetTotalMilliSeconds() const;
    sint64 GetTotalMicroSeconds() const;

    LLBC_TimeSpan AddDays(int days);
    LLBC_TimeSpan AddHours(int hours);
    LLBC_TimeSpan AddMinutes(int minutes);
    LLBC_TimeSpan AddSeconds(int seconds);
    LLBC_TimeSpan AddMilliSeconds(sint64 milliSeconds);
    LLBC_TimeSpan AddMicroSeconds(sint64 microSeconds);

    LLBC_TimeSpan operator +(const LLBC_TimeSpan &span) const;
    LLBC_TimeSpan operator -(const LLBC_TimeSpan &span) const;
    LLBC_TimeSpan operator *(const LLBC_TimeSpan &span) const;
    LLBC_TimeSpan operator /(const LLBC_TimeSpan &span) const;
    LLBC_TimeSpan operator %(const LLBC_TimeSpan &span) const;

    LLBC_TimeSpan &operator +=(const LLBC_TimeSpan &span);
    LLBC_TimeSpan &operator -=(const LLBC_TimeSpan &span);
    LLBC_TimeSpan &operator *=(const LLBC_TimeSpan &span);
    LLBC_TimeSpan &operator /=(const LLBC_TimeSpan &span);
    LLBC_TimeSpan &operator %=(const LLBC_TimeSpan &span);

    bool operator ==(const LLBC_TimeSpan &span) const;
    bool operator !=(const LLBC_TimeSpan &span) const;
    bool operator <(const LLBC_TimeSpan &span)const;
    bool operator >(const LLBC_TimeSpan &span) const;
    bool operator <=(const LLBC_TimeSpan &span) const;
    bool operator >=(const LLBC_TimeSpan &span) const;

    LLBC_TimeSpan &operator =(const LLBC_TimeSpan &span);

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
    friend std::ostream & ::operator <<(std::ostream &o, const LLBC_TimeSpan &s);

public:
    /**
     * Serialize&DeSerialize support.
     */
    void Serialize(LLBC_Stream &stream) const;
    bool DeSerialize(LLBC_Stream &stream);

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


