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

/**
 * Convert byte array to string format(hexadecimal format).
 * @param[in] bytes     - buffer pointer.
 * @param[in] len       - buffer length.
 * @param[in] byteSep   - byte separator.
 * @param[in] lineWidth - line width, default is 0.
 * @return LLBC_String - the formatted string data.
 */
LLBC_EXPORT LLBC_String LLBC_Byte2Hex(const void *bytes, size_t len, char byteSep = '\0', size_t lineWidth = 0);

/**
 * Trace function define.
 * Note: Like ActionScript Language, trace function use to simple output some message.
 *       Just useful in debug mode(defined DEBUG macro).
 * @param[in] format - string format argument.
 * @param[in] ...    - optional arguments.
 */
#ifndef trace 
 #ifdef LLBC_DEBUG
  #define trace(format, ...) LLBC_FilePrint(stdout, format, ##__VA_ARGS__)
  #define traceline(format, ...) LLBC_FilePrintLn(stdout, format, ##__VA_ARGS__)
 #else
  #define trace(format, ...)
  #define traceline(format, ...)
 #endif
#endif

/**
 * \brief Provides a set of methods and properties that you can use to accurately measure elapsed time.
 */
class LLBC_EXPORT LLBC_Stopwatch final
{
public:
    /**
     * Construct stopwatch object.
     * @param[in] autoStart - start measuring after constructed flag, default is true.
     */
    explicit LLBC_Stopwatch(bool autoStart = true);

    /**
     * Construct stopwatch object by elapsed ticks.
     * @param[in] elapsedTicks      - elapsed ticks.
     * @param[in] continueMeasuring - continue measuring flag, default is false.
     */
    explicit LLBC_Stopwatch(uint64 elapsedTicks, bool continueMeasuring = false);

public:
    /**
     * Resume measuring elapsed time for an interval. 
     */
    void Resume();

    /**
     * Pause measuring elapsed time for an interval.
     */
    void Pause();

    /**
     * Stops time interval measurement and resets the elapsed time to zero.
     */
    void Reset();

    /**
     * Gets a value indicating whether the Stopwatch timer is running.
     * @return bool - the running flag.
     */
    bool IsRunning() const { return _beginTime != 0; }

public:
    /**
     * Indicates whether the timer is based on a high-resolution performance counter. This field is read-only.
     * @return bool - the high resolution flag.
     */
    LLBC_NO_DISCARD static constexpr bool IsHighResolution()
    {
        #if LLBC_SUPPORT_RDTSC
        return true;
        #else
        return false;
        #endif
    }

    /**
     * Gets the frequency of the timer as the number of ticks per second. This field is read-only.
     * @return uint64 - the watcher frequency.
     */
    LLBC_NO_DISCARD static uint64 GetFrequency() { return _frequency; }

    /**
     * Gets the total elapsed time measured by the current instance.
     * @return LLBC_TimeSpan - the elapsed time.
     */
    LLBC_TimeSpan Elapsed() const;

    /**
     * Gets the total elapsed time measured by the current instance, in nano-seconds.
     * @return uint64 - the total elapsed time, in nano-seconds.
     */
    uint64 ElapsedNanos() const;

    /**
     * ets the total elapsed time measured by the current instance, in timer ticks.
     * @return uint64 - the total elapsed time, in timer tick.
     */
    uint64 ElapsedTicks() const;

public:
    /**
     * Get elapsed time string reprsentation.
     * @return LLBC_String - the elapsed time string representation, in milli-seconds.
     */
    LLBC_String ToString() const;

public:
    /**
     * Init watcher frequency.
     */
    static void InitFrequency();

private:
    uint64 _beginTime;
    uint64 _elapsedTime;
    static uint64 _frequency;
};

__LLBC_NS_END

/**
 * stream output operator function for Stopwatch(in global ns).
 */
inline std::ostream &operator<<(std::ostream &o, const LLBC_NS LLBC_Stopwatch &watcher)
{
    return o << watcher.ToString();
}

#include "llbc/core/utils/Util_DebugInl.h"



