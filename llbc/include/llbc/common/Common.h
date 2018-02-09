/**
 * @file    Common.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/03/31
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COM_COMMON_H__
#define __LLBC_COM_COMMON_H__

#include "llbc/common/PFConfig.h"
#include "llbc/common/Compiler.h"

#include "llbc/common/Config.h"

#include "llbc/common/OSHeader.h"
// #include "llbc/common/ThirdHeader.h"
#include "llbc/common/Macro.h"
#include "llbc/common/BasicDataType.h"
#include "llbc/common/Define.h"
#include "llbc/common/Template.h"
#include "llbc/common/Endian.h"
#include "llbc/common/Stream.h"
#include "llbc/common/StringDataType.h"
#include "llbc/common/EventDataType.h"
#include "llbc/common/SocketDataType.h"
#include "llbc/common/Exceptions.h"
#include "llbc/common/Functional.h"

#include "llbc/common/Errno.h"
#include "llbc/common/Errors.h"

#include "llbc/common/RTTI.h"

#include "llbc/common/Version.h"

#include "llbc/common/LibTlsInl.h"

__LLBC_NS_BEGIN

/**
 * LLBC library Common-Module startup function, before use any Common module's
 * functions or classes, must call this function to initialize the module.
 * @return int - return 0 if success, otherwise return -1.
 */

LLBC_EXTERN LLBC_HIDDEN int __LLBC_CommonStartup();

/**
 * LLBC library Common-Module cleanup function.
 */
LLBC_EXTERN LLBC_HIDDEN void __LLBC_CommonCleanup();

__LLBC_NS_END

#endif // !__LLBC_COM_COMMON_H__
