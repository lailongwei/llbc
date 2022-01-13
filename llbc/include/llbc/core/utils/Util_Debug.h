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

#ifndef __LLBC_CORE_UTILS_UTIL_DEBUG_H__
#define __LLBC_CORE_UTILS_UTIL_DEBUG_H__

#include "llbc/common/Common.h"

#include "llbc/core/thread/SpinLock.h"

__LLBC_NS_BEGIN

/**
 * Convert byte array to string format(hexadecimal format).
 * @param[in] buf - buffer pointer.
 * @param[in] len - buffer length.
 * @param[in] lineWidth - line width, default is 16.
 * @return std::string - the formatted string data.
 */
LLBC_EXPORT std::string LLBC_Byte2Hex(const void *buf, size_t len, uint32 lineWidth = 16);

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
  #define traceline(format, ...) LLBC_FilePrintLine(stdout, format, ##__VA_ARGS__)
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
    typedef uint64 CPUTimeCount;

public:
    LLBC_CPUTime();
    LLBC_CPUTime(CPUTimeCount count);
    ~LLBC_CPUTime();

public:
    static LLBC_CPUTime Current();
    
    CPUTimeCount ToSeconds() const;

    CPUTimeCount ToMilliSeconds() const;

    CPUTimeCount ToMicroSeconds() const;
    CPUTimeCount ToNanoSeconds() const;

    std::string ToString() const;

    LLBC_CPUTime operator +(const LLBC_CPUTime &right) const;
    LLBC_CPUTime operator -(const LLBC_CPUTime &right) const;
    LLBC_CPUTime &operator +=(const LLBC_CPUTime &right);
    LLBC_CPUTime &operator -=(const LLBC_CPUTime &right);

    bool operator <(const LLBC_CPUTime &right) const;
    bool operator >(const LLBC_CPUTime &right) const;
    bool operator <=(const LLBC_CPUTime &right) const;
    bool operator >=(const LLBC_CPUTime &right) const;
    bool operator ==(const LLBC_CPUTime &right) const;
    bool operator !=(const LLBC_CPUTime &right) const;

public:
    static void InitFrequency();

private:
    static CPUTimeCount _freq;
    CPUTimeCount _count;
};

/**
 * \brief The performanze analyze point encapsulation.
 */
class LLBC_EXPORT LLBC_PerfAnalyzePoint
{
public:
    /**
     * \brief The point analyze data encapsulation.
     */
    class LLBC_PointAnalyzeData
    {
    public:
        sint64 analyzeTime; // analyze time.

        bool analyzeFinished; // analyze finish flag.
        sint64 beginAnalyzeTime; // eegin analyze time.
        sint64 endAnalyzeTime; // end finish time.
    };

public:
    explicit LLBC_PerfAnalyzePoint(LLBC_PerfAnalyzePoint *parent,
                                   const char *name,
                                   const char *codeFileName,
                                   int codeLine);
    ~LLBC_PerfAnalyzePoint();

public:
    /**
     * Start performanze analyze.
     */
    void StartPerfAnalyze();

    /**
     * Stop performanze analyze.
     */
    void StopPerfAnalyze();

public:
    /**
     * Get analyze point name.
     * @const char * - the analyze point name.
     */
    const char *GetName() const;

    /**
     * Get code file name.
     * @const char * - analyze point code file name.
     */
    const char *GetCodeFileName() const;

    /**
     * Get code line number.
     * @return int - the code line number.
     */
    int GetCodeLine() const;

    /**
     * Get parent analyze point.
     * @return LLBC_PerfAnalyzePoint - the parent analyze point(maybe null).
     */
    const LLBC_PerfAnalyzePoint *GetParent() const;

    /**
     * Get children analyze points.
     * @return const std::map<const char *, LLBC_PerfAnalyzePoint *> * - the children analyze points.
     */
    const std::map<const char *, LLBC_PerfAnalyzePoint *> *GetChildren() const;

public:
    /**
     * Get total trigger times.
     * @return uint64 - the total trigger times.
     */
    uint64 GetTotalTriggerTimes() const;

    /**
     * Get total cost time.
     * @return sint64 - the total cost time.
     */
    sint64 GetTotalCostTime() const;

public:
    /**
     * Check current time trigger finished or not.
     * @return bool - the finished flag.
     */
    bool IsCurrentTimeTriggerFinished() const;

    /**
     * Get first trigger begin time.
     * @return sint64 - the first trigger begin time.
     */
    sint64 GetFirstTriggerBeginTime() const;

    /**
     * Get first trigger end time.
     * @return sint64 - the first trigger end time.
     */
    sint64 GetFirstTriggerEndTime() const;

    /**
     * Get last trigger begin time.
     * @return sint64 - the last trigger begin time.
     */
    sint64 GetLastTriggerBeginTime() const;

    /**
     * Get last trigger end time.
     * @return sint64 - the last trigger end time.
     */
    sint64 GetLastTriggerEndTime() const;

public:

private:
    const char *_name; // Performance point name.
    const char *_codeFileName; // Performance analyze code file name.
    int _codeLine; // Performance analyze code line.
    LLBC_PerfAnalyzePoint *_parent; // Parent performance analyze point(weak ref).
    std::map<const char *, LLBC_PerfAnalyzePoint *> *_children; // Children performance analyze point.

    uint64 _totalAnalyzeTimes; // Total analyze times.
    sint64 _totalCostTime; // Total cost time, in micro-seconds.

    bool _curTimeTriggerFinished; // Current time trigger finished flag.
    sint64 _firstTriggerBeginTime; // First trigger begin.
    sint64 _firstTriggerEndTime; // First trigger end time.
    sint64 _lastTriggerBeginTime; // Last trigger begin time.
    sint64 _lastTriggerEndTime; // Last trigger end time.

    sint64 _maxCostTriggerBeginTime; // Max cost trigger begin time.
    sint64 _maxCostTriggerEndTime; // Max cost trigger end time.
    sint64 _maxCostTriggerTimes; // Max cost trigger times(which trigger point triggered).

    sint64 _minCostTriggerBeginTime; // Min cost trigger begin time.
    sint64 _minCostTriggerEndTime; // Min cost trigger end time.
    sint64 _minCostTriggerTimes; // Min cost trigger times(which trigger point triggered).

    std::list<LLBC_PointAnalyzeData> _analyzeData;
};

/**
 * \brief The performance analyzer encapsulation.
 */
class LLBC_EXPORT LLBC_PerfAnalyzer
{
private:
    LLBC_SpinLock _lock;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_UTILS_UTIL_DEBUG_H__

