/**
 * @file    Comm.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/26
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/comm/Comm.h"

#include "llbc/comm/PacketHeaderDescAccessor.h"

__LLBC_NS_BEGIN

int __LLBC_CommStartup()
{
    return LLBC_OK;
}

void __LLBC_CommCleanup()
{
    LLBC_PacketHeaderDescAccessor::CleanupHeaderDesc();
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
