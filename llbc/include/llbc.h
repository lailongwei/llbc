/**
 * @file    llbc.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/03/31
 * @version 1.0
 *
 * @brief   llbc library header file.
 */
#ifndef __LLBC_H__
#define __LLBC_H__

/* before include llbc library, include specific setting header file */
#include "llbc/common/BeforeIncl.h"

/* llbc/common part */
#include "llbc/common/Common.h"
/* llbc/core part */
#include "llbc/core/Core.h"
/* llbc/objbase part */
#include "llbc/objbase/ObjBase.h"
/* llbc/communication part */
#include "llbc/comm/Comm.h"
/* llbc/testcase part */
#include "llbc/testcase/BaseTestCase.h"
/* llbc/application part */
#include "llbc/application/Application.h"

__LLBC_NS_BEGIN

/**
 * LLBC library startup function, before use any LLBC library functions
 * or classes, must call this function to initialize the LLBC library.
 * @return int - return 0 if success, otherwise return -1, to get extended
 *               error information, call LLBC_GetLastError().
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_Startup();

/**
 * LLBC library cleanup function, when your program will terminate,
 * must call this function to cleanup LLBC library.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_Cleanup();

__LLBC_NS_END

/* After include llbc library, include specific setting header file */
#include "llbc/common/AfterIncl.h"

#endif // !__LLBC_H__

