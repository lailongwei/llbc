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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * @brief The Delegate define(undefined).
 * 
 */
template <typename Rtn, typename ...Args>
struct LLBC_Delegate;

/**
 * @brief The Delegate encapsulation(template specification).
 * 
 */
template <typename Rtn, typename ...Args>
struct LLBC_Delegate<Rtn(Args...)>
{
public:
    /**
     * @brief The c-style function pointer type define. 
     * 
     */
    typedef Rtn(*CFunc)(Args...);

    /**
     * @brief The stl function object type define.
     */
    typedef std::function<Rtn(Args...)> StlFunc;

public:
    /**
     * @brief Construct a new invalidate Delegate object.
     * 
     * @param[in] _ - unused parameter. 
     */
    LLBC_Delegate(std::nullptr_t _ = nullptr);

    /**
     * @brief Construct delegate object by c-style function pointer.
     * 
     * @param[in] cfunc - the c-style function pointer.
     */
    LLBC_Delegate(CFunc cfunc);

    /**
     * @brief Construct a new Delegate object by stl function object.
     */
    LLBC_Delegate(const StlFunc &stlFunc);

    /**
     * @brief Construct a delegate object by object pointer and object method pointer.
     * 
     * @param[in] obj  - the object pointer.
     * @param[in] meth - the object method pointer.
     */
    template <typename Obj>
    LLBC_Delegate(Obj *obj, Rtn(Obj::*meth)(Args...));

    /**
     * @brief Construct a delegate object by const object pointer and const object method pointer.
     * 
     * @param[in] obj  - the const object pointer.
     * @param[in] meth - the const object method pointer.
     */
    template <typename Obj>
    LLBC_Delegate(const Obj *, Rtn(Obj::*meth)(Args...) const);

    /**
     * @brief Construct a delegate object by function object other then c-style function/stl function/class method, eg: lambda.
     *
     * @param func - the function object.
     */
    template <typename Func>
    LLBC_Delegate(const Func &func);

    /**
     * @brief Construct delegate object by another delegate object.
     *
     * @param[in] another - the another delegate object.
     */
    LLBC_Delegate(const LLBC_Delegate &another);

    /**
     * @brief Destructor.
     */
    ~LLBC_Delegate();

public:
    /**
     * @brief Check delegate object validate or not.
     * 
     * @return bool - return true if validate, otherwise return false.
     */
    explicit operator bool() const;

    /**
     * @brief Compare the delegate with nullptr.
     * 
     * @return bool - return true if delegate invalidate, otherwise return false.
     */
    bool operator==(std::nullptr_t _) const;

    /**
     * @brief Compare the delegate with nullptr.
     * 
     * @return bool - return true if delegate validate, otherwise return false.
     */
    bool operator!=(std::nullptr_t _) const;

    /**
     * The invoke operator.
     *
     * @param[in] args - the call args.
     * @return Rtn - the return value.
     */
    Rtn operator()(Args... args) const;

public:
    /**
     * @brief Assignemnt by nullptr(make delegate invalidate).
     * 
     * @param[in] _ - unused parameter.
     * @return LLBC_Delegate & - the delegate object.
     */
    LLBC_Delegate &operator=(std::nullptr_t _);

    /**
     * @brief Assignment by c-style function pointer.
     * 
     * @param[in] cfunc - the c-style function pointer.
     * @return LLBC_Delegate & - the delegate object.
     */
    LLBC_Delegate &operator=(CFunc cfunc);

    /**
     * @brief Assignment by stl function object.
     *
     * @return LLBC_Delegate & - the delegate object.
     */
    LLBC_Delegate &operator=(const StlFunc &stlFunc);

    /**
     * @brief Assignment by function object other then c-style function/stl function/class method, eg: lambda.
     *
     * @param func - the function object.
     *
     *@return LLBC_Delegate & - the delegate object.
     */
    template <typename Func>
    LLBC_Delegate &operator=(const Func &func);

    /**
     * @brief Assignment by another delegate object.
     *
     * @return LLBC_Delegate & - the delegate object.
     */
    LLBC_Delegate &operator=(const LLBC_Delegate &another);

private:
    /**
     * @brief Reset delegate object.
     */
    void Reset();

private:
    /**
     * @brief The method holder interface class define.
     */
    struct _IClsMethHolder
    {
        virtual Rtn Invoke(Args... args) const = 0;
    };

    /**
     * @brief The method holder class define.
     */
    template <typename Obj>
    struct _ClsMethHolder : public _IClsMethHolder
    {
    public:
        bool cmeth;
        union
        {
            struct
            {
                Obj *obj;
                Rtn(Obj::*meth)(Args...);
            } meth;

            struct
            {
                const Obj *obj;
                Rtn(Obj:: *meth)(Args...) const;
            } cmeth;
        } meth;

        _ClsMethHolder(Obj *obj, Rtn(Obj::*meth)(Args...))
        : cmeth(false)
        , meth{}
        {
            this->meth.meth.obj = obj;
            this->meth.meth.meth = meth;
        }

        _ClsMethHolder(const Obj *obj, Rtn(Obj::*meth)(Args...) const)
        : cmeth(true)
        , meth{}
        {
            this->meth.cmeth.obj = obj;
            this->meth.cmeth.meth = meth;
        }

        virtual Rtn Invoke(Args... args) const
        {
            return cmeth ?
                (meth.cmeth.obj->*meth.cmeth.meth)(std::forward<Args>(args)...) :
                    (meth.meth.obj->*meth.meth.meth)(std::forward<Args>(args)...);
        }
    };

    /**
     * @brief The delegate holded function union define.
     */
    union _Func
    {
        CFunc cfunc;
        using STLFuncType = std::function<Rtn(Args...)>;
        char stlFunc[sizeof(STLFuncType)];
        char methHolder[sizeof(void *) * 5
            /* sizeof(vptr) + sizeof(bool)(aligement to 8) + sizeof(union meth) : default aligement */];

        _Func()
        {
        }

        _Func(CFunc cfunc)
        : cfunc(cfunc)
        {
        }

        _Func(const std::function<Rtn(Args...)> &stlFunc)
        {
            new (stlFunc) StlFunc(stlFunc);
        }

        template <typename Obj>
        _Func(Obj *obj, Rtn(Obj::*meth)(Args...))
        {
            new (methHolder) _ClsMethHolder<Obj>(obj, meth);
        }

        template <typename Obj>
        _Func(const Obj *obj, Rtn(Obj::*meth)(Args...) const)
        {
            new (methHolder) _ClsMethHolder<Obj>(obj, meth);
        }
    };

    int _funcType; // function type, 0: nullptr, 1: c-style function, 2: stl-style function, 3: class method.
    _Func _func; // function union object.
};

__LLBC_NS_END

#include "llbc/core/utils/Util_DelegateInl.h"


