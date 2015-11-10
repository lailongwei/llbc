/**
 * @file    SamplerType.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/11/24
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/sampler/SamplerType.h"

__LLBC_NS_BEGIN

bool LLBC_SamplerType::IsValid(int type)
{
    return (type >= LLBC_SamplerType::Begin && 
        type < LLBC_SamplerType::End) ? true : false;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
