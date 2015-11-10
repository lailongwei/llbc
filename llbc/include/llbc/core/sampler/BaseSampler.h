/**
 * @file    BaseSampler.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/11/25
 * @version 1.0
 *
 * @brief
 */
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
    virtual int Sampling(sint64 value, void *appData = NULL);

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
