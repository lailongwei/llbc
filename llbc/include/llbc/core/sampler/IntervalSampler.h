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

#ifndef __LLBC_CORE_SAMPLER_INTERVAL_SAMPLER_H__
#define __LLBC_CORE_SAMPLER_INTERVAL_SAMPLER_H__

#include "llbc/common/Common.h"

#include "llbc/core/sampler/CountSampler.h"

__LLBC_NS_BEGIN

/**
 * \brief The interval sampler class encapsulation.
 */
class LLBC_EXPORT LLBC_IntervalSampler : public LLBC_CountSampler
{
    typedef LLBC_CountSampler _Base;

public:
    LLBC_IntervalSampler();
    virtual ~LLBC_IntervalSampler();

public:
   /**
    * Get sampler type.
    * @return int - sampler type.
    */
    virtual int GetType() const;

    /**
     * Reset sampler.
     */
     virtual void Reset();

public:
    /**
     * Update sampler, using to heartbeat.
     * @param[in] time - update time.
     */
    virtual void Update(time_t time);

    /**
     * Sampling function.
     * @param[in] value   - increment value.
      *@param[in] appData - current time sampling value.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Sampling(sint64 value, void *appData = NULL);

public:
    /**
     * Get last N second(s) speed, in seconds.
     * @param[in] secs - second(s).
     * @return sint64 - speed, in seconds.
     */
    sint64 GetSpeedInSecs(int secs) const;

    /**
     * Get last N minute(s) speed, in minutes.
     * @param[in] mins - minute(s).
     * @return sint64 - speed, in minutes.
     */
    sint64 GetSpeedInMins(int mins) const;

    /**
     * Get last N hour(s) speed, in minutes.
     * @param[in] hours - hour(s).
     * @return sint64 - speed, in hours.
     */
    sint64 GetSpeedInHours(int hours) const;

private:
    /**
     * Shift speed array.
     */
    void ShiftSpeedArray(sint64 *arr, int size, int diff);

private:
    sint64 _collector;
    sint64 _speeds[LLBC_CFG_CORE_SAMPLER_INTERVAL_SAMPLING_HOURS * 3600];
};

__LLBC_NS_END

#endif // !__LLBC_CORE_SAMPLER_INTERVAL_SAMPLER_H__
