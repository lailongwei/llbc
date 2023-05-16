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
     */
    LLBC_Delegate(std::nullptr_t _ = nullptr);

    /**
     * @brief Construct delegate object by c-style function pointer.
     */
    LLBC_Delegate(const CFunc &cfunc);

    /**
     * @brief Construct a new Delegate object by stl function object.
     */
    LLBC_Delegate(StlFunc &&stlFunc);
    LLBC_Delegate(const StlFunc &stlFunc);

    /**
     * @brief Construct a delegate object by object pointer and object method pointer.
     */
    template <typename Obj>
    LLBC_Delegate(Obj *obj, Rtn(Obj::*meth)(Args...));

    template <typename Obj>
    LLBC_Delegate(Obj *obj, Rtn(Obj::*constMeth)(Args...) const);

    /**
     * @brief Construct delegate object by another delegate object.
     */
    LLBC_Delegate(LLBC_Delegate &&another);
    LLBC_Delegate(const LLBC_Delegate &another);

    /**
     * @brief Construct a delegate object by callable object.
     */
    template <typename Func>
    LLBC_Delegate(const Func &func);

    /**
     * @brief Destructor.
     */
    ~LLBC_Delegate() = default;

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
     */
    LLBC_Delegate &operator=(std::nullptr_t _);

    /**
     * @brief Assignment by c-styled function.
     */
    LLBC_Delegate &operator=(const CFunc &cfunc);

    /**
     * @brief Assignment by stl function.
     */
    LLBC_Delegate &operator=(StlFunc &&stlFunc);
    LLBC_Delegate &operator=(const StlFunc &stlFunc);

    /**
     * @brief Assignment by another delegate object.
     */
    LLBC_Delegate &operator=(LLBC_Delegate &&another);
    LLBC_Delegate &operator=(const LLBC_Delegate &another);

    /**
     * @brief Assignment by callable object.
     */
    template <typename Func>
    typename std::enable_if<!std::is_same<Func, LLBC_Delegate>::value, LLBC_Delegate &>::type
    operator=(const Func &func);

private:
    CFunc _cfunc;
    StlFunc _stlFunc;
};

__LLBC_NS_END

#include "llbc/core/utils/Util_DelegateInl.h"


