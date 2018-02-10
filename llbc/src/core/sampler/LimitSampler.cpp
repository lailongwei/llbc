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
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/sampler/SamplerType.h"
#include "llbc/core/sampler/LimitSampler.h"

__LLBC_NS_BEGIN

LLBC_LimitSampler::LLBC_LimitSampler()
: _minVal(0)
, _minValSamplingTime(0)

, _maxVal(0)
, _maxValSamplingTime(0)
{
}

LLBC_LimitSampler::~LLBC_LimitSampler()
{
}

int LLBC_LimitSampler::GetType() const
{
    return LLBC_SamplerType::LimitSampler;
}

void LLBC_LimitSampler::Reset()
{
    _minVal = _maxVal = 0;

    _minValSamplingTime = 0;
    _maxValSamplingTime = 0;
}

int LLBC_LimitSampler::Sampling(sint64 value, void *appData)
{
    if (_Base::Sampling(value, appData) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    if (value < _minVal)
    {
        _minVal = value;
        _minValSamplingTime = time(NULL);
    }
    else if (value > _maxVal)
    {
        _maxVal = value;
        _maxValSamplingTime = time(NULL);
    }

    return LLBC_OK;
}

sint64 LLBC_LimitSampler::GetMinValue() const
{
    return _minVal;
}

time_t LLBC_LimitSampler::GetMinValueSamplingTime() const
{
    return _minValSamplingTime;
}

sint64 LLBC_LimitSampler::GetMaxValue() const
{
    return _maxVal;
}

time_t LLBC_LimitSampler::GetMaxValueSamplingTime() const
{
    return _maxValSamplingTime;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
