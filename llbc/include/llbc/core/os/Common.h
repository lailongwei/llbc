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

#ifndef __LLBC_CORE_OS_COMMON_H__
#define __LLBC_CORE_OS_COMMON_H__

#include "llbc/core/os/OS_Time.h"
#include "llbc/core/os/OS_Atomic.h"
#include "llbc/core/os/OS_Library.h"
#include "llbc/core/os/OS_Thread.h"
#include "llbc/core/os/OS_Process.h"
#include "llbc/core/os/OS_Console.h"
#include "llbc/core/os/OS_Socket.h"
#include "llbc/core/os/OS_Select.h"
#include "llbc/core/os/OS_Network.h"
#include "llbc/core/os/OS_Symbol.h"

#if LLBC_TARGET_PLATFORM_LINUX
#include "llbc/core/os/OS_Epoll.h"
#endif
#if LLBC_TARGET_PLATFORM_WIN32
#include "llbc/core/os/OS_Iocp.h"
#endif

#endif // __LLBC_CORE_OS_COMMON_H__
