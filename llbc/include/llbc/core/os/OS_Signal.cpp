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
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <signal.h>

#include "llbc/common/Export.h"

#include "llbc/core/os/OS_Thread.h"
#include "llbc/core/os/OS_Atomic.h"

#include "llbc/core/os/OS_Signal.h"

#if LLBC_TARGET_PLATFORM_WIN32
char *strsignal(int sig)
{
    // On Windows platform, only SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM are supported.
    // Doc: https://learn.microsoft.com/en-us/previous-versions/xdkz3x12(v=vs.140)
    // NSIG (defined in signal.h): 23

    // Signal descs:
    static constexpr int linuxNSIG = 65;
    static const char *signalStrDescs[linuxNSIG] = {
        /* 0  */ "Unknown signal 0",
        /* 1  */ "Hangup (Not Supported on Windows)",                   // SIGHUP
        /* 2  */ "Interrupt",                                           // SIGINT
        /* 3  */ "Quit (Not Supported on Windows)",                     // SIGQUIT
        /* 4  */ "Illegal instruction",                                 // SIGILL
        /* 5  */ "Trace/breakpoint trap (Not Supported on Windows)",    // SIGTRAP
        /* 6  */ "Aborted",                                             // SIGABRT
        /* 7  */ "Bus error (Not Supported on Windows)",                // SIGBUS
        /* 8  */ "Floating point exception",                            // SIGFPE
        /* 9  */ "Killed (Not Supported on Windows)",                   // SIGKILL
        /* 10 */ "User defined signal 1 (Not Supported on Windows)",    // SIGUSR1
        /* 11 */ "Segmentation fault",                                  // SIGSEGV
        /* 12 */ "User defined signal 2 (Not Supported on Windows)",    // SIGUSR2
        /* 13 */ "Broken pipe (Not Supported on Windows)",              // SIGPIPE
        /* 14 */ "Alarm clock (Not Supported on Windows)",              // SIGALRM
        /* 15 */ "Terminated",                                          // SIGTERM
        /* 16 */ "Stack fault (Not Supported on Windows)",              // SIGSTKFLT
        /* 17 */ "Child exited (Not Supported on Windows)",             // SIGCHLD
        /* 18 */ "Continued (Not Supported on Windows)",                // SIGCONT
        /* 19 */ "Stopped (signal) (Not Supported on Windows)",         // SIGSTOP
        /* 20 */ "Stopped (Not Supported on Windows)",                  // SIGTSTP
        /* 21 */ "Stopped (tty input) (Not Supported on Windows)",      // SIGTTIN
        /* 22 */ "Stopped (tty output) (Not Supported on Windows)",     // SIGTTOU
        /* 23 */ "Urgent I/O condition (Not Supported on Windows)",     // SIGURG
        /* 24 */ "CPU time limit exceeded (Not Supported on Windows)",  // SIGCPU
        /* 25 */ "File size limit exceeded (Not Supported on Windows)", // SIGXFSZ
        /* 26 */ "Virtual timer expired (Not Supported on Windows)",    // SIGVTALRM
        /* 27 */ "Profiling timer expired (Not Supported on Windows)",  // SIGPROF
        /* 28 */ "Window changed (Not Supported on Windows)",           // SIGWINCH
        /* 29 */ "I/O possible (Not Supported on Windows)",             // SIGIO
        /* 30 */ "Power failure (Not Supported on Windows)",            // SIGPWR
        /* 31 */ "Bad system call (Not Supported on Windows)",          // SIGSYS
        /* 32 */ "Unknown signal 32",
        /* 33 */ "Unknown signal 33",
        /* 34 */ "Real-time signal 0 (Not Supported on Windows)",       // SIGRTMIN + 0
        /* 35 */ "Real-time signal 1 (Not Supported on Windows)",       // SIGRTMIN + 1
        /* 36 */ "Real-time signal 2 (Not Supported on Windows)",       // SIGRTMIN + 2
        /* 37 */ "Real-time signal 3 (Not Supported on Windows)",       // SIGRTMIN + 3
        /* 38 */ "Real-time signal 4 (Not Supported on Windows)",       // SIGRTMIN + 4
        /* 39 */ "Real-time signal 5 (Not Supported on Windows)",       // SIGRTMIN + 5
        /* 40 */ "Real-time signal 6 (Not Supported on Windows)",       // SIGRTMIN + 6
        /* 41 */ "Real-time signal 7 (Not Supported on Windows)",       // SIGRTMIN + 7
        /* 42 */ "Real-time signal 8 (Not Supported on Windows)",       // SIGRTMIN + 8
        /* 43 */ "Real-time signal 9 (Not Supported on Windows)",       // SIGRTMIN + 9
        /* 44 */ "Real-time signal 10 (Not Supported on Windows)",      // SIGRTMIN + 10
        /* 45 */ "Real-time signal 11 (Not Supported on Windows)",      // SIGRTMIN + 11
        /* 46 */ "Real-time signal 12 (Not Supported on Windows)",      // SIGRTMIN + 12
        /* 47 */ "Real-time signal 13 (Not Supported on Windows)",      // SIGRTMIN + 13
        /* 48 */ "Real-time signal 14 (Not Supported on Windows)",      // SIGRTMIN + 14
        /* 49 */ "Real-time signal 15 (Not Supported on Windows)",      // SIGRTMIN + 15
        /* 50 */ "Real-time signal 16 (Not Supported on Windows)",      // SIGRTMIN + 16
        /* 51 */ "Real-time signal 17 (Not Supported on Windows)",      // SIGRTMIN + 17
        /* 52 */ "Real-time signal 18 (Not Supported on Windows)",      // SIGRTMIN + 18
        /* 53 */ "Real-time signal 19 (Not Supported on Windows)",      // SIGRTMIN + 19
        /* 54 */ "Real-time signal 20 (Not Supported on Windows)",      // SIGRTMIN + 20
        /* 55 */ "Real-time signal 21 (Not Supported on Windows)",      // SIGRTMIN + 21
        /* 56 */ "Real-time signal 22 (Not Supported on Windows)",      // SIGRTMIN + 22
        /* 57 */ "Real-time signal 23 (Not Supported on Windows)",      // SIGRTMIN + 23
        /* 58 */ "Real-time signal 24 (Not Supported on Windows)",      // SIGRTMIN + 24
        /* 59 */ "Real-time signal 25 (Not Supported on Windows)",      // SIGRTMIN + 25
        /* 60 */ "Real-time signal 26 (Not Supported on Windows)",      // SIGRTMIN + 26
        /* 61 */ "Real-time signal 27 (Not Supported on Windows)",      // SIGRTMIN + 27
        /* 62 */ "Real-time signal 28 (Not Supported on Windows)",      // SIGRTMIN + 28
        /* 63 */ "Real-time signal 29 (Not Supported on Windows)",      // SIGRTMIN + 29
        /* 64 */ "Real-time signal 30 (Not Supported on Windows)",      // SIGRTMIN + 30
    };

    static thread_local char sigDesc[64];
    if (sig >= 0 && sig < linuxNSIG)
    {
        auto &constSigDesc = signalStrDescs[sig];
        const auto sigDescLen = strlen(constSigDesc);
        llbc_assert(UNLIKELY(sigDescLen < sizeof(sigDesc)) &&
            "llbc framework internal error: signal description length too long");

        memcpy(sigDesc, constSigDesc, sigDescLen + 1);
    }
    else
    {
        snprintf(sigDesc, sizeof(sigDesc), "Unknown signal %d", sig);
    }

    return sigDesc;
}
#endif // Win32

