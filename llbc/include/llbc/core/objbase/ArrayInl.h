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

template <typename _Fn1>
void LLBC_Array::Foreach(_Fn1 &fn)
{
    this->Foreach(this->Begin(), this->End(), fn);
}

template <typename _Fn1>
void LLBC_Array::Foreach(LLBC_Array::Iter n0, _Fn1 &fn)
{
    this->Foreach(n0, this->End(), fn);
}

template <typename _Fn1>
void LLBC_Array::Foreach(LLBC_Array::difference_type n0, _Fn1 &fn)
{
    this->Foreach(this->Begin() + n0, this->End(), fn);
}

template <typename _Fn1>
void LLBC_Array::Foreach(LLBC_Array::Iter n0, LLBC_Array::Iter n1, _Fn1 &fn)
{
    for (Iter it = n0; it != n1; ++it)
    {
        if (!fn(*it))
            break;
    }
}

template <typename _Fn1>
void LLBC_Array::Foreach(LLBC_Array::difference_type n0, LLBC_Array::difference_type n1, _Fn1 &fn)
{
    this->Foreach(this->Begin() + n0, this->Begin() + n1, fn);
}

template <typename _Fn1>
void LLBC_Array::Foreach(_Fn1 &fn) const
{
    this->Foreach(this->Begin(), this->End(), fn);
}

template <typename _Fn1>
void LLBC_Array::Foreach(LLBC_Array::ConstIter n0, _Fn1 &fn) const
{
    this->Foreach(n0, this->End(), fn);
}

template <typename _Fn1>
void LLBC_Array::Foreach(LLBC_Array::difference_type n0, _Fn1 &fn) const
{
    this->Foreach(this->Begin() + n0, this->End(), fn);
}

template <typename _Fn1>
void LLBC_Array::Foreach(LLBC_Array::ConstIter n0, LLBC_Array::ConstIter n1, _Fn1 &fn) const
{
    for (ConstIter it = n0; it != n1; ++it)
    {
        if (!fn(*it))
            break;
    }
}

template <typename _Fn1>
void LLBC_Array::Foreach(LLBC_Array::difference_type n0, LLBC_Array::difference_type n1, _Fn1 &fn) const
{
    this->Foreach(this->Begin() + n0, this->Begin() + n1, fn);
}

template <typename _Fn2>
void LLBC_Array::Sort(_Fn2 &fn)
{
    this->Sort(this->Begin(), this->End(), fn);
}

template <typename _Fn2>
void LLBC_Array::Sort(LLBC_Array::Iter n0, _Fn2 &fn)
{
    this->Sort(n0, this->End(), fn);
}

template <typename _Fn2>
void LLBC_Array::Sort(LLBC_Array::difference_type n0, _Fn2 &fn)
{
    this->Sort(this->Begin() + n0, this->End(), fn);
}

template <typename _Fn2>
void LLBC_Array::Sort(LLBC_Array::Iter n0, LLBC_Array::Iter n1, _Fn2 &fn)
{
    const difference_type begin = n0._idx;
    const difference_type end = n1._idx;
    if (begin == end)
        return;

    for (difference_type i = begin + 1; i < end; ++i)
    {
        Obj *obj = _objs[i];
        difference_type j = i - 1;
        while (j >= begin && !fn(_objs[j], obj))
        {
            _objs[j + 1] = _objs[j];
            j--;
        }

        _objs[j + 1] = obj;
    }
}

template <typename _Fn2>
void LLBC_Array::Sort(LLBC_Array::difference_type n0, LLBC_Array::difference_type n1, _Fn2 &fn)
{
    this->Sort(this->Begin() + n0, this->Begin() + n1, fn);
}

__LLBC_NS_END
