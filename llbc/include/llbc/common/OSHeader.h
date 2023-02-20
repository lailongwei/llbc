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

#include "llbc/common/PFConfig.h"

// OS header files.
#if LLBC_TARGET_PLATFORM_WIN32
 #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
 #endif

 #ifndef WINVER
  #define WINVER 0x600
 #endif

 #include <Windows.h>
 #include <Winsock2.h>
 #include <Mswsock.h>
 #include <Ws2tcpip.h>
 #include <process.h>
 #include <ObjBase.h>
 #include <ShlObj.h>
#pragma warning(disable:4091)
 #include <DbgHelp.h>
#pragma warning(default:4091)
#endif // LLBC_TARGET_PLATFORM_WIN32

#include <sys/stat.h>
#include <sys/types.h>

#if LLBC_TARGET_PLATFORM_NON_WIN32
 #include <netinet/in.h>
 #include <pthread.h>
 #include <sys/time.h>
 #include <sys/socket.h>
 #include <netdb.h>
 #include <semaphore.h>
 #include <arpa/inet.h>

 #if LLBC_TARGET_PLATFORM_LINUX
  #include <sys/epoll.h>
 #endif

 #if LLBC_TARGET_PLATFORM_MAC || LLBC_TARGET_PLATFORM_IPHONE
  #include <sys/param.h>
 #endif

 #if LLBC_TARGET_PLATFORM_MAC
  #include <mach/mach.h>
  #include <mach/mach_time.h>
 #endif

#endif // LLBC_TARGET_PLATFORM_NON_WIN32

#if LLBC_TARGET_PLATFORM_WIN32
 #include <io.h>
 #include <intrin.h>
#endif // LLBC_TARGET_PLATFORM_WIN32

// C standard heder files.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#if LLBC_TARGET_PLATFORM_NON_WIN32
 #include <unistd.h>
#endif

// C++ standard header files.
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include <list>
#include <queue>
#include <deque>
#include <stack>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <limits.h>
#include <limits>
#include <functional>
#include <utility>

// RTTI support header files.
#include <typeinfo>

// Enable posix support, if in WIN32 platform.
#if LLBC_TARGET_PLATFORM_WIN32
 #ifndef _POSIX_
  #define _POSIX_
 #endif // !_POSIX_
#endif // LLBC_TARGET_PLATFORM_WIN32


