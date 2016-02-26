/**
 * @file    llbc.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/05
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc.h"

__LLBC_INTERNAL_NS_BEGIN

volatile bool __llbc_inited = false;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

int LLBC_Startup()
{
    if(LLBC_INTERNAL_NS __llbc_inited)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    if(__LLBC_CommonStartup() != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    if(__LLBC_CoreStartup() != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    if(__LLBC_CommStartup() != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    LLBC_INTERNAL_NS __llbc_inited = true;

    return LLBC_OK;
}

int LLBC_Cleanup()
{
    if(!LLBC_INTERNAL_NS __llbc_inited)
    {
        return LLBC_FAILED;
    }

    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    if(!tls->coreTls.entryThread)
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

#include "llbc/common/AfterIncl.h"
