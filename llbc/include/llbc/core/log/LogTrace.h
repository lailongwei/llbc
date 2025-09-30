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
 * \brief The log trace object encapsulation.
 */
struct LLBC_EXPORT LLBC_LogTrace final
{
public:
    /**
     * \brief The TraceData template class encapsulation, used for trace key/content define.
     */
    template <size_t _TraceStrLimit>
    struct TraceData
    {
        // Trace string length.
        size_t strLen;
        // Trace string.
        char str[_TraceStrLimit];

        TraceData(): strLen(0), str{ '\0' } {  }

        template <typename _TraceDataTy>
        explicit TraceData(const _TraceDataTy &traceData);

        TraceData &operator=(const TraceData &other);

        bool operator<(const TraceData &other) const;
        bool operator<=(const TraceData &other) const { return !(other < *this); }
        bool operator>(const TraceData &other) const { return other < *this; }
        bool operator>=(const TraceData &other) const { return !(*this < other); }
        bool operator==(const TraceData &other) const;
        bool operator!=(const TraceData &other) const { return !(*this == other); }

        explicit operator bool() const;
        bool operator!() const { return !static_cast<bool>(*this); }
    };

public:
    // The trace key.
    typedef TraceData<LLBC_CFG_CORE_LOG_TRACE_KEY_LIMIT> TraceKey;
    TraceKey traceKey;

    // The trace content.
    typedef TraceData<LLBC_CFG_CORE_LOG_TRACE_CONTENT_LIMIT> TraceContent;
    TraceContent traceContent;

public:
    /**
     * Construct by traceKey and traceContent.
     * @param[in] traceKey     - the trace key.
     * @param[in] traceContent - the trace content.
     */
    template <typename _TraceKeyTy,
              typename TraceContentTy>
    LLBC_LogTrace(const _TraceKeyTy &traceKey, const TraceContentTy &traceContent);

    /**
     * Copy constructor.
     */
    explicit LLBC_LogTrace(const LLBC_LogTrace &other);

    /**
     * Copy assignment.
     */
    LLBC_LogTrace &operator=(const LLBC_LogTrace &other);
};

/**
 * The log trace manager(used for manage log traces.
 */
class LLBC_HIDDEN LLBC_LogTraceMgr
{
public:
    /**
     * Construct by separators.
     * @param[in] logTracesSep       - the traces separator.
     * @param[in] traceKeyContentSep - the trace key and content separator.
     * @param[in] traceContentsSep   - the trace contents separator.
     */
    LLBC_LogTraceMgr(char logTracesSep,
                     char traceKeyContentSep,
                     char traceContentsSep);

public:
    /**
     * Add log trace.
     * @param[in] logTrace - the log trace obj. 
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddLogTrace(const LLBC_LogTrace &logTrace);

    /**
     * Remove log trace.
     * @param[in] logTrace            - the log trace obj.
     * @param[in] setTraceTimesToZero - set this log trace times to zero flag,
     *                                  if true, will set trace times to zero and remove this log trace.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveLogTrace(const LLBC_LogTrace &logTrace, bool setTraceTimesToZero);

    /**
     * Get log trace times.
     * @param logTrace - the log trace obj.
     * @return size_t - this log trace obj trace times.
     */
    size_t GetLogTraceTimes(const LLBC_LogTrace &logTrace) const;

    /**
     * Clear specific key log traces.
     * @param[in] traceKey - the trace key.
     */
    void ClearLogTrace(const LLBC_LogTrace::TraceKey &traceKey);

    /**
     * Clear all log traces.
     */
    void ClearAllLogTraces();

    /**
     * Get trace content(built log traces).
     * @return std::vector<std::pair<LLBC_LogTrace::TraceContent, sint64>> - the trace content.
     */
    const std::vector<std::pair<LLBC_LogTrace::TraceContent, sint64>>* GetLogTraceContents(const LLBC_LogTrace::TraceKey &traceKey) const;

    /**
     * Get trace info(built log traces).
     * @return std::shared_ptr<LLBC_String> - the trace info.
     */
    std::shared_ptr<LLBC_String> GetTraceInfo() const { return _traceInfo; }

    /**
     * Set trace separators.
     * @param[in] logTracesSep       - the traces separator.
     * @param[in] traceKeyContentSep - the trace key and content separator.
     * @param[in] traceContentsSep   - the trace contents separator.
     */
    void SetSeparators(char logTracesSep,
                       char traceKeyContentSep,
                       char traceContentsSep);

private:
    /**
     * Rebuild trace info.
     */
    void _RebuildTraceInfo();

private:
    // The log trace separators.
    char _logTracesSep; // traces separator.
    char _logTraceKeyContentSep; // key/content separator.
    char _traceContentsSep; // contents separator.

    // The trace info.
    std::shared_ptr<LLBC_String> _traceInfo;

    // all log traces.
    // {traceKey: [<traceContent, traceTimes>]}
    std::map<LLBC_LogTrace::TraceKey, std::vector<std::pair<LLBC_LogTrace::TraceContent, sint64>>> _logTraces;
};

__LLBC_NS_END

#include "llbc/core/log/LogTraceInl.h"
