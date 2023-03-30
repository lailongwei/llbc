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

#include "llbc/core/os/OS_Time.h"
#include "llbc/core/time/Time.h"

__LLBC_NS_BEGIN

inline LLBC_CPUTime::LLBC_CPUTime(uint64 cpuCount)
: _cpuCount(cpuCount)
{
}

inline uint64 LLBC_CPUTime::GetCPUFreqPerSecond()
{
    return _freqPerSecond;
}

inline uint64 LLBC_CPUTime::GetCPUCount() const
{
    return _cpuCount;
}

inline int LLBC_CPUTime::ToSeconds() const
{
#if LLBC_SUPPORT_RDTSC
    return static_cast<int>(_cpuCount / _freqPerSecond);
#else // Not supp rdtsc
    return _cpuCount / LLBC_TimeConstant::NumOfMicroSecondsPerSecond;
#endif // Supp rdtsc
}

inline sint64 LLBC_CPUTime::ToMilliSeconds() const
{
#if LLBC_SUPPORT_RDTSC
    return _cpuCount * 1000ll / _freqPerSecond;
#else // Not supp rdtsc
    return _cpuCount / LLBC_TimeConstant::NumOfMilliSecondsPerSecond;
#endif // Supp rdtsc
}

inline sint64 LLBC_CPUTime::ToMicroSeconds() const
{
#if LLBC_SUPPORT_RDTSC
    return _cpuCount * 1000000ll / _freqPerSecond;
#else // Not supp rdtsc
    return _cpuCount;
#endif // Supp rdtsc
}

inline sint64 LLBC_CPUTime::ToNanoSeconds() const
{
#if LLBC_SUPPORT_RDTSC
    return _cpuCount * 1000000000ll / _freqPerSecond;
#else // Not supp rdtsc
    return _cpuCount * LLBC_TimeConstant::NumOfNanoSecondsPerMicroSecond;
#endif // Supp rdtsc
}

inline int LLBC_CPUTime::ToSeconds(uint64 cpuCount)
{
    return LLBC_CPUTime(cpuCount).ToSeconds();
}

inline sint64 LLBC_CPUTime::ToMilliSeconds(uint64 cpuCount)
{
    return LLBC_CPUTime(cpuCount).ToMilliSeconds();
}

inline sint64 LLBC_CPUTime::ToMicroSeconds(uint64 cpuCount)
{
    return LLBC_CPUTime(cpuCount).ToMicroSeconds();
}

inline sint64 LLBC_CPUTime::ToNanoSeconds(uint64 cpuCount)
{
    return LLBC_CPUTime(cpuCount).ToNanoSeconds();
}

inline LLBC_CPUTime LLBC_CPUTime::operator +(const LLBC_CPUTime &right) const
{
    return LLBC_CPUTime(_cpuCount + right._cpuCount);
}

inline LLBC_CPUTime LLBC_CPUTime::operator -(const LLBC_CPUTime &right) const
{
    if (_cpuCount < right._cpuCount)
        return LLBC_CPUTime(0);

    return LLBC_CPUTime(_cpuCount - right._cpuCount);
}

inline LLBC_CPUTime &LLBC_CPUTime::operator +=(const LLBC_CPUTime &right)
{
    _cpuCount += right._cpuCount;
    return *this;
}

inline LLBC_CPUTime &LLBC_CPUTime::operator -=(const LLBC_CPUTime &right)
{
    if (_cpuCount < right._cpuCount)
        _cpuCount = 0;
    else
        _cpuCount -= right._cpuCount;

    return *this;
}

inline bool LLBC_CPUTime::operator <(const LLBC_CPUTime &right) const
{
    return _cpuCount < right._cpuCount;
}

inline bool LLBC_CPUTime::operator >(const LLBC_CPUTime &right) const
{
    return _cpuCount > right._cpuCount;
}

inline bool LLBC_CPUTime::operator <=(const LLBC_CPUTime &right) const
{
    return _cpuCount <= right._cpuCount;
}

inline bool LLBC_CPUTime::operator >=(const LLBC_CPUTime &right) const
{
    return _cpuCount >= right._cpuCount;
}

inline bool LLBC_CPUTime::operator ==(const LLBC_CPUTime &right) const
{
    return _cpuCount == right._cpuCount;
}

inline bool LLBC_CPUTime::operator !=(const LLBC_CPUTime &right) const
{
    return _cpuCount != right._cpuCount;
}

inline LLBC_CPUTime::operator uint64() const
{
    return _cpuCount;
}

__LLBC_NS_END

