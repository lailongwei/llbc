/**
 * @file    Version.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/18
 * @version 1.0
 *
 * @brief   Library version info header file.
 */
#ifndef __LLBC_COM_VERSION_H__
#define __LLBC_COM_VERSION_H__

#include "llbc/common/PFConfig.h"

#include "llbc/common/OSHeader.h"
#include "llbc/common/StringDataType.h"

__LLBC_NS_BEGIN

/**
 * Version number define.
 */
// Major version.
LLBC_EXTERN LLBC_EXPORT int LLBC_majorVersion;
// Minor version.
LLBC_EXTERN LLBC_EXPORT int LLBC_minorVersion;
// Update number.
LLBC_EXTERN LLBC_EXPORT int LLBC_updateNo;

/**
 * Debug flag.
 */
LLBC_EXTERN LLBC_EXPORT bool LLBC_isDebugVer;

/**
 * Obtain version describe string.
 * @param[in] verbose - verbose option.
 * @return LLBC_String - Version describe.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_GetVersionInfo(bool verbose = false);

__LLBC_NS_END

#endif // !__LLBC_COM_VERSION_H__

