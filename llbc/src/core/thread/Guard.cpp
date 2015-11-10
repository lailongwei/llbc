/**
 * @file    Guard.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/thread/ILock.h"
#include "llbc/core/thread/Guard.h"

__LLBC_NS_BEGIN

LLBC_Guard::LLBC_Guard(LLBC_ILock &lock):_lock(lock)
{
    _lock.Lock();
}

LLBC_Guard::~LLBC_Guard()
{
    _lock.Unlock();
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
