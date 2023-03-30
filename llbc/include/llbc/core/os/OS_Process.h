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

#include "llbc/core/utils/Util_Delegate.h"

// Hook process crash support macro define.
#if LLBC_TARGET_PLATFORM_WIN32 || LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_MAC
 #define LLBC_SUPPORT_HOOK_PROCESS_CRASH 1
#else // Non Win32 and Linux
 #define LLBC_SUPPORT_HOOK_PROCESS_CRASH 0
#endif

__LLBC_NS_BEGIN

/**
 * Get current process Id.
 * @return int - the current process Id.
 */
LLBC_EXPORT int LLBC_GetCurrentProcessId();

/**
 * Hook process crash.
 * @param[in] dumpFilePath - the dump file path.
 *                           in Windows platform, is a dump file path, if is empty, dump file path is <your_app_path>.dmp.
 *                           in Non-Windows platform, is a core pattern, if is empty, will use system default config.
 * @param[in] callback     - the crash callback delegate.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_HookProcessCrash(const LLBC_String &dumpFilePath = "",
                                      const LLBC_Delegate<void(const char *stackBacktrace,
                                                               int sig)> &callback = nullptr);

__LLBC_NS_END
