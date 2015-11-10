/**
 * @file    IntervalSampler.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/11/24
 * @version 1.0
 *
 * @brief
 */
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
