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

#pragma once

__LLBC_NS_BEGIN

template <typename T>
int LLBC_CheckFlowUseAdd(T value, T addent)
{
    if (UNLIKELY(addent == 0))
    {
        return LLBC_FlowType::NoFlow;
    }

    T result = value + addent;
    if (T(T(0) - 1) > 0)
    {
        return ((result < value) ?
            LLBC_FlowType::OverFlow : LLBC_FlowType::NoFlow);
    }
    else
    {
        if (addent > 0)
        {
            return ((result < value) ?
                LLBC_FlowType::OverFlow : LLBC_FlowType::NoFlow);
        }
        else
        {
            //return ((value + addent > value) ?
                //LLBC_FlowType::UnderFlow : LLBC_FlowType::NoFlow);
            if (result > value)
                return LLBC_FlowType::UnderFlow;
            else
                return LLBC_FlowType::NoFlow;
        }
    }

    return LLBC_FlowType::NoFlow;
}

template <typename T>
int LLBC_CheckFlowUseSub(T value, T subtrahend)
{
    if (UNLIKELY(subtrahend == 0))
    {
        return LLBC_FlowType::NoFlow;
    }

    T result = value - subtrahend;
    if (T(T(0) - 1) > 0)
    {
        return ((result > value) ?
            LLBC_FlowType::UnderFlow : LLBC_FlowType::NoFlow);
    }
    else
    {
        if (subtrahend > 0)
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
