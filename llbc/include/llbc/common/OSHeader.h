/**
 * @file    OSHeader.h
 * @author  Longwei Lai
 * @date    2013/03/31
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COM_OS_HEADER_H__
#define __LLBC_COM_OS_HEADER_H__

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
#endif // LLBC_TARGET_PLATFORM_WIN32

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/timeb.h>

#if LLBC_TARGET_PLATFORM_NON_WIN32
 #include <fcntl.h>
 #include <netinet/in.h>
 #include <pthread.h>
 #include <libgen.h>
 #include <sys/time.h>
 #include <sys/socket.h>
 #include <netdb.h>
 #include <dirent.h>
 #include <semaphore.h>
 #include <arpa/inet.h>
 #include <dlfcn.h>

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
#include <signal.h>
#include <math.h>
#include <time.h>
#if LLBC_TARGET_PLATFORM_NON_WIN32
 #include <unistd.h>
#endif

// C++ standard header files.
#include <iostream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <vector>
#include <list>
#include <queue>
#include <deque>
#include <stack>
#include <map>
#include <set>
#include <algorithm>
#include <functional>
#include <limits.h>

// UUID lib header file.
#if LLBC_TARGET_PLATFORM_NON_WIN32
#include <uuid/uuid.h>
#endif

// iconv lib header file.
#if LLBC_TARGET_PLATFORM_NON_WIN32
#include <iconv.h>
#endif

// Enable posix support, if in WIN32 platform.
#if LLBC_TARGET_PLATFORM_WIN32
 #ifndef _POSIX_
  #define _POSIX_
 #endif // !_POSIX_
#endif // LLBC_TARGET_PLATFORM_WIN32

#endif // !__LLBC_COM_OS_HEADER_H__
