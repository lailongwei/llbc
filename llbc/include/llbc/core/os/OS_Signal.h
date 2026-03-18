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

#include "llbc/common/Common.h"

#if LLBC_TARGET_PLATFORM_WIN32
LLBC_EXPORT char *strsignal(int sig);
#endif

__LLBC_NS_BEGIN

/**
 * @brief The llbc framework signal handler type.
 * @param[in] recvThreadId - the id of the thread that received the signal.
 * @param[in] sig          - the signal number.
 * @param[in] sigValue     - the signal value(always 0 on Windows platform).
 */
typedef void (*LLBC_SignalHandler)(int recvThreadId, int sig, int sigValue);

/**
 * Set signal handler.
 * Notes:
 * 1. Cannot set <LLBC_CFG_OS_CRASH_SIGNALS> signals via this function;
 *    to set crash signal handlers, use LLBC_SetCrashHandler() instead.
 * 2. On Windows platform, the sigValue argument in handler is always 0.
 * 3. If handler is nullptr, any pending signal for this sig will be silently
 *    discarded on the next LLBC_ProcessReceivedSignals() call.
 *
 * @param[in] sig     - the signal number, must be valid.
 * @param[in] handler - the signal handler; if nullptr, clears the handler and restores default behavior.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_SetSignalHandler(int sig, LLBC_SignalHandler handler);

/**
 * Get signal handler.
 * @param[in] sig - the signal, must be a valid signal number.
 * @return LLBC_SignalHandler - the signal handler, or nullptr if not set or an error occurred.
 *                              If nullptr is returned, use LLBC_GetLastError() to distinguish:
 *                              - LLBC_ERROR_SUCCESS: the signal handler is not set.
 *                              - Other error codes: an error occurred (e.g. invalid signal number).
 */
LLBC_EXPORT LLBC_SignalHandler LLBC_GetSignalHandler(int sig);

/**
 * @brief Process received signals. This function should be called in the llbc entry thread loop.
 * If you are using LLBC_App, you don't need to call this function manually;
 * otherwise, call it periodically.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_ProcessReceivedSignals();

__LLBC_NS_END