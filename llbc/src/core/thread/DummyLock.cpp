/**
 * @file    DummyLock.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/thread/DummyLock.h"

__LLBC_NS_BEGIN

LLBC_DummyLock::LLBC_DummyLock()
{
}

LLBC_DummyLock::~LLBC_DummyLock()
{
}

void LLBC_DummyLock::Lock()
{
    // ... ...
}

bool LLBC_DummyLock::TryLock()
{
    // ... ...
    return true;
}

void LLBC_DummyLock::Unlock()
{
    // ... ...
}

#if LLBC_TARGET_PLATFORM_NON_WIN32
void *LLBC_DummyLock::Handle()
{
    return NULL;
}
#endif

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
