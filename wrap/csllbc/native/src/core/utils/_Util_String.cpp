/**
 * @file    _Util_String.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/07/03
 * @version 1.0
 *
 * @brief
 */

#include "csllbc/common/Export.h"

#include "csllbc/core/utils/_Util_String.h"

LLBC_BEGIN_C_DECL

int csllbc_Utils_String_HashString(const char *str, int strLen)
{
    return LLBC_HashString(str, strLen);
}

LLBC_END_C_DECL