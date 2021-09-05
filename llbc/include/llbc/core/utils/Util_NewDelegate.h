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

#ifndef __LLBC_CORE_UTILS_UTIL_NEWDELEGATE_H__
#define __LLBC_CORE_UTILS_UTIL_NEWDELEGATE_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * @brief The Delegate define(undefined).
 * 
 */
template <typename Rtn, typename ...Args>
struct LLBC_NewDelegate;

/**
 * @brief The Delegate encapsulation(template specification).
 * 
 */
template <typename Rtn, typename ...Args>
struct LLBC_NewDelegate<Rtn(Args...)>
{
    /**
     * @brief The function pointer type define. 
     * 
     */
    typedef Rtn(*Func)(Args...);

    /**
     * @brief The Class method point type define(as function).
     * 
     */
    typedef Rtn(*Meth)(void *, Args...);

public:
    /**
     * @brief Construct a new invalidate Delegate object.
     * 
     * @param[in] _ - unused parameter. 
     */
    LLBC_NewDelegate(nullptr_t _);

    /**
     * @brief Construct a new Delegate object by function pointer.
     * 
     * @param[in] func 
     */
    LLBC_NewDelegate(Func func);

    /**
     * @brief Construct a new Delegate object by object pointer and object method pointer.
     * 
     * @param[in] obj  - the object pointer.
     * @param[in] meth - the object method pointer.
     */
    template <typename Obj>
    LLBC_NewDelegate(Obj *obj, Rtn(Obj::*meth)(Args...));

    /**
     * @brief Construct a new Delegate object by const object pointer and const object method pointer.
     * 
     * @param[in] obj  - the const object pointer.
     * @param[in] meth - the const object method pointer.
     */
    template <typename Obj>
    LLBC_NewDelegate(const Obj *, Rtn(Obj::*meth)(Args...) const);

public:
    /**
     * @brief Check delegate object validate or not.
     * 
     * @return bool - return true if validate, otherwise return false.
     */
    explicit operator bool() const;

    /**
     * The invoke operator. 
     * @param[in] args - the call args.
     * @return Rtn - the return value.
     */
    Rtn operator()(Args... args) const;

public:
    /**
     * @brief Assignemnt by nullptr(make delegate invalidate).
     * 
     * @param[in] _ - unused parameter.
     * @return LLBC_NewDelegate & - the delegate object.
     */
    LLBC_NewDelegate &operator=(nullptr_t _);

    /**
     * @brief Assignment by new function pointer.
     * 
     * @param[in] func - the function pointer.
     * @return LLBC_NewDelegate & - the delegate object.
     */
    LLBC_NewDelegate &operator=(Func func);

private:
    /**
     * @brief Init delegate object by object pointer and method parts.
     * 
     * @param[in] obj   - the object pointer.
     * @param methParts - the method parts.
     */
    void _InitByObjAndMethParts(void *obj, void *methParts[2]);

private:
    void *_obj;
    union
    {
        Func func;
        Meth meth;
    } _func;
};

__LLBC_NS_END

#include "llbc/core/utils/Util_NewDelegateImpl.h"

#endif // !__LLBC_CORE_UTILS_UTIL_NEWDELEGATE_H__
