/**
 * @file    Library.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/10/01
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Library.h"
#include "llbc/core/library/Library.h"

__LLBC_NS_BEGIN

LLBC_Library::LLBC_Library()
: _handle(LLBC_INVALID_LIBRARY_HANDLE)
{
}

LLBC_Library::~LLBC_Library()
{
    (void)this->Close();
}

int LLBC_Library::Open(const char *fileName)
{
    if (_handle != LLBC_INVALID_LIBRARY_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_RTN_FAILED;
    }

    if ((_handle = LLBC_LoadLibrary(fileName)) == LLBC_INVALID_LIBRARY_HANDLE)
        return LLBC_RTN_FAILED;

    return LLBC_RTN_OK;
}

LLBC_LibraryFun LLBC_Library::GetProcAddress(const char *procName)
{
    return LLBC_GetProcAddress(_handle, procName);
}

int LLBC_Library::Close()
{
    if (_handle == LLBC_INVALID_LIBRARY_HANDLE)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_OPEN);
        return LLBC_RTN_FAILED;
    }

    if (LLBC_CloseLibrary(_handle) != LLBC_RTN_OK)
        return LLBC_RTN_FAILED;

    _handle = LLBC_INVALID_LIBRARY_HANDLE;

    return LLBC_RTN_OK;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
