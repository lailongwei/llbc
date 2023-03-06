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
 * Pre-declare some classes.
 */
class LLBC_ILock;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief Lock guardian class encapsulation.
 *        Use to ensure lock/unlock in pairs call.
 */
class LLBC_EXPORT LLBC_LockGuard
{
public:
    explicit LLBC_LockGuard(LLBC_ILock &lock, bool reverse = false);

    ~LLBC_LockGuard();

    LLBC_DISABLE_ASSIGNMENT(LLBC_LockGuard);

private:
    LLBC_ILock &_lock;
    bool _reverse;
};

/**
 * \brief Delete guardian class encapsulation.
 *        Use to ensure pointer delete when leave scope.
 */
template <typename T>
class LLBC_DeleteGuard
{
public:
    explicit LLBC_DeleteGuard(T *&ptr, bool setNullAfterDelete = true);

    ~LLBC_DeleteGuard();

    LLBC_DISABLE_ASSIGNMENT(LLBC_DeleteGuard);

private:
    T *&_ptr;
    bool _setNullAfterDelete;
};

/**
 * \brief Delete object array guardian class encapsulation.
 *        Use to ensure object array delete when leave scope.
 */
template <typename T>
class LLBC_DeletesGuard
{
public:
    explicit LLBC_DeletesGuard(T *&ptr, bool setNullAfterDeletes = true);

    ~LLBC_DeletesGuard();

    LLBC_DISABLE_ASSIGNMENT(LLBC_DeletesGuard);

private:
    T *&_ptr;
    bool _setNullAfterDeletes;
};

/**
 * \brief Free memory guardian class encapsulation.
 *        Use to ensure memory free when leave scope.
 */
template <typename T>
class LLBC_FreeGuard
{
public:
    explicit LLBC_FreeGuard(T *&ptr, bool setNullAfterFree = true);

    ~LLBC_FreeGuard();

    LLBC_DISABLE_ASSIGNMENT(LLBC_FreeGuard);

private:
    T *&_ptr;
    bool _setNullAfterFree;
};

/**
 * \brief The invoke guard helper class encapsulation.
 */
class LLBC_InvokeGuard
{
public:
    template <typename Func, typename ...Args>
    explicit LLBC_InvokeGuard(const Func &func, Args ...args);

    /**
     * Create invoke guard by class method, and arguments.
     * @param[in] obj  - the object pointer.
     * @param[in] meth - the class method.
     * @param[in] args - the guard call arguments.
     */
    template <typename Obj, typename ...Args>
    explicit LLBC_InvokeGuard(Obj *obj, void (Obj::*meth)(Args...), Args ...args);

    ~LLBC_InvokeGuard();

private:
    LLBC_DISABLE_ASSIGNMENT(LLBC_InvokeGuard);

private:
    std::function<void()> _deleg;
};

__LLBC_NS_END

#include "llbc/core/thread/GuardInl.h"


