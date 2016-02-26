/**
 * @file    IntervalSampler.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/11/24
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/utils/Util_Math.h"

#include "llbc/core/sampler/SamplerType.h"
#include "llbc/core/sampler/IntervalSampler.h"

__LLBC_NS_BEGIN

LLBC_IntervalSampler::LLBC_IntervalSampler()
{
    Reset();
}

LLBC_IntervalSampler::~LLBC_IntervalSampler()
{
}

int LLBC_IntervalSampler::GetType() const
{
    return LLBC_SamplerType::IntervalSampler;
}

void LLBC_IntervalSampler::Reset()
{
    _collector = 0;
    memset(_speeds, 0, sizeof(_speeds));

    _Base::Reset();
}

void LLBC_IntervalSampler::Update(time_t time)
{
    int diff = static_cast<int>(time - GetLastUpdateTime());
    _Base::Update(time);

    if(diff == 0)
    {
        return;
    }

    ShiftSpeedArray(_speeds, sizeof(_speeds) / sizeof(_speeds[0]), diff);

    _speeds[0] = _collector;
    _collector = 0;
}

int LLBC_IntervalSampler::Sampling(sint64 value, void *appData)
{
    if(_Base::Sampling(value, appData) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    _collector += value;

    return LLBC_OK;
}

sint64 LLBC_IntervalSampler::GetSpeedInSecs(int secs) const
{
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    if(UNLIKELY(secs <= 0))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return 0;
    }

    if(!IsBeginSampling())
    {
        return 0;
    }

    int nowTime = static_cast<int>(time(NULL));
    int firstSamplingTime = static_cast<int>(GetFirstSamplingTime());
    if(nowTime <= firstSamplingTime)
    {
        return 0;
    }

    secs = MIN(secs, static_cast<int>(sizeof(_speeds) / sizeof(_speeds[0])));

    if(nowTime - secs < firstSamplingTime)
    {
        secs = nowTime - firstSamplingTime;
    }

    sint64 value = 0;
    for(int i = 0; i < secs; i ++)
    {
        value += _speeds[i];
    }

    return value / secs;
}

sint64 LLBC_IntervalSampler::GetSpeedInMins(int mins) const
{
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    if(UNLIKELY(mins <= 0))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return 0;
    }

    if(!IsBeginSampling())
    {
        return 0;
    }

    int nowTime = static_cast<int>(time(NULL));
    int firstSamplingTime = static_cast<int>(GetFirstSamplingTime());
    if(nowTime <= firstSamplingTime)
    {
        return 0;
    }

    mins = MIN(mins, static_cast<int>(sizeof(_speeds) / sizeof(_speeds[0]) / 60));

    if(nowTime - mins * 60 < firstSamplingTime)
    {
        if((mins = (nowTime - firstSamplingTime) / 60) == 0)
        {
            return GetSpeedInSecs(
                       nowTime - firstSamplingTime) * (nowTime - firstSamplingTime);
        }
    }

    sint64 value = 0;
    for(int i = 0; i < mins; i ++)
    {
        for(int j = 0; j < 60; j ++)
        {
            value += _speeds[i * 60 + j];
        }
    }

    return value / mins;
}

sint64 LLBC_IntervalSampler::GetSpeedInHours(int hours) const
{
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    if(hours <= 0)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return 0;
    }

    if(!IsBeginSampling())
    {
        return 0;
    }

    int nowTime = static_cast<int>(time(NULL));
    int firstSamplingTime = static_cast<int>(GetFirstSamplingTime());
    if(nowTime <= firstSamplingTime)
    {
        return 0;
    }

    hours = MIN(hours, static_cast<int>(sizeof(_speeds) / sizeof(_speeds[0]) / 3600));
    if(nowTime - hours * 3600 < firstSamplingTime)
    {
        if((hours = (nowTime - firstSamplingTime) / 3600) == 0)
        {
            int secs = nowTime - firstSamplingTime;
            return GetSpeedInSecs(secs) * secs;
        }
    }

    sint64 value = 0;
    for(int i = 0; i < hours; i ++)
    {
        for(int j = 0; j < 3600; j ++)
        {
            value += _speeds[i * 3600 + j];
        }
    }

    return value;
}

void LLBC_IntervalSampler::ShiftSpeedArray(sint64 *arr, int size, int diff)
{
    sint64 *tmpArr = new sint64[size - 1];
    if(diff > 0)
    {
        if(diff < size)
        {
            memcpy(tmpArr, arr, (size - diff) * sizeof(sint64));
            memcpy(&arr[diff], tmpArr, (size - diff) * sizeof(sint64));
            memset(arr, 0, diff * sizeof(sint64));
        }
        else
        {
            memset(arr, 0, size * sizeof(sint64));
        }
    }
    else
    {
        int absDiff = LLBC_Abs(diff);
        if(absDiff < size)
        {
            memcpy(tmpArr, &arr[absDiff], (size - diff) * sizeof(sint64));
            memcpy(arr, tmpArr, (size - absDiff) * sizeof(sint64));
            memset(&arr[size - absDiff], 0, absDiff * sizeof(sint64));
        }
        else
        {
            memset(arr, 0, size * sizeof(sint64));
        }
    }

    delete []tmpArr;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
