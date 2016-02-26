/**
 * @file    CountSampler.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/11/25
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

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

#include "llbc/common/AfterIncl.h"
