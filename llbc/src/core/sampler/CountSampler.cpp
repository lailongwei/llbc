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

#include "llbc/core/sampler/SamplerType.h"
#include "llbc/core/sampler/CountSampler.h"

__LLBC_NS_BEGIN

LLBC_CountSampler::LLBC_CountSampler()
: _totalSamplingTimes(0)
, _totalSamplingValue(0)
{
}

LLBC_CountSampler::~LLBC_CountSampler()
{
}

int LLBC_CountSampler::GetType() const
{
    return LLBC_SamplerType::CountSampler;
}

void LLBC_CountSampler::Reset()
{
    _totalSamplingTimes = 0;
    _totalSamplingValue = 0;

    _Base::Reset();
}

int LLBC_CountSampler::Sampling(sint64 value, void *appData)
{
    if (_Base::Sampling(value, appData) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    _totalSamplingTimes += 1;
    _totalSamplingValue += value;

    return LLBC_OK;
}

uint64 LLBC_CountSampler::GetTotalSamplingTimes() const
{
    return _totalSamplingTimes;
}

sint64 LLBC_CountSampler::GetTotalSamplingValue() const
{
    return _totalSamplingValue;
}

double LLBC_CountSampler::GetAverageSamplingValue() const
{
    return static_cast<double>(
        _totalSamplingValue) / _totalSamplingTimes;
}

__LLBC_NS_END
