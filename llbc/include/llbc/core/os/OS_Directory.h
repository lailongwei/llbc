/**
 * @file    OS_Directory.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/24
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_OS_OS_DIRECTORY_H__
#define __LLBC_CORE_OS_OS_DIRECTORY_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Check directory exist or not.
 * @param[in] dir - directory path.
 * @return bool - true if exist, otherwise return false.
 */
LLBC_EXTERN LLBC_EXPORT bool LLBC_DirectoryExist(const LLBC_String &dir);

/**
 * Join the directories.
 * @param[in] path1~7 - the will join paths.
 * @return LLBC_String - the joined directory.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_JoinDirectory(const LLBC_String &path1,
                                                       const LLBC_String &path2);
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_JoinDirectory(const LLBC_String &path1,
                                                       const LLBC_String &path2,
                                                       const LLBC_String &path3);
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_JoinDirectory(const LLBC_String &path1,
                                                       const LLBC_String &path2,
                                                       const LLBC_String &path3,
                                                       const LLBC_String &path4);
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_JoinDirectory(const LLBC_String &path1,
                                                       const LLBC_String &path2,
                                                       const LLBC_String &path3,
                                                       const LLBC_String &path4,
                                                       const LLBC_String &path5);
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_JoinDirectory(const LLBC_String &path1,
                                                       const LLBC_String &path2,
                                                       const LLBC_String &path3,
                                                       const LLBC_String &path4,
                                                       const LLBC_String &path5,
                                                       const LLBC_String &path6);
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_JoinDirectory(const LLBC_String &path1,
                                                       const LLBC_String &path2,
                                                       const LLBC_String &path3,
                                                       const LLBC_String &path4,
                                                       const LLBC_String &path5,
                                                       const LLBC_String &path6,
                                                       const LLBC_String &path7);

/**
 * Expand directory, if need.
 * @param[in] path        - directory path.
 * @param[in] appendSlash - append slash or not.
 * @return LLBC_String - full directory path.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_ExpandDirectory(const LLBC_String &path, bool appendSlash = false);

/**
 * Get current executable module file name.
 * @return LLBC_String - the module file name.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_GetModuleFileName();

/**
 * Get current executable module file path.
 * @return LLBC_String - the module file path.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_GetModuleFileDirectory(bool appendSlash = false);

/**
 * Get current directory for the current process.
 * @param[in] appendSlash - auto append slash character flag. default is false.
 * @return LLBC_String - current directory.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_GetCurrentDirectory(bool appendSlash = false);

/**
 * Set current directory.
 * @param[in] curDir - the current directory.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_SetCurrentDirectory(const LLBC_String &curDir);

/**
 * Get home directory.
 * @param[in] appendSlash - auto append slash character flag. default is false.
 * @return LLBC_String - home directory.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_GetHomeDirectory(bool appendSlash = false);

/**
 * Get document directory.
 * @param[in] appendSlash - auto append slash character flag. default is false.
 * @return LLBC_String - document directory.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_GetDocumentDirectory(bool appendSlash = false);

/**
 * Get temporary directory.
 * @param[in] appendSlash - auto append slash character flag. default is false.
 * @return LLBC_String - temporary directory.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_GetTemporaryDirectory(bool appendSlash = false);

/**
 * Get cache directory.
 * @param[in] appendSlash - auto append slash character flag. default is false.
 * @return LLBC_String - temporary directory.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_GetCacheDirectory(bool appendSlash = false);

__LLBC_NS_END

#endif // !__LLBC_CORE_OS_OS_DIRECTORY_H__

