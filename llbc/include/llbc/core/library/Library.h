/**
 * @file    Library.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/10/01
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LIBRARY_LIBRARY_H__
#define __LLBC_CORE_LIBRARY_LIBRARY_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \breif Stand for library on different operation systems.
 */
class LLBC_EXPORT LLBC_Library
{
public:
    LLBC_Library();
    ~LLBC_Library();

public:
    /**
     * Open library.
     * @param[in] fileName - library name.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Open(const char *fileName);

    /**
     * Get library process address.
     * @param[in] procName - Pointer to a null-terminated string containing the function name.
     * @return LLBC_LibraryFun - library function address, if failed, return NULL.
     */
    LLBC_LibraryFun GetProcAddress(const char *procName);

    /**
     * Close library.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Close();

    LLBC_DISABLE_ASSIGNMENT(LLBC_Library);

private:
    LLBC_LibraryHandle _handle;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LIBRARY_LIBRARY_H__
