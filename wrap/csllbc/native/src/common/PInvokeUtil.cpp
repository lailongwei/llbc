/**
 * @file    PInvokeUtil.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/09/10
 * @version 1.0
 *
 * @brief
 */

#include "csllbc/common/Export.h"

#include "csllbc/common/PInvokeUtil.h"

char *csllbc_PInvokeUtil::BuildCSharpStr(const LLBC_String &nativeStr, int *nativeStrLen)
{
    size_t len = 0;
    char *csStr = BuildCSharpStr(nativeStr, &len);
    if (nativeStrLen)
        *nativeStrLen = static_cast<int>(len);

    return csStr;
}

char *csllbc_PInvokeUtil::BuildCSharpStr(const LLBC_String &nativeStr, size_t *nativeStrLen)
{
    if (nativeStr.empty())
        return NULL;

    char *csStr = BuildCSharpStr(nativeStr.c_str(), nativeStr.length());
    if (nativeStrLen)
        *nativeStrLen = nativeStr.length();

    return csStr;
}

char *csllbc_PInvokeUtil::BuildCSharpStr(const char *nativeStr, size_t nativeStrLen)
{
    if (UNLIKELY(nativeStr == NULL || nativeStrLen == 0))
        return NULL;

    char *csStr = LLBC_Malloc(char, nativeStrLen);
    LLBC_MemCpy(csStr, nativeStr, nativeStrLen);

    return csStr;
}