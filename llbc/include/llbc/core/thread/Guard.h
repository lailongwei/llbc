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
class LLBC_ILock;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief Guardian class encapsulation.
 *        Use to ensure lock/unlock in pairs call.
 */
class LLBC_EXPORT LLBC_Guard
{
public:
    explicit LLBC_Guard(LLBC_ILock &lock);

    ~LLBC_Guard();

    LLBC_DISABLE_ASSIGNMENT(LLBC_Guard);

private:
    LLBC_ILock &_lock;
};

__LLBC_NS_END

#endif // __LLBC_CORE_THREAD_GUARD_H__
