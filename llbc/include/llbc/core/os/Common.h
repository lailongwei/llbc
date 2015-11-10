/**
 * @file    Common.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_OS_COMMON_H__
#define __LLBC_CORE_OS_COMMON_H__

#include "llbc/core/os/OS_Time.h"
#include "llbc/core/os/OS_Atomic.h"
#include "llbc/core/os/OS_File.h"
#include "llbc/core/os/OS_Directory.h"
#include "llbc/core/os/OS_Library.h"
#include "llbc/core/os/OS_Thread.h"
#include "llbc/core/os/OS_Console.h"
#include "llbc/core/os/OS_Socket.h"
#include "llbc/core/os/OS_Select.h"
#include "llbc/core/os/OS_Network.h"

#if LLBC_TARGET_PLATFORM_LINUX
#include "llbc/core/os/OS_Epoll.h"
#endif
#if LLBC_TARGET_PLATFORM_WIN32
#include "llbc/core/os/OS_Iocp.h"
#endif

#endif // __LLBC_CORE_OS_COMMON_H__
