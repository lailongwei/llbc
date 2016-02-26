/**
 * @file    SamplerGroup.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/12/08
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/helper/STLHelper.h"

#include "llbc/core/sampler/SamplerType.h"
#include "llbc/core/sampler/CountSampler.h"
#include "llbc/core/sampler/LimitSampler.h"
#include "llbc/core/sampler/IntervalSampler.h"

#include "llbc/core/sampler/SamplerGroup.h"

__LLBC_NS_BEGIN

LLBC_SamplerGroup::LLBC_SamplerGroup()
: _samplers(LLBC_New(_SamplerMap))
{
}

LLBC_SamplerGroup::~LLBC_SamplerGroup()
{
    LLBC_STLHelper::DeleteContainer(*_samplers, false);
    LLBC_Delete(_samplers);
}

int LLBC_SamplerGroup::AddSampler(int type, const LLBC_String &name)
{
    if (!LLBC_SamplerType::IsValid(type) || name.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    if (_samplers->find(name) != _samplers->end())
    {
        LLBC_SetLastError(LLBC_ERROR_EXIST);
        return LLBC_FAILED;
    }

    LLBC_ISampler *sampler = NULL;
    switch (type)
    {
    case LLBC_SamplerType::CountSampler:
        sampler = new LLBC_CountSampler;
        break;

    case LLBC_SamplerType::LimitSampler:
        sampler = new LLBC_LimitSampler;
        break;

    case LLBC_SamplerType::IntervalSampler:
        sampler = new LLBC_IntervalSampler;
        break;

    default:
        ASSERT(false && "llbc library internal error, unknown sampler type!");
        break;
    }

    _samplers->insert(std::make_pair(name, sampler));

    return true;
}

void LLBC_SamplerGroup::Reset()
{
    _SamplerMapIter iter = _samplers->begin();
    for (; iter != _samplers->end(); iter++)
    {
        iter->second->Reset();
    }
}

void LLBC_SamplerGroup::Update(time_t time)
{
    _SamplerMapIter iter = _samplers->begin();
    for (; iter != _samplers->end(); iter++)
    {
        iter->second->Update(time);
    }
}

LLBC_ISampler *LLBC_SamplerGroup::GetSampler(const LLBC_String &name)
{
    _SamplerMapIter iter = _samplers->find(name);
    return iter != _samplers->end() ? 
        iter->second : (LLBC_SetLastError(LLBC_ERROR_NOT_FOUND), (LLBC_ISampler *)NULL);
}

const LLBC_ISampler *LLBC_SamplerGroup::GetSampler(const LLBC_String &name) const
{
    _SamplerMapCIter iter = _samplers->find(name);
    return iter != _samplers->end() ?
        iter->second : (LLBC_SetLastError(LLBC_ERROR_NOT_FOUND), (LLBC_ISampler *)NULL);

}

int LLBC_SamplerGroup::Sampling(const LLBC_String &name, sint64 value, void *appData)
{
    LLBC_ISampler *sam = this->GetSampler(name);
    return sam ? sam->Sampling(value, appData) : LLBC_FAILED;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
