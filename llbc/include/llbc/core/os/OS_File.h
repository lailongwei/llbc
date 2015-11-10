/**
 * @file    OS_File.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/10/14
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_OS_OS_FILE_H__
#define __LLBC_CORE_OS_OS_FILE_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Get the file number.
 * @param[in] file - the file pointer.
 * @return int - the file number, if failed, return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_GetFileNo(FILE *file);

/**
 * Check file exist or not.
 * @param[in] file - file name.
 * @return bool - true if success, otherwise return false.
 */
LLBC_EXTERN LLBC_EXPORT bool LLBC_FileExist(const LLBC_String &file);

__LLBC_NS_END

#endif // !__LLBC_CORE_OS_OS_FILE_H__
