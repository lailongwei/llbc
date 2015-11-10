/**
 * @file    OS_Library.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/10/01
 * @version 1.0
 *
 * @brief   Stand for library on different operation systems.
 */
#ifndef __LLBC_CORE_OS_OS_LIBRARY_H__
#define __LLBC_CORE_OS_OS_LIBRARY_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * The function maps the specified executable module into the address space of the calling process.
 * @param[in] fileName - module file name.
 * @return LLBC_LibraryHandle - Library handle.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_LibraryHandle LLBC_LoadLibrary(const char *fileName);

/**
 * Retrieves the address of specified exported function from library.
 * @param[in] handle   - library handle.
 * @param[in] procName - function name.
 * @return LLBC_LibraryFun - library func address, if return NULL, see LLBC_GetLastError().
 */
LLBC_EXTERN LLBC_EXPORT LLBC_LibraryFun LLBC_GetProcAddress(LLBC_LibraryHandle handle, const char *procName);

/**
 * Close library.
 * @param[in] handle - library handle.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_CloseLibrary(LLBC_LibraryHandle handle);

__LLBC_NS_END

#endif // !__LLBC_CORE_OS_OS_LIBRARY_H__
