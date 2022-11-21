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


#include "llbc/common/Export.h"

#include "llbc.h"

__LLBC_INTERNAL_NS_BEGIN

volatile bool __llbc_inited = false;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

int LLBC_Startup(bool initWinSock)
{
    // ReEntry judge.
    if (LLBC_INTERNAL_NS __llbc_inited)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    // Init common module.
    if (__LLBC_CommonStartup() != LLBC_OK)
        return LLBC_FAILED;

    // Set some init parameters.
    __LLBC_GetLibTls()->coreTls.needInitWinSock = initWinSock;

    // Init core module.
    if (__LLBC_CoreStartup() != LLBC_OK)
        return LLBC_FAILED;

    // Init communication module
    if (__LLBC_CommStartup() != LLBC_OK)
        return LLBC_FAILED;

    LLBC_INTERNAL_NS __llbc_inited = true;

    return LLBC_OK;
}

int LLBC_Cleanup()
{
    if (!LLBC_INTERNAL_NS __llbc_inited)
        return LLBC_FAILED;

    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    if (!tls->coreTls.entryThread)
    {
        LLBC_SetLastError(LLBC_ERROR_ACCESS_DENY);
        return LLBC_FAILED;
    }

    __LLBC_CommCleanup();

    __LLBC_CoreCleanup();

    __LLBC_CommonCleanup();

    LLBC_INTERNAL_NS __llbc_inited = false;

    return LLBC_OK;
}

__LLBC_NS_END
