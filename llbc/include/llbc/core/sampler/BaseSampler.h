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

#ifndef __LLBC_CORE_SAMPLER_BASE_SAMPLER_H__
#define __LLBC_CORE_SAMPLER_BASE_SAMPLER_H__

#include "llbc/common/Common.h"

#include "llbc/core/sampler/ISampler.h"

__LLBC_NS_BEGIN

/**
 * \brief The basic sampler class encapsulation.
 */
class LLBC_EXPORT LLBC_BaseSampler : public LLBC_ISampler
{
public:
    LLBC_BaseSampler();
    virtual ~LLBC_BaseSampler();

public:
    /**
     * Reset sampler.
     */
     virtual void Reset();

public:
    /**
     * Update sampler, using to heartbeat.
     * @param[in] time - update time, GMT time.
     */
    virtual void Update(time_t time);

    /**
     * Sampling function.
     * @param[in] value   - increment value.
      *@param[in] appData - current time sampling value.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Sampling(sint64 value, void *appData = nullptr);

public:
    /**
     * Check sampler is begin sampling.
     * @return bool - return true if begin sampling, otherwise return false.
     */
    virtual bool IsBeginSampling() const;

    /**
     * Get first samping time.
     * @return time_t - first samping time.
     */
    virtual time_t GetFirstSamplingTime() const;

    /**
     * Get first samping value.
     * @return sint64 - first sampling value.
     */
    virtual sint64 GetFirstSamplingValue() const;

    /**
     * Get first samping append data.
     * @return void * - first samping append data.
     */
    virtual void *GetFirstSamplingAppData() const;

    /**
     * Get last sampling value.
     * @return sint64 - last sampling value.
     */
    virtual sint64 GetLastSamplingValue() const;

    /**
     * Get last sampling append data.
     * @return void * - last sampling append data.
     */
    virtual void *GetLastSamplingAppData() const;

    /**
     * Get last update time.
     * @return time_t - last update time.
     */
    virtual time_t GetLastUpdateTime() const;

private:
    bool _beginSampling;

    time_t _firstSamplingTime;
    sint64 _firstSamplingValue;
    void *_firstSamplingAppData;

    sint64 _lastSamplingValue;
    void *_lastSamplingAppData;

    time_t _lastUpdateTime;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_SAMPLER_BASE_SAMPLER_H__
