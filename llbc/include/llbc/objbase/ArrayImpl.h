/**
 * @file    ArrayImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/17
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_OBJBASE_ARRAY_H__

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
    for (Iter it = n0; it != n1; it ++)
    {
        if (!fn(*it))
        {
            break;
        }
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
    for (ConstIter it = n0; it != n1; it ++)
    {
        if (!fn(*it))
        {
            break;
        }
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
    {
        return;
    }

    for (difference_type i = begin + 1; i < end; i ++)
    {
        Obj *obj = _objs[i];
        difference_type j = i - 1;
        while (j >= begin && !fn(_objs[j], obj))
        {
            _objs[j + 1] = _objs[j];
            j --;
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

#endif // __LLBC_OBJBASE_ARRAY_H__
