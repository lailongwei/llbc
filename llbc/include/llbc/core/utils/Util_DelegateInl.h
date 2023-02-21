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
: _funcType(0)
, _func()
{
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)>::LLBC_Delegate(CFunc cfunc)
: _funcType(cfunc ? 1 : 0)
, _func(cfunc)
{
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)>::LLBC_Delegate(const StlFunc &stlFunc)
: _funcType(stlFunc != nullptr ? 2 : 0)
, _func()
{
    if (_funcType == 2)
        new (_func.stlFunc) StlFunc(stlFunc);
}

template <typename Rtn, typename ...Args>
template <typename Obj>
LLBC_Delegate<Rtn(Args...)>::LLBC_Delegate(Obj *obj, Rtn(Obj::*meth)(Args...))
: _funcType(obj != nullptr && meth != nullptr ? 3 : 0)
, _func()
{
    if (_funcType == 3)
        new (_func.methHolder) _ClsMethHolder<Obj>(obj, meth);
}

template <typename Rtn, typename ...Args>
template <typename Obj>
LLBC_Delegate<Rtn(Args...)>::LLBC_Delegate(const Obj *obj, Rtn(Obj::*meth)(Args...) const)
: _funcType(obj != nullptr && meth != nullptr ? 3 : 0)
, _func()
{
    if (_funcType == 3)
        new (_func.methHolder) _ClsMethHolder<Obj>(obj, meth);
}

template <typename Rtn, typename ...Args>
template <typename Func>
LLBC_Delegate<Rtn(Args...)>::LLBC_Delegate(const Func &func)
: LLBC_Delegate(StlFunc(func))
{
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)>::LLBC_Delegate(const LLBC_Delegate &another)
: _funcType(another._funcType)
, _func()
{
    if (_funcType == 1)
        _func.cfunc = another._func.cfunc;
    else if (_funcType == 2)
        new (_func.stlFunc) StlFunc(*reinterpret_cast<const StlFunc *>(another._func.stlFunc));
    else if (_funcType == 3)
        memcpy(_func.methHolder, another._func.methHolder, sizeof(_func.methHolder));
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)>::~LLBC_Delegate()
{
    if (_funcType == 2)
        reinterpret_cast<StlFunc *>(_func.stlFunc)->~StlFunc();
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)>::operator bool() const
{
    return _funcType != 0;
}

template <typename Rtn, typename ...Args>
bool LLBC_Delegate<Rtn(Args...)>::operator==(std::nullptr_t _) const
{
    return _funcType == 0;
}

template <typename Rtn, typename ...Args>
bool LLBC_Delegate<Rtn(Args...)>::operator!=(std::nullptr_t _) const
{
    return _funcType != 0;
}

template <typename Rtn, typename ...Args>
Rtn LLBC_Delegate<Rtn(Args...)>::operator()(Args... args) const
{
    if (_funcType == 1)
        return (*_func.cfunc)(std::forward<Args>(args)...);
    else if (_funcType == 2)
        return (*reinterpret_cast<const StlFunc *>(_func.stlFunc))(std::forward<Args>(args)...);
    else if (_funcType == 3)
        return reinterpret_cast<const _IClsMethHolder *>(_func.methHolder)->Invoke(std::forward<Args>(args)...);
    else
        return Rtn();
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)> &LLBC_Delegate<Rtn(Args...)>::operator=(std::nullptr_t _)
{
    Reset();
    return *this;
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)> &LLBC_Delegate<Rtn(Args...)>::operator=(CFunc cfunc)
{
    if (cfunc == nullptr)
    {
        Reset();
        return *this;
    }

    if (_funcType == 2)
        reinterpret_cast<StlFunc *>(_func.stlFunc)->~StlFunc();

    _funcType = 1;
    _func.cfunc = cfunc;

    return *this;
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)> &LLBC_Delegate<Rtn(Args...)>::operator=(const StlFunc &stlFunc)
{
    if (stlFunc == nullptr)
    {
        Reset();
        return *this;
    }

    if (_funcType == 2)
    {
        *reinterpret_cast<StlFunc *>(_func.stlFunc) = stlFunc;
    }
    else
    {
        _funcType = 2;
        new (_func.stlFunc) StlFunc(stlFunc);
    }

    return *this;
}

template <typename Rtn, typename ... Args>
template <typename Func>
LLBC_Delegate<Rtn(Args ...)> &LLBC_Delegate<Rtn(Args ...)>::operator=(const Func &func)
{
    return operator=(StlFunc(func));
}

template <typename Rtn, typename ...Args>
LLBC_Delegate<Rtn(Args...)> &LLBC_Delegate<Rtn(Args...)>::operator=(const LLBC_Delegate<Rtn(Args...)> &another)
{
    if (this == &another)
        return *this;

    if (another._funcType == 1)
    {
        if (_funcType == 2)
            reinterpret_cast<StlFunc *>(_func.methHolder)->~StlFunc();

        _funcType = 1;
        _func.cfunc = another._func.cfunc;
    }
    else if (another._funcType == 2)
    {
        if (_funcType == 2)
        {
            *reinterpret_cast<StlFunc *>(_func.stlFunc) =
                *reinterpret_cast<const StlFunc *>(another._func.stlFunc);
        }
        else
        {
            _funcType = 2;
            new (_func.stlFunc) StlFunc(*reinterpret_cast<const StlFunc *>(another._func.stlFunc));
        }
    }
    else if (another._funcType == 3)
    {
        if (_funcType == 2)
            reinterpret_cast<StlFunc *>(_func.stlFunc)->~StlFunc();

        _funcType = 3;
        memcpy(_func.methHolder, another._func.methHolder, sizeof(_func.methHolder));
    }
    else
    {
        Reset();
    }

    return *this;
}

template <typename Rtn, typename ...Args>
void LLBC_Delegate<Rtn(Args...)>::Reset()
{
    if (_funcType == 2)
        reinterpret_cast<StlFunc *>(_func.stlFunc)->~StlFunc();

    _funcType = 0;
}

__LLBC_NS_END
