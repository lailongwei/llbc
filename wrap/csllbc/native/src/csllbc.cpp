/**
 * @file    csllbc.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/25
 * @version 1.0
 *
 * @brief
 */

#include "csllbc/common/Export.h"

#include "csllbc.h"

LLBC_BEGIN_C_DECL

int csllbc_Startup()
{
    return LLBC_Startup();
}

int csllbc_Cleanup()
{
    return LLBC_Cleanup();
}

LLBC_END_C_DECL