__LLBC_INTERNAL_NS_BEGIN

// Received signals.
// |<---   32 bit   --->|<--- 32 bit --->|
// | receiver thread id |  signal value  |
static volatile LLBC_NS sint64 __receivedSignals[NSIG] = {0};

// Received signals count.
static volatile LLBC_NS sint64 __receivedSignalCount = 0;

// Build received signal.
static LLBC_FORCE_INLINE LLBC_NS uint64 __LLBC_BuildReceivedSignal(int recvThreadId, int sigValue)
{
    return (static_cast<LLBC_NS uint64>(static_cast<LLBC_NS uint32>(recvThreadId)) << 32) |
        static_cast<LLBC_NS uint64>(static_cast<LLBC_NS uint32>(sigValue));
}

// Parse received signal.
static LLBC_FORCE_INLINE void __LLBC_ParseReceivedSignal(LLBC_NS uint64 receivedSig, int &recvThreadId, int &sigValue)
{
    recvThreadId = static_cast<int>(receivedSig >> 32);
    sigValue = static_cast<int>(receivedSig & 0xffffffff);
}

// Crash signals.
static constexpr int __crashSignals[] LLBC_CFG_OS_CRASH_SIGNALS;

// Check given signal is crash signal or not.
constexpr bool __LLBC_IsCrashSignal(int sig)
{
    constexpr size_t arrSize = sizeof(__crashSignals) / sizeof(__crashSignals[0]);
    for (size_t idx = 0; idx < arrSize; ++idx)
    {
        if (__crashSignals[idx] == sig)
            return true;
    }

    return false;
}

