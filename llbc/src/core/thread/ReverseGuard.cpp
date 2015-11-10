/**
 * @file    ReverseGuard.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/10/13
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/thread/ILock.h"
#include "llbc/core/thread/ReverseGuard.h"

__LLBC_NS_BEGIN

LLBC_ReverseGuard::LLBC_ReverseGuard(LLBC_ILock &lock)
: _lock(lock)
{
    _lock.Unlock();
}

LLBC_ReverseGuard::~LLBC_ReverseGuard()
{
    _lock.Lock();
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
