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
#include "llbc/common/BeforeIncl.h"

#include "llbc/common/Errno.h"
#include "llbc/common/LibTlsInl.h"

__LLBC_INTERNAL_NS_BEGIN
static int __g_invalidErrno;
static int __g_invalidSubErrno;
__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

int *__LLBC_Errno()
{
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    if (LIKELY(libTls))
        return &libTls->commonTls.errNo;

    return &(LLBC_INTERNAL_NS __g_invalidErrno);
}

int *__LLBC_SubErrno()
{
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    if (LIKELY(libTls))
        return &libTls->commonTls.subErrNo;

    return &(LLBC_INTERNAL_NS __g_invalidSubErrno);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
