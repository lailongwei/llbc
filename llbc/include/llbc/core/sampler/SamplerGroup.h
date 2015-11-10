/**
 * @file    SamplerGroup.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/12/08
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_SAMPLER_SAMPLER_GROUP_H__
#define __LLBC_CORE_SAMPLER_SAMPLER_GROUP_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Previous declare LLBC_ISampler interface class.
 */
class LLBC_ISampler;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The sampler group class encapsulation.
 */
class LLBC_EXPORT LLBC_SamplerGroup
{
    typedef std::map<LLBC_String, LLBC_ISampler *> _SamplerMap;
    typedef _SamplerMap::iterator _SamplerMapIter;
    typedef _SamplerMap::const_iterator _SamplerMapCIter;

public:
    LLBC_SamplerGroup();
    ~LLBC_SamplerGroup();

public:
    /**
     * Add new sampler.
     * @param[in] type - sampler type.
     * @param[in] name - sampler name, not empty.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddSampler(int type, const LLBC_String &name);

public:
    /**
     * Reset all samplers.
     */
    void Reset();

    /**
     * Update all samplers.
     */
    void Update(time_t time);

    /**
     * Get sampler.
     * @param[in] name - sampler name.
     * @return LLBC_ISampler * - sampler.
     */
    LLBC_ISampler *GetSampler(const LLBC_String &name);

    /**
     * Get sampler(const).
     * @param[in] name - sampler name.
     * return const LLBC_ISampler * - sampler.
     */
    const LLBC_ISampler *GetSampler(const LLBC_String &name) const;

public:
    /**
     * Let specific name's sampler sampling.
     * @param[in] name    - sampler name.
     * @param[in] value   - sampling value.
     * @param[in] appData - append data.
     * return int - return true if success, otherwise return -1.
     */
    int Sampling(const LLBC_String &name, sint64 value, void *appData = NULL);

private:
    LLBC_DISABLE_ASSIGNMENT(LLBC_SamplerGroup);

private:
    _SamplerMap *_samplers;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_SAMPLER_SAMPLER_GROUP_H__
