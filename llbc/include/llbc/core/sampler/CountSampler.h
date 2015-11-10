/**
* @file    CountSampler.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/11/25
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_SAMPLER_COUNT_SAMPLER_H__
#define __LLBC_CORE_SAMPLER_COUNT_SAMPLER_H__

#include "llbc/common/Common.h"

#include "llbc/core/sampler/BaseSampler.h"

__LLBC_NS_BEGIN

/**
 * \brief The count type sampler class encapsulation.
 */
class LLBC_EXPORT LLBC_CountSampler : public LLBC_BaseSampler
{
    typedef LLBC_BaseSampler _Base;

public:
    LLBC_CountSampler();
    virtual ~LLBC_CountSampler();

public:
    /**
     * Get sampler type.
     * @return int = sampler type.
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
     * Get total sampling times.
     * @return uint64 - total sampling times.
     */
    uint64 GetTotalSamplingTimes() const;

    /**
     * Get total sampling value.
     * @return sint64 - total sampling value.
     */
    sint64 GetTotalSamplingValue() const;

    /**
     * Get average sampling value.
     * @return double - average sampling value.
     */
    double GetAverageSamplingValue() const;

private:
    uint64 _totalSamplingTimes;
    sint64 _totalSamplingValue;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_SAMPLER_COUNT_SAMPLER_H__
