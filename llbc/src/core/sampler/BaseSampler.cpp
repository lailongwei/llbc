/**
 * @file    BaseSampler.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/11/25
 * @version 1.0
 *
 * @brief
 */

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
