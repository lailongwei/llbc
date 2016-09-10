/**
 * @file    PInvokeUtil.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/09/10
 * @version 1.0
 *
 * @brief
 */
#ifndef __CSLLBC_COM_PINVOKE_UTIL_H__
#define __CSLLBC_COM_PINVOKE_UTIL_H__

#include "csllbc/common/LibHeader.h"
#include "csllbc/common/Macro.h"
#include "csllbc/common/Export.h"

/**
 * \brief The csllbc PInvoke util class encapsulation.
 */
class CSLLBC_EXPORT csllbc_PInvokeUtil
{
public:
    /**
     * Build csharp layer string.
     * Note:
     *      CSharp layer string is allocated from global heap space, You must free it after used.
     *      CSharp layer free method: LibUtil.FreeNativePtr() or Marshal.FreeHGlobal()
     * @param[in] nativeStr     - the c++ native string.
     * @param[out] nativeStrLen - the native string length, optional.
     * @return char * - the csharp layer string.
     */
    static char *BuildCSharpStr(const LLBC_String &nativeStr, int *nativeStrLen);
    static char *BuildCSharpStr(const LLBC_String &nativeStr, size_t *nativeStrLen);

    /**
     * Build csharp layer string.
     * Note:
     *      CSharp layer string is allocated from global heap space, You must free it after used.
     *      CSharp layer free method: LibUtil.FreeNativePtr() or Marshal.FreeHGlobal()
     * @param[in] nativeStr    - the c++ native string.
     * @param[in] nativeStrLen - the native string length.
     * @return char * - the csharp layer string.
     */
    static char *BuildCSharpStr(const char *nativeStr, size_t nativeStrLen);
};

#endif // !__CSLLBC_COM_PINVOKE_UTIL_H__
