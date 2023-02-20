// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

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


