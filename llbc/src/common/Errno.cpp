/**
 * @file    Errno.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/05
 * @version 1.0
 *
 * @brief
 */

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
