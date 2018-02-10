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

#include "llbc/core/sampler/BaseSampler.h"

__LLBC_NS_BEGIN

LLBC_BaseSampler::LLBC_BaseSampler()
: _beginSampling(false)

, _firstSamplingTime(0)
, _firstSamplingValue(0)
, _firstSamplingAppData(NULL)

, _lastSamplingValue(0)
, _lastSamplingAppData(NULL)

, _lastUpdateTime(0)
{
}

LLBC_BaseSampler::~LLBC_BaseSampler()
{
}

void LLBC_BaseSampler::Reset()
{
    _beginSampling = false;

    _firstSamplingTime = 0;
    _firstSamplingValue = 0;
    _firstSamplingAppData = NULL;

    _lastSamplingValue = 0;
    _lastSamplingAppData = NULL;

    _lastUpdateTime = 0;
}

void LLBC_BaseSampler::Update(time_t time)
{
    _lastUpdateTime = time;
}

int LLBC_BaseSampler::Sampling(sint64 value, void *appData)
{
    if (UNLIKELY(!_beginSampling))
    {
        _firstSamplingTime = time(NULL);
        _firstSamplingValue = value;
        _firstSamplingAppData = appData;

        _beginSampling = true;
    }

    _lastSamplingValue = value;
    _lastSamplingAppData = appData;

    return LLBC_OK;
}

bool LLBC_BaseSampler::IsBeginSampling() const
{
    return _beginSampling;
}

time_t LLBC_BaseSampler::GetFirstSamplingTime() const
{
    return _firstSamplingTime;
}

sint64 LLBC_BaseSampler::GetFirstSamplingValue() const
{
    return _firstSamplingValue;
}

void *LLBC_BaseSampler::GetFirstSamplingAppData() const
{
    return _firstSamplingAppData;
}

sint64 LLBC_BaseSampler::GetLastSamplingValue() const
{
    return _lastSamplingValue;
}

void *LLBC_BaseSampler::GetLastSamplingAppData() const
{
    return _lastSamplingAppData;
}

time_t LLBC_BaseSampler::GetLastUpdateTime() const
{
    return _lastUpdateTime;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
