/**
 * @file    LimitSampler.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/11/25
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_SAMPLER_LIMIT_SAMPLER_H__
#define __LLBC_CORE_SAMPLER_LIMIT_SAMPLER_H__

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
    virtual int Sampling(sint64 value, void *appData = NULL);

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

#endif // !__LLBC_CORE_SAMPLER_LIMIT_SAMPLER_H__