// NPTL reserved signals.
// - NPTL: Native POSIX Thread Library.
#if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
static constexpr int __nptlReservedSignals[] LLBC_CFG_OS_NPTL_RESERVED_SIGNALS;

// Check given signal is NPTL reserved signal or not.
constexpr bool __LLBC_IsNPTLReservedSignal(int sig)
{
    constexpr size_t arrSize = sizeof(__nptlReservedSignals) / sizeof(__nptlReservedSignals[0]);
    for (size_t idx = 0; idx < arrSize; ++idx)
    {
        if (__nptlReservedSignals[idx] == sig)
            return true;
    }

    return false;
}
#else // Non Linux&Android
constexpr bool __LLBC_IsNPTLReservedSignal(int sig)
{
    return false;
}
#endif // Linux || Android

// Signal handlers.
static volatile LLBC_NS sint64 __signalHandlers[NSIG] = {0};

// Signal dispatcher.
#if LLBC_TARGET_PLATFORM_WIN32
static void __LLBC_SignalDispatcher(int sig)
#else
static void __LLBC_SignalDispatcher(int sig, siginfo_t *sigInfo, void *sigCtx)
#endif
{
    // Not allow to process "crash" / "NPTL reserved" signal.
    if (UNLIKELY(__LLBC_IsCrashSignal(sig) ||
                 __LLBC_IsNPTLReservedSignal(sig)))
        return;

    // Get signal value(on Windows platform, the signal value is always 0).
    #if LLBC_TARGET_PLATFORM_WIN32
    const int sigVal = 0;
    #else
    const int sigVal = sigInfo->si_code == SI_QUEUE ? sigInfo->si_value.sival_int : 0;
    #endif

    // Save the received signal via CAS: only writes if the slot is empty (0),
    // ensuring an already-pending signal is never overwritten.
    const auto recvedSig = __LLBC_BuildReceivedSignal(LLBC_NS LLBC_GetCurrentThreadId(), sigVal);
    LLBC_ReturnIf(UNLIKELY(recvedSig == 0), void());
    const LLBC_NS sint64 prevRecvedSig =
        LLBC_NS LLBC_AtomicCompareAndExchange(&__receivedSignals[sig], static_cast<LLBC_NS sint64>(recvedSig), 0);

    // On Windows: signal() handler is one-shot, must re-register after each signal.
    // TODO: The current implementation is not rigorous and will be optimized later.
    #if LLBC_TARGET_PLATFORM_WIN32
    if (LLBC_NS LLBC_AtomicGet(&__signalHandlers[sig]) != 0)
        ::signal(sig, __LLBC_SignalDispatcher);
    #endif

    // Increase received signal count.
    if (prevRecvedSig == 0)
        LLBC_NS LLBC_AtomicFetchAndAdd(&__receivedSignalCount, 1);
}

