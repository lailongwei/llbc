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


#include "llbc/common/Export.h"

#include "llbc/core/helper/STLHelper.h"

#include "llbc/core/sampler/SamplerType.h"
#include "llbc/core/sampler/CountSampler.h"
#include "llbc/core/sampler/LimitSampler.h"
#include "llbc/core/sampler/IntervalSampler.h"

#include "llbc/core/sampler/SamplerGroup.h"

__LLBC_NS_BEGIN

LLBC_SamplerGroup::LLBC_SamplerGroup()
: _samplers(new _SamplerMap)
{
}

LLBC_SamplerGroup::~LLBC_SamplerGroup()
{
    LLBC_STLHelper::DeleteContainer(*_samplers, false);
    delete _samplers;
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

    LLBC_ISampler *sampler = nullptr;
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
    for (; iter != _samplers->end(); ++iter)
    {
        iter->second->Reset();
    }
}

void LLBC_SamplerGroup::Update(time_t time)
{
    _SamplerMapIter iter = _samplers->begin();
    for (; iter != _samplers->end(); ++iter)
    {
        iter->second->Update(time);
    }
}

LLBC_ISampler *LLBC_SamplerGroup::GetSampler(const LLBC_String &name)
{
    _SamplerMapIter iter = _samplers->find(name);
    return iter != _samplers->end() ? 
        iter->second : (LLBC_SetLastError(LLBC_ERROR_NOT_FOUND), static_cast<LLBC_ISampler *>(nullptr));
}

const LLBC_ISampler *LLBC_SamplerGroup::GetSampler(const LLBC_String &name) const
{
    _SamplerMapCIter iter = _samplers->find(name);
    return iter != _samplers->end() ?
        iter->second : (LLBC_SetLastError(LLBC_ERROR_NOT_FOUND), static_cast<LLBC_ISampler *>(nullptr));

}

int LLBC_SamplerGroup::Sampling(const LLBC_String &name, sint64 value, void *appData)
{
    LLBC_ISampler *sam = GetSampler(name);
    return sam ? sam->Sampling(value, appData) : LLBC_FAILED;
}

__LLBC_NS_END
