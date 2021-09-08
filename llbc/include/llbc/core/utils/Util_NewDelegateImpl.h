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

#ifdef __LLBC_CORE_UTILS_UTIL_NEWDELEGATE_H__

__LLBC_NS_BEGIN

template <typename Rtn, typename ...Args>
LLBC_NewDelegate<Rtn(Args...)>::LLBC_NewDelegate(nullptr_t _)
: _obj(nullptr)
, _func{}
{
}

template <typename Rtn, typename ...Args>
LLBC_NewDelegate<Rtn(Args...)>::LLBC_NewDelegate(Func func)
: _obj(nullptr)
{
    _func.func = func;
}

// Ignore pointer to member function warning.
#if LLBC_CUR_COMP == LLBC_COMP_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
#endif // LLBC_CUR_COMP == LLBC_COMP_GCC

template <typename Rtn, typename ...Args>
template <typename Obj>
LLBC_NewDelegate<Rtn(Args...)>::LLBC_NewDelegate(Obj *obj, Rtn(Obj::*meth)(Args...))
{
    void *methParts[2];
    memcpy(methParts, &meth, sizeof(meth));
    _InitByObjAndMethParts(obj, methParts);
}

template <typename Rtn, typename ...Args>
template <typename Obj>
LLBC_NewDelegate<Rtn(Args...)>::LLBC_NewDelegate(const Obj *obj, Rtn(Obj::*meth)(Args...) const)
{
    void *methParts[2];
    memcpy(methParts, &meth, sizeof(meth));
    _InitByObjAndMethParts(const_cast<Obj *>(obj), methParts);
}

// Pop diagnostic settings.
#if LLBC_CUR_COMP == LLBC_COMP_GCC
#pragma GCC diagnostic pop
#endif // LLBC_CUR_COMP == LLBC_COMP_GCC

template <typename Rtn, typename ...Args>
LLBC_NewDelegate<Rtn(Args...)>::operator bool() const
{
    return _func.func != nullptr;
}

template <typename Rtn, typename ...Args>
Rtn LLBC_NewDelegate<Rtn(Args...)>::operator()(Args... args) const
{
    if (_obj)
        return (*_func.meth)(_obj, std::forward<Args>(args)...);
    else
        return (*_func.func)(std::forward<Args>(args)...);
}

template <typename Rtn, typename ...Args>
LLBC_NewDelegate<Rtn(Args...)> &LLBC_NewDelegate<Rtn(Args...)>::operator=(nullptr_t _)
{
    _obj = nullptr;
    memset(&_func, 0, sizeof(_func));

    return *this;
}

template <typename Rtn, typename ...Args>
LLBC_NewDelegate<Rtn(Args...)> &LLBC_NewDelegate<Rtn(Args...)>::operator=(Func func)
{
    _obj = nullptr;
    _func.func = func;

    return *this;
}

template <typename Rtn, typename ...Args>
void LLBC_NewDelegate<Rtn(Args...)>::_InitByObjAndMethParts(void *obj, void *methParts[2])
{
    uint64 objOffset = 0;
    memcpy(&objOffset, &methParts[1], sizeof(void *));
    _obj = reinterpret_cast<sint8 *>(obj) + objOffset;

    memcpy(&_func.meth, &methParts[0], sizeof(void *));
}

__LLBC_NS_END

#endif // __LLBC_CORE_UTILS_UTIL_NEWDELEGATE_H__
