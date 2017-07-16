/**
 * @file    Common.cpp
 * @author  Longwei lai<lailongwei@126.com>
 * @date    2017-07-07
 * @version 1.0
 *
 * @brief
 */

#include "lullbc/common/Export.h"

#include "lullbc/common/Common.h"

void lullbc_InitCommon()
{
    // Register lullbc common errno.
    LLBC_AddCustomErrno(LULLBC_ERROR_COMMON, "lullbc error");
}