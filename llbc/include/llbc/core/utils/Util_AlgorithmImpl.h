/**
 * @file    Util_AlgorithmImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/09/13
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_CORE_UTILS_UTIL_ALGORITHM_H__

__LLBC_NS_BEGIN

template <typename T>
int LLBC_CheckFlowUseAdd(T value, T addent)
{
    if(UNLIKELY(addent == 0))
    {
        return LLBC_FlowType::NoFlow;
    }

    T result = value + addent;
    if(T(T(0) - 1) > 0)
    {
        return ((result < value) ? 
            LLBC_FlowType::OverFlow : LLBC_FlowType::NoFlow);
    }
    else
    {
        if(addent > 0)
        {
            return ((result < value) ? 
                LLBC_FlowType::OverFlow : LLBC_FlowType::NoFlow);
        }
        else
        {
            //return ((value + addent > value) ? 
                //LLBC_FlowType::UnderFlow : LLBC_FlowType::NoFlow);
            if(result > value)
            {
                return LLBC_FlowType::UnderFlow;
            }
            else
            {
                return LLBC_FlowType::NoFlow;
            }
        }
    }

    return LLBC_FlowType::NoFlow;
}

template <typename T>
int LLBC_CheckFlowUseSub(T value, T subtrahend)
{
    if(UNLIKELY(subtrahend == 0))
    {
        return LLBC_FlowType::NoFlow;
    }

    T result = value - subtrahend;
    if(T(T(0) - 1) > 0)
    {
        return ((result > value) ? 
            LLBC_FlowType::UnderFlow : LLBC_FlowType::NoFlow);
    }
    else
    {
        if(subtrahend > 0)
        {
            return ((result > value) ? 
                LLBC_FlowType::UnderFlow : LLBC_FlowType::NoFlow);
        }
        else
        {
            return ((result < value) ? 
                LLBC_FlowType::OverFlow : LLBC_FlowType::NoFlow);
        }
    }

    return LLBC_FlowType::NoFlow;
}

__LLBC_NS_END

#endif // __LLBC_CORE_UTILS_UTIL_ALGORITHM_H__
