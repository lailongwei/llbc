/**
 * @file    Guard.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_THREAD_GUARD_H__
#define __LLBC_CORE_THREAD_GUARD_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_ILock;

template <typename Rtn, typename Arg1>
class LLBC_IDelegate1;

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
    explicit LLBC_FORCE_INLINE LLBC_DeleteGuard(T *&ptr, bool setNullAfterDelete = true);

    LLBC_FORCE_INLINE ~LLBC_DeleteGuard();

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
    explicit LLBC_FORCE_INLINE LLBC_DeletesGuard(T *&ptr, bool setNullAfterDeletes = true);

    LLBC_FORCE_INLINE ~LLBC_DeletesGuard();

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
    explicit LLBC_FORCE_INLINE LLBC_FreeGuard(T *&ptr, bool setNullAfterFree = true);

    LLBC_FORCE_INLINE ~LLBC_FreeGuard();

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
    /**
     * Create function type invoke guard.
     * @param[in] func - the guard invoke function pointer.
     * @param[in] data - the call data, default is NULL.
     */
    explicit LLBC_FORCE_INLINE LLBC_InvokeGuard(LLBC_GuardFunc func, void *data = NULL);
    
    /**
     * Create method type invoke guard.
     * @param[in] obj  - the class instance pointer.
     * @param[in] meth - the guard invoke method pointer
     * @param[in] data - the call data, default is NULL.
     */
    template <typename Object>
    LLBC_FORCE_INLINE LLBC_InvokeGuard(Object *obj, void (Object::*meth)(void *), void *data = NULL);

    LLBC_FORCE_INLINE ~LLBC_InvokeGuard();

private:
    LLBC_DISABLE_ASSIGNMENT(LLBC_InvokeGuard);

private:
    LLBC_GuardFunc _func;
    LLBC_IDelegate1<void, void *> *_meth;

    void *_data;
};

__LLBC_NS_END

#include "llbc/core/thread/GuardImpl.h"

#endif // __LLBC_CORE_THREAD_GUARD_H__
