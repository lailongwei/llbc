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

#include "llbc/core/sampler/BaseSampler.h"

__LLBC_NS_BEGIN

/**
 * \brief The limit type sampler class encapsulation.
 */
class LLBC_EXPORT LLBC_LimitSampler : public LLBC_BaseSampler
{
    typedef LLBC_BaseSampler _Base;

public:
    LLBC_LimitSampler();
    virtual ~LLBC_LimitSampler();

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
     * Sampling function.
     * @param[in] value   - increment value.
      *@param[in] appData - current time sampling value.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Sampling(sint64 value, void *appData = nullptr);

public:
    /**
     * Get minimize value sampling value.
     * @return sint64 - sampling value.
     */
    sint64 GetMinValue() const;

    /**
     * Get minimize value sampling time.
     * @return time_t - sampling time.
     */
    time_t GetMinValueSamplingTime() const;

    /**
     * Get maximize vaue sampling value.
     * @return sint64 - sampling value.
     */
    sint64 GetMaxValue() const;

    /**
     * Get maximize value sampling time.
     * @return time_t - sampling time.
     */
    time_t GetMaxValueSamplingTime() const;

private:
    sint64 _minVal;
    time_t _minValSamplingTime;

    sint64 _maxVal;
    time_t _maxValSamplingTime;
};

__LLBC_NS_END


