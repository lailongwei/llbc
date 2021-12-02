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
    int Sampling(const LLBC_String &name, sint64 value, void *appData = nullptr);

private:
    LLBC_DISABLE_ASSIGNMENT(LLBC_SamplerGroup);

private:
    _SamplerMap *_samplers;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_SAMPLER_SAMPLER_GROUP_H__
