/**
 * @file    SamplerType.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/11/24
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_SAMPLER_SAMPLER_TYPE_H__
#define __LLBC_CORE_SAMPLER_SAMPLER_TYPE_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief The sampler type enumeration.
 */
class LLBC_EXPORT LLBC_SamplerType
{
public:
    enum
    {
        Begin,

        CountSampler = Begin,
        IntervalSampler,
        LimitSampler,

        End
    };

    /**
     * Check given sampler type vaildate or not.
     * @return bool - return true if validate, otherwise return false.
     */
    static bool IsValid(int type);
};

__LLBC_NS_END

#endif // !__LLBC_CORE_SAMPLER_SAMPLER_TYPE_H__
