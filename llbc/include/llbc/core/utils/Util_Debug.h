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

#include "llbc/core/thread/SpinLock.h"

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
 * \brief CPU tick counter, is a high-resoultion preformance counter.
 */
class LLBC_EXPORT LLBC_CPUTime
{
public:
    LLBC_CPUTime(uint64 cpuCount = 0);
    ~LLBC_CPUTime() = default;

public:
    static uint64 GetCPUFreqPerSecond();

public:
    static LLBC_CPUTime Current();
    
    uint64 GetCPUCount() const;

    int ToSeconds() const;
    sint64 ToMillis() const;
    sint64 ToMicros() const;
    sint64 ToNanos() const;

    static int ToSeconds(uint64 cpuCount);
    static sint64 ToMillis(uint64 cpuCount);
    static sint64 ToMicros(uint64 cpuCount);
    static sint64 ToNanos(uint64 cpuCount);

    LLBC_String ToString() const;

public:
    LLBC_CPUTime operator+(const LLBC_CPUTime &right) const;
    LLBC_CPUTime operator-(const LLBC_CPUTime &right) const;
    LLBC_CPUTime &operator+=(const LLBC_CPUTime &right);
    LLBC_CPUTime &operator-=(const LLBC_CPUTime &right);

    bool operator<(const LLBC_CPUTime &right) const;
    bool operator>(const LLBC_CPUTime &right) const;
    bool operator<=(const LLBC_CPUTime &right) const;
    bool operator>=(const LLBC_CPUTime &right) const;
    bool operator==(const LLBC_CPUTime &right) const;
    bool operator!=(const LLBC_CPUTime &right) const;

    operator uint64() const;

public:
    static void InitFrequency();

private:
    uint64 _cpuCount;
    static uint64 _freqPerSecond;
};


__LLBC_NS_END

/**
 * stream output operator function for cpu time(in global ns).
 */
LLBC_EXTERN LLBC_EXPORT std::ostream &operator<<(std::ostream &o, const LLBC_NS LLBC_CPUTime &cpuTime);

#include "llbc/core/utils/Util_DebugInl.h"



