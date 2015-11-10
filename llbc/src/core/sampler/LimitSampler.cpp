/**
 * @file    LimitSampler.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/11/25
 * @version 1.0
 *
 * @brief
 */

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
    if (_Base::Sampling(value, appData) != LLBC_RTN_OK)
    {
        return LLBC_RTN_FAILED;
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

    return LLBC_RTN_OK;
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
