/**
 * @file    csllbc.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/25
 * @version 1.0
 *
 * @brief   The llbc lib csharp lang wrapper header file.
 */
#ifndef __CSLLBC_H__
#define __CSLLBC_H__

#include "csllbc/common/Common.h"
#include "csllbc/core/Core.h"
#include "csllbc/comm/Comm.h"

/**
 * Startup csllbc library.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Startup();

/**
 * Startup csllbc library.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Cleanup();

#endif // !__CSLLBC_H__