/** 
 * Internal function: Set crash signal alternative stack.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_HIDDEN int __LLBC_SetCrashSignalAltStack()
{
    #if LLBC_TARGET_PLATFORM_NON_WIN32 && LLBC_CFG_OS_USE_ALT_STACK_FOR_CRASH_SIGNAL
    // Get lib tls.
    LLBC_NS __LLBC_LibTls *libTls = LLBC_NS __LLBC_GetLibTls();
    if (UNLIKELY(!libTls))
    {
        // Warn: libTls is nullptr, Setting last error has no effect.
        // LLBC_NS LLBC_SetLastError(LLBC_ERROR_UNKNOWN);
        return LLBC_FAILED;
    }

    // Calculate alternative signal stack size.
    size_t &sigStackSize = libTls->coreTls.crashSignalAltStackSize;
    constexpr std::pair<size_t, size_t> altSigStackSizeRange {16 * 1024, 512 * 1024};
    sigStackSize = std::clamp(static_cast<size_t>(LLBC_CFG_OS_ALT_CRASH_SIGNAL_STACK_SIZE),
                              altSigStackSizeRange.first,
                              altSigStackSizeRange.second);
    
    // - Allocate alternative signal stack and set it.
    void *&sigStack = libTls->coreTls.crashSignalAltStack;
    sigStack = LLBC_Malloc(void, sigStackSize);
    if (UNLIKELY(!sigStack))
    {
        LLBC_NS LLBC_SetLastError(LLBC_ERROR_CLIB);
        sigStackSize = 0;

        return LLBC_FAILED;
    }

    stack_t ss;
    memset(&ss, 0, sizeof(ss));
    ss.ss_sp = sigStack;
    ss.ss_size = sigStackSize;
    ss.ss_flags = 0;
    if (UNLIKELY(sigaltstack(&ss, nullptr) != 0))
    {
        LLBC_NS LLBC_SetLastError(LLBC_ERROR_CLIB);
        free(sigStack);
        sigStack = nullptr;
        sigStackSize = 0;

        return LLBC_FAILED;
    }

    return LLBC_OK;
    #else // Alt crash signal stack is not supported or disabled.
    LLBC_NS LLBC_SetLastError(LLBC_ERROR_NOT_SUPPORT);
    return LLBC_FAILED;
    #endif
}

/** 
 * Internal function: Clear crash signal alternative stack.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_HIDDEN int __LLBC_ClearCrashSignalAltStack()
{
    #if LLBC_TARGET_PLATFORM_NON_WIN32 && LLBC_CFG_OS_USE_ALT_STACK_FOR_CRASH_SIGNAL
    // Get lib tls.
    LLBC_NS __LLBC_LibTls *libTls = LLBC_NS __LLBC_GetLibTls();
    if (UNLIKELY(!libTls))
    {
        // Warn: libTls is nullptr, Set last error is no effect.
        // LLBC_NS LLBC_SetLastError(LLBC_ERROR_UNKNOWN);
        return LLBC_FAILED;
    }

    // sigStack is nullptr, no need to reset.
    void *&sigStack = libTls->coreTls.crashSignalAltStack;
    if (!sigStack)
        return LLBC_OK;

    // Reset alternative signal stack.
    stack_t ss;
    memset(&ss, 0, sizeof(ss));
    ss.ss_flags = SS_DISABLE;
    // Note: macOS(XNU) validates ss_size >= MINSIGSTKSZ even when SS_DISABLE is set and would
    // otherwise fail with ENOMEM, so pass the still-valid stack pointer/size along with SS_DISABLE.
    ss.ss_sp = sigStack;
    ss.ss_size = libTls->coreTls.crashSignalAltStackSize;
    if (UNLIKELY(sigaltstack(&ss, nullptr) != 0))
    {
        LLBC_NS LLBC_SetLastError(LLBC_ERROR_CLIB);
        fprintf(stderr,
                "[%d] Clear alternative signal stack failed, error:%s\n",
                LLBC_NS LLBC_GetCurrentThreadId(),
                LLBC_NS LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    // Free alternative signal stack.
    free(sigStack);
    sigStack = nullptr;
    libTls->coreTls.crashSignalAltStackSize = 0;

    return LLBC_OK;
    #else // Alt crash signal stack is not supported or disabled.
    LLBC_NS LLBC_SetLastError(LLBC_ERROR_NOT_SUPPORT);
    return LLBC_FAILED;
    #endif
}

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

int LLBC_SetSignalHandler(int sig, LLBC_SignalHandler handler)
{
    // Validate signal number.
    if (UNLIKELY(sig <= 0 || sig >= NSIG))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    // Not allow to set crash signal handler/NPTL reserved signal handler.
    if (UNLIKELY(LLBC_INL_NS __LLBC_IsCrashSignal(sig) ||
                 LLBC_INL_NS __LLBC_IsNPTLReservedSignal(sig)))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }

    // Build sigaction struct.
    #if LLBC_TARGET_PLATFORM_WIN32
    // WIN32 signal api doc: https://learn.microsoft.com/en-us/previous-versions/xdkz3x12(v=vs.140)
    ::signal(sig, handler ? LLBC_INL_NS __LLBC_SignalDispatcher : SIG_DFL);
    #else // Non-Win32
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    if (handler)
    {
        sa.sa_flags = SA_RESTART | SA_SIGINFO;
        sa.sa_sigaction = LLBC_INL_NS __LLBC_SignalDispatcher;
    }
    else
    {
        sa.sa_handler = SIG_DFL;
    }

    // Set signal action.
    const int ret = sigaction(sig, &sa, nullptr);
    if (UNLIKELY(ret != 0))
    {
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }
    #endif // Win32

    // Set signal handler.
    sint64 handlerAddr = 0;
    ::memcpy(&handlerAddr, &handler, std::min(sizeof(handlerAddr), sizeof(handler)));
    LLBC_AtomicSet(&LLBC_INL_NS __signalHandlers[sig], handlerAddr);

    return LLBC_OK;
}

LLBC_SignalHandler LLBC_GetSignalHandler(int sig)
{
    // Validate signal number.
    if (UNLIKELY(sig <= 0 || sig >= NSIG))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return nullptr;
    }

    // Not allow to get crash signal handler/NPTL reserved signal handler.
    if (UNLIKELY(LLBC_INL_NS __LLBC_IsCrashSignal(sig) ||
                 LLBC_INL_NS __LLBC_IsNPTLReservedSignal(sig)))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return nullptr;
    }

    // Reset last error.
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    // Get signal handler.
    LLBC_SignalHandler handler = nullptr;
    const sint64 handlerAddr = LLBC_AtomicGet(&LLBC_INL_NS __signalHandlers[sig]);
    ::memcpy(&handler, &handlerAddr, std::min(sizeof(handler), sizeof(handlerAddr)));

    return handler;
}

int LLBC_ProcessReceivedSignals()
{
    // If no received signals, return directly.
    sint64 recvedSigCount = LLBC_AtomicGet(&LLBC_INL_NS __receivedSignalCount);
    if (LIKELY(recvedSigCount <= 0))
        return LLBC_OK;

    // Get lib tls.
    auto libTls = __LLBC_GetLibTls();
    if (UNLIKELY(!libTls))
    {
        LLBC_SetLastError(LLBC_ERROR_UNKNOWN);
        return LLBC_FAILED;
    }

    // Not allowed to process received signals in non-entry thread.
    if (UNLIKELY(!libTls->coreTls.entryThread))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }

    for (int sig = 1; sig < NSIG; ++sig)
    {
        // If no received signals, break.
        // CONSTRAINT: <recvedSigCount> must be greater than or equal to 0.
        recvedSigCount = LLBC_AtomicGet(&LLBC_INL_NS __receivedSignalCount);
        llbc_assert(LIKELY(recvedSigCount >= 0) &&
                        "llbc framework internal error: received signal count must be greater than or equal to 0 [1]");
        LLBC_BreakIf(recvedSigCount == 0);

        // If no received this signal, continue.
        const auto recvedSig = static_cast<uint64>(LLBC_AtomicSet(&LLBC_INL_NS __receivedSignals[sig], 0ll));
        LLBC_ContinueIf(recvedSig == 0);

        // Decrease received signal count.
        // CONSTRAINT: Before decrement, <recvedSigCount> must be greater than 0.
        // NOTE: <recvedSigCount> will be updated at the start of the next loop iteration.
        const sint64 prevRecvedSigCount = LLBC_AtomicFetchAndSub(&LLBC_INL_NS __receivedSignalCount, 1);
        llbc_assert(LIKELY(prevRecvedSigCount > 0) &&
                        "llbc framework internal error: previous received signal count must be greater than 0 [2]");

        // Parse received signal.
        int sigValue = 0;
        int recvThreadId = 0;
        LLBC_INL_NS __LLBC_ParseReceivedSignal(recvedSig, recvThreadId, sigValue);

        // Invoke handler.
        LLBC_SignalHandler handler = nullptr;
        const auto handlerAddr = LLBC_AtomicGet(&LLBC_INL_NS __signalHandlers[sig]);
        ::memcpy(&handler, &handlerAddr, std::min(sizeof(handler), sizeof(handlerAddr)));
        if (handler)
            (*handler)(recvThreadId, sig, sigValue);

        // Reset sig to 0 after handling the current signal, so that the next
        // signal to be processed is always the highest-priority one among all
        // pending signals (lower signal number means higher priority).
        sig = 0; // !!! sig will increment to 1 in the next loop.
    }

    return LLBC_OK;
}

__LLBC_NS_END
