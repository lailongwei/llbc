// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

/* llbc/common part */
#include "llbc/common/Common.h"
/* llbc/core part */
#include "llbc/core/Core.h"
/* llbc/communication part */
#include "llbc/comm/Comm.h"
/* llbc/testcase part */
#include "llbc/testcase/BaseTestCase.h"
/* llbc/app part */
#include "llbc/app/App.h"

__LLBC_NS_BEGIN

/**
 * LLBC library startup function, before use any LLBC library functions
 * or classes, must call this function to initialize the LLBC library.
 * @param[in] initWinSock - init windows socket library, only available on Windows platform, default true.
 * @return int - return 0 if success, otherwise return -1, to get extended
 *               error information, call LLBC_GetLastError().
 */
LLBC_EXPORT int LLBC_Startup(bool initWinSock = true);

/**
 * LLBC library cleanup function, when your program will terminate,
 * must call this function to cleanup LLBC library.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_Cleanup();

__LLBC_NS_END
