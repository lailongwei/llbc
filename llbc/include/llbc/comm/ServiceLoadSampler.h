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

#include "llbc/core/Core.h"

__LLBC_NS_BEGIN

/**
 * \brief The service recent load info structure.
 */
struct LLBC_EXPORT LLBC_ServiceRecentLoadInfo
{
    LLBC_TimeSpan recentTime;
    LLBC_TimeSpan workingTime;

    size_t updateTimes;
    size_t overloadTimes;

    LLBC_ServiceRecentLoadInfo();

    /**
     * Get this class object string representation.
     * @return LLBC_String - the string representation.
     */
    LLBC_String ToString() const;
};

/**
 * \brief The service load sampler class encapsulation.
 */
class LLBC_HIDDEN __LLBC_ServiceLoadSampler
{
public:
    __LLBC_ServiceLoadSampler();
    ~__LLBC_ServiceLoadSampler();

public:
    /**
     * Init recent load sample collect.
     * @param[in] loadSampleTime - the load sample time span.
     */
    void Init(const LLBC_TimeSpan &loadSampleTime);

    /**
     * Clear recent load sample collect.
     */
    void Clear();

    /**
     * Check whether recent load sample collect is enabled or not.
     * @return bool - return true if enabled, otherwise return false.
     */
    bool IsEnabled() const { return _loadSampleRing.GetCapacity() > 0; }

    /**
     * Collect one OnSvc() loop load sample.
     * @param[in] begMillis     - the OnSvc() begin millis(time before heart-beat work).
     * @param[in] endMillis     - the OnSvc() end millis(time after heart-beat work, before sleep).
     * @param[in] frameInterval - the service frame interval, in millis.
     */
    void Collect(sint64 begMillis, sint64 endMillis, sint64 frameInterval);

    /**
     * Get recent load info.
     * @param[in]  recentTime - the user expect recent time, must be greater than zero.
     * @param[out] loadInfo   - the output recent load info.
     * @return int - return 0 if success, otherwise return -1.
     */
    int GetRecentLoadInfo(const LLBC_TimeSpan &recentTime,
                          LLBC_ServiceRecentLoadInfo &loadInfo) const;

private:
    /**
     * \brief Service load sample(per-interval) data structure.
     */
    struct _ServiceLoadSample
    {
        sint64 beginStatTimeInMillis; // Sample begin stat time, in millis.
        sint64 lastStatTimeInMillis;  // Sample last stat time(last update time), in millis.

        sint64 workingTime;           // Total OnSvc() working time in this sample, in millis.

        size_t updateTimes;           // Total OnSvc() call count in this sample.
        size_t overloadTimes;         // Total overload(working time >= frameInterval) count in this sample.
    };
    static_assert(std::is_trivially_copyable<_ServiceLoadSample>::value,
                  "_ServiceLoadSample must be trivially copyable!");

private:
    mutable LLBC_RingBuffer<_ServiceLoadSample> _loadSampleRing; // Ring buffer of load samples.
    mutable LLBC_SpinLock _loadSampleLock; // Protects _loadSampleRing.
};

__LLBC_NS_END

/**
 * Service recent load info stream output operator previous declare.
 */
LLBC_EXTERN LLBC_EXPORT std::ostream &operator<<(std::ostream &o, const LLBC_NS LLBC_ServiceRecentLoadInfo &loadInfo);
