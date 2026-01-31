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

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)>::LLBC_Delegate(std::nullptr_t _)
: _cFunc(nullptr)
, _stlFunc()
{
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)>::LLBC_Delegate(const CFunc &cFunc)
: _cFunc(cFunc)
, _stlFunc()
{
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)>::LLBC_Delegate(StlFunc &&stlFunc)
: _cFunc(nullptr)
, _stlFunc(std::move(stlFunc))
{
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)>::LLBC_Delegate(const StlFunc &stlFunc)
: _cFunc(nullptr)
, _stlFunc(stlFunc)
{
}

template <typename Rtn, typename ...Args>
template <typename Obj>
LLBC_Delegate<Rtn(Args...)>::LLBC_Delegate(Obj *obj, Rtn(Obj::*meth)(Args...))
: _cFunc(nullptr)
, _stlFunc(LLBC_BindClassMethod(obj, meth))
{
}

template <typename Rtn, typename ...Args>
template <typename Obj>
LLBC_Delegate<Rtn(Args...)>::LLBC_Delegate(Obj *obj, Rtn(Obj::*constMeth)(Args...) const)
: _cFunc(nullptr)
, _stlFunc(LLBC_BindClassMethod(obj, constMeth))
{
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)>::LLBC_Delegate(LLBC_Delegate &&another) noexcept
: _cFunc(another._cFunc)
, _stlFunc(std::move(another._stlFunc))
{
    another._cFunc = nullptr;
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)>::LLBC_Delegate(const LLBC_Delegate &another)
: _cFunc(another._cFunc)
, _stlFunc(another._stlFunc)
{
}

template <typename Rtn, typename ...Args>
template <typename Func>
LLBC_Delegate<Rtn(Args...)>::LLBC_Delegate(const Func &func)
: _cFunc(nullptr)
, _stlFunc(func)
{
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)>::operator bool() const
{
    return _cFunc != nullptr || _stlFunc != nullptr;
}

template <typename Rtn, typename ...Args>
bool LLBC_Delegate<Rtn(Args...)>::operator==(std::nullptr_t _) const
{
    return _cFunc == nullptr && _stlFunc == nullptr;
}

template <typename Rtn, typename ...Args>
bool LLBC_Delegate<Rtn(Args...)>::operator!=(std::nullptr_t _) const
{
    return _cFunc != nullptr || _stlFunc != nullptr;
}

template <typename Rtn, typename ...Args>
Rtn LLBC_Delegate<Rtn(Args...)>::operator()(Args... args) const
{
    if (_cFunc)
        return (*_cFunc)(std::forward<Args>(args)...);
    else
        return _stlFunc(std::forward<Args>(args)...);
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)> &LLBC_Delegate<Rtn(Args...)>::operator=(std::nullptr_t _)
{
    _cFunc = nullptr;
    _stlFunc = nullptr;

    return *this;
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)> &LLBC_Delegate<Rtn(Args...)>::operator=(const CFunc &cFunc)
{
    _cFunc = cFunc;
    _stlFunc = nullptr;

    return *this;
}


template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)> &LLBC_Delegate<Rtn(Args...)>::operator=(StlFunc &&stlFunc) noexcept
{
    _cFunc = nullptr;
    _stlFunc = std::move(stlFunc);

    return *this;
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)> &LLBC_Delegate<Rtn(Args...)>::operator=(const StlFunc &stlFunc)
{
    _cFunc = nullptr;
    _stlFunc = stlFunc;

    return *this;
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)> &LLBC_Delegate<Rtn(Args...)>::operator=(LLBC_Delegate<Rtn(Args...)> &&another) noexcept
{
    if (UNLIKELY(&another == this))
        return *this;

    if (another._cFunc)
    {
        _cFunc = another._cFunc;
        another._cFunc = nullptr;
        _stlFunc = nullptr;
    }
    else
    {
        _stlFunc = std::move(another._stlFunc);
        _cFunc = nullptr;
    }

    return *this;
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)> &LLBC_Delegate<Rtn(Args...)>::operator=(const LLBC_Delegate<Rtn(Args...)> &another)
{
    if (UNLIKELY(&another == this))
        return *this;

    if (another._cFunc)
    {
        _cFunc = another._cFunc;
        _stlFunc = nullptr;
    }
    else
    {
        _cFunc = nullptr;
        _stlFunc = another._stlFunc;
    }

    return *this;
}

template <typename Rtn, typename ...Args>
template <typename Func>
std::enable_if_t<!std::is_same_v<Func, LLBC_Delegate<Rtn(Args...)> >, LLBC_Delegate<Rtn(Args...)> &>
LLBC_Delegate<Rtn(Args...)>::operator=(const Func &func)
{
    _cFunc = nullptr;
    _stlFunc = func;

    return *this;
}

__LLBC_NS_END
