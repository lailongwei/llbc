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


#ifdef __LLBC_CORE_UTILS_UTIL_DEBUG_H__

__LLBC_NS_BEGIN

inline LLBC_CPUTime::LLBC_CPUTime()
: _cpuCount(0)
{
}

inline LLBC_CPUTime::LLBC_CPUTime(uint64 cpuCount)
: _cpuCount(cpuCount)
{
}

inline LLBC_CPUTime::~LLBC_CPUTime()
{
}

inline uint64 LLBC_CPUTime::GetCPUFreqPerSecond()
{
    return _freqPerSecond;
}

inline uint64 LLBC_CPUTime::GetCPUFreqPerMilliSecond()
{
    return _freqPerMillisecond;
}

inline uint64 LLBC_CPUTime::GetCPUFreqPerMicroSecond()
{
    return _freqPerMicroSecond;
}

inline uint64 LLBC_CPUTime::GetCPUFreqPerNanoSecond()
{
    return _freqPerNanoSecond;
}

inline uint64 LLBC_CPUTime::GetCpuCount() const
{
    return _cpuCount;
}

inline int LLBC_CPUTime::ToSeconds() const
{
    return static_cast<int>(_cpuCount / _freqPerSecond);
}

inline sint64 LLBC_CPUTime::ToMilliSeconds() const
{
    return _cpuCount / _freqPerMillisecond;
}

inline sint64 LLBC_CPUTime::ToMicroSeconds() const
{
    return _cpuCount / _freqPerMicroSecond;
}

inline sint64 LLBC_CPUTime::ToNanoSeconds() const
{
    return _cpuCount / _freqPerNanoSecond;
}

inline int LLBC_CPUTime::ToSeconds(uint64 cpuCount)
{
    return static_cast<int>(cpuCount / _freqPerSecond);
}

inline sint64 LLBC_CPUTime::ToMilliSeconds(uint64 cpuCount)
{
    return cpuCount / _freqPerMillisecond;
}

inline sint64 LLBC_CPUTime::ToMicroSeconds(uint64 cpuCount)
{
    return cpuCount / _freqPerMicroSecond;
}

inline sint64 LLBC_CPUTime::ToNanoSeconds(uint64 cpuCount)
{
    return cpuCount / _freqPerNanoSecond;
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

#endif // __LLBC_CORE_UTILS_UTIL_DEBUG_H__