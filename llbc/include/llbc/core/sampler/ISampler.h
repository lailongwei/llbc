/**
 * @file    ISampler.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/11/24
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_SAMPLER_ISAMPLER_H__
#define __LLBC_CORE_SAMPLER_ISAMPLER_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief The sampler interface class encapsulation.
 */
class LLBC_EXPORT LLBC_ISampler
{
public:
    virtual ~LLBC_ISampler() {  }

public:
   /**
    * Get sampler type.
    * @return int = sampler type.
    */
    virtual int GetType() const = 0;

    /**
     * Reset sampler.
     */
     virtual void Reset() = 0;

public:
    /**
     * Update sampler, using to heartbeat.
     * @param[in] time - update time, GMT time.
     */
    virtual void Update(time_t time) = 0;

    /**
     * Sampling function.
     * @param[in] time    - sampling time.
     * @param[in] value   - increment value.
      *@param[in] appData - current time sampling value.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Sampling(sint64 value, void *appData = NULL) = 0;

public:
    /**
     * Check sampler is begin sampling.
     * @return bool - return true if begin sampling, otherwise return false.
     */
    virtual bool IsBeginSampling() const = 0;

    /**
     * Get first samping time.
     * @return time_t - first samping time.
     */
    virtual time_t GetFirstSamplingTime() const = 0;

    /**
     * Get first samping value.
     * @return sint64 - first samping value.
     */
    virtual sint64 GetFirstSamplingValue() const = 0;

    /**
     * Get first samping append data.
     * @return void * - first samping append data.
     */
    virtual void *GetFirstSamplingAppData() const = 0;

    /**
     * Get last sampling value.
     * @return sint64 - last sampling value.
     */
    virtual sint64 GetLastSamplingValue() const = 0;

    /**
     * Get last sampling append data.
     * @return void * - last sampling append data.
     */
    virtual void *GetLastSamplingAppData() const = 0;

    /**
     * Get last update time.
     * @return time_t - last update time.
     */
    virtual time_t GetLastUpdateTime() const = 0;

};

__LLBC_NS_END

#endif // !__LLBC_CORE_SAMPLER_ISAMPLER_H__
