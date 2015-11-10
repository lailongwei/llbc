/**
 * @file    ReverseGuard.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/10/13
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_THREAD_REVERSE_GUARD_H__
#define __LLBC_CORE_THREAD_REVERSE_GUARD_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN
class LLBC_ILock;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * @\brief The Reverse guardian class encapsulation.
 *         use to ensure lock/unlock in pairs reverse call.
 */
class LLBC_EXPORT LLBC_ReverseGuard
{
public:
    explicit LLBC_ReverseGuard(LLBC_ILock &lock);

    ~LLBC_ReverseGuard();

    LLBC_DISABLE_ASSIGNMENT(LLBC_ReverseGuard);

private:
    LLBC_ILock &_lock;
};

__LLBC_NS_END

#endif // __LLBC_CORE_THREAD_REVERSE_GUARD_H__
