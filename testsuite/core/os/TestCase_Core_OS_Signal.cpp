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

#include <signal.h>
#include "core/os/TestCase_Core_OS_Signal.h"

namespace
{

static volatile int loopTimes = 20;
static volatile int sleepMsTimePerLoop = 500;
static volatile bool printStackBacktrace = false;

static volatile int triggeredTimes = 0;

static void _SetupSigHandlerBehavior(int lTimes, int sleepMs, bool printBacktrace)
{
    loopTimes = lTimes;
    sleepMsTimePerLoop = sleepMs;
    printStackBacktrace = printBacktrace;

    triggeredTimes = 0;
}

static void _TestSigHandler(int recvThreadId, int sig, int sigVal)
{
    auto threadTls = LLBC_NS __LLBC_GetLibTls();
    LLBC_Expect(threadTls && threadTls->coreTls.entryThread, ">>>>>> Signal Handler should be in llbc Entry-Thread <<<<<<");

    LLBC_String logPrefix;
    logPrefix.format("[threadId:%d, recvThreadId:%d][signal:%d(%s)]", LLBC_GetCurrentThreadId(), recvThreadId, sig, strsignal(sig));
    if (printStackBacktrace)
        LLBC_PrintLn("Stack backtrace:\n%s", LLBC_CaptureStackBackTrace().c_str());

    LLBC_PrintLn("%s SigHandler called, sigVal:%d", logPrefix.c_str(), sigVal);
    for (int i = 0; i < loopTimes; ++i)
    {
        LLBC_PrintLn(" %s Handling Signal: %d/%d...", logPrefix.c_str(), i + 1, loopTimes);
        LLBC_Sleep(sleepMsTimePerLoop);
    }

    LLBC_AtomicFetchAndAdd(&triggeredTimes, 1);
    LLBC_PrintLn("%s SigHandler finished, triggeredTimes:%d", logPrefix.c_str(), triggeredTimes);
}

static void _DummySigHandler(int recvThreadId, int sig, int sigVal)
{
}

class ForUsedTestTasks : public LLBC_Task
{
public:
    ForUsedTestTasks(int sleepTimes, int sleepMsTimePerTime)
    : _sleepTimes(sleepTimes)
    , _sleepMsTimePerTime(sleepMsTimePerTime)
    {
    }

public:
    void Svc() override
    {
        LLBC_PrintLn(" [%d] Test thread started", LLBC_GetCurrentThreadId());
        for (int i = 0; i < _sleepTimes; ++i)
        {
            // LLBC_PrintLn(" [%d][%d/%d] Test thread running", LLBC_GetCurrentThreadId(), i + 1, _sleepTimes);
            // LLBC_PrintLn(" [%d] Sleep %d ms", LLBC_GetCurrentThreadId(), _sleepMsTimePerTime);
            LLBC_Sleep(_sleepMsTimePerTime);
        }

        LLBC_PrintLn("[%d] Test thread finished", LLBC_GetCurrentThreadId());
    }

    void Cleanup() override
    {
    }

private:
    int _sleepTimes;
    int _sleepMsTimePerTime;
};

}

int TestCase_Core_OS_Signal::Run(int argc, char *argv[])
{
    LLBC_PrintLn("core/os/signal test:\n");

    LLBC_Expect(SetSignalHandlerTest() == LLBC_OK);
    LLBC_Expect(SignalTriggerTest() == LLBC_OK);

    return LLBC_OK;
}

int TestCase_Core_OS_Signal::SetSignalHandlerTest()
{
    LLBC_PrintLn("SetSignalHandler test:");

    // Setup sig handler behavior.
    _SetupSigHandlerBehavior(10, 500, false);

    // Define test signals(standard signals + real-time signals).
    #if LLBC_TARGET_PLATFORM_WIN32
    const auto testSignals = {SIGSEGV, SIGFPE};
    #elif LLBC_TARGET_PLATFORM_MAC || LLBC_TARGET_PLATFORM_IPHONE
    const auto testSignals = {SIGUSR1, SIGUSR2};
    #else // Non-Win32 && Non-Mac && Non-iPhone
    const auto testSignals = {SIGUSR1, SIGUSR2, SIGRTMIN, SIGRTMIN + 1, SIGRTMIN + 2, SIGRTMAX - 1, SIGRTMAX};
    #endif

    // Test get signal handler.
    for (auto &sig : testSignals)
    {
        LLBC_Expect(!LLBC_GetSignalHandler(sig) && LLBC_GetLastError() == LLBC_ERROR_SUCCESS,
                    "  - Before set signal handler, signal[%d(%s)] handler should be nullptr",
                    sig,
                    strsignal(sig));
    }

    // Test set signal handler.
    LLBC_PrintLn("%s", "");
    for (auto &sig : testSignals)
    {
        LLBC_Expect(LLBC_SetSignalHandler(sig, &_TestSigHandler) == LLBC_OK,
                    "  - Set signal handler, signal:%d(%s)",
                    sig,
                    strsignal(sig));
    }

    // Test get non testSignals's handler.
    LLBC_PrintLn("%s", "");
    for (int sig = 1; sig < NSIG; ++sig)
    {
        if (std::find(std::begin(testSignals), std::end(testSignals), sig) != std::end(testSignals))
            continue;

        LLBC_Expect(!LLBC_GetSignalHandler(sig) &&
                        (LLBC_GetLastError() == LLBC_ERROR_SUCCESS || LLBC_GetLastError() == LLBC_ERROR_NOT_ALLOW),
                    "  - Get non testSignals's handler, signal:%d(%s)",
                    sig,
                    strsignal(sig));
    }

    // Test get signal handler.
    LLBC_PrintLn("%s", "");
    for (auto &sig : testSignals)
    {
        LLBC_Expect(LLBC_GetSignalHandler(sig) == &_TestSigHandler,
                    "  - After set signal handler, signal[%d(%s)] handler should be _TestSigHandler",
                    sig,
                    strsignal(sig));
    }

    // Test set signal handler again.
    LLBC_PrintLn("%s", "");
    for (auto &sig : testSignals)
    {
        LLBC_Expect(LLBC_SetSignalHandler(sig, &_DummySigHandler) == LLBC_OK,
                    "  - Set signal handler again, signal:%d(%s)",
                    sig,
                    strsignal(sig));

        LLBC_Expect(LLBC_GetSignalHandler(sig) == &_DummySigHandler,
                    "  - After set signal handler again, signal[%d(%s)] handler should be _DummySigHandler",
                    sig,
                    strsignal(sig));
    }

    // Test reset signal handler.
    LLBC_PrintLn("%s", "");
    for (auto &sig : testSignals)
    {
        LLBC_Expect(LLBC_SetSignalHandler(sig, nullptr) == LLBC_OK,
                    "  - Reset signal handler, signal:%d(%s)",
                    sig,
                    strsignal(sig));
    }

    // Test get signal handler(after reset).
    LLBC_PrintLn("%s", "");
    for (auto &sig : testSignals)
    {
        LLBC_Expect(!LLBC_GetSignalHandler(sig) && LLBC_GetLastError() == LLBC_ERROR_SUCCESS,
                    "  - After reset signal handler, signal[%d(%s)] handler should be nullptr",
                    sig,
                    strsignal(sig));
    }

    // Test set invalid signal handler.
    LLBC_PrintLn("%s", "");
    const int invalidSigs[] = {-1, 0, NSIG, NSIG + 1};
    for (auto &invalidSig : invalidSigs)
    {
        LLBC_Expect(LLBC_SetSignalHandler(invalidSig, _TestSigHandler) == LLBC_FAILED &&
                        LLBC_GetLastError() == LLBC_ERROR_INVALID,
                    "  - Set invalid signal handler should failed, signal:%d(%s)",
                    invalidSig,
                    strsignal(invalidSig));
    }
    // Test get invalid signal's handler.
    LLBC_PrintLn("%s", "");
    for (auto &invalidSig : invalidSigs)
    {
        LLBC_Expect(!LLBC_GetSignalHandler(invalidSig) &&
                        LLBC_GetLastError() == LLBC_ERROR_INVALID,
                    "  - Get invalid signal's handler should failed, signal:%d(%s)",
                    invalidSig,
                    strsignal(invalidSig));
    }

    // Test set crash signals handler.
    LLBC_PrintLn("%s", "");
    const int crashSigs[] = LLBC_CFG_OS_CRASH_SIGNALS;
    for (auto &crashSig : crashSigs)
    {
        LLBC_Expect(LLBC_SetSignalHandler(crashSig, _TestSigHandler) == LLBC_FAILED &&
                        LLBC_GetLastError() == LLBC_ERROR_NOT_ALLOW,
                    "  - Set crash signal handler should failed, signal:%d(%s)",
                    crashSig,
                    strsignal(crashSig));
    }
    // Test get crash signals's handler.
    LLBC_PrintLn("%s", "");
    for (auto &crashSig : crashSigs)
    {
        LLBC_Expect(LLBC_GetSignalHandler(crashSig) == nullptr &&
                        LLBC_GetLastError() == LLBC_ERROR_NOT_ALLOW,
                    "  - Get crash signal's handler should failed, signal:%d(%s)",
                    crashSig,
                    strsignal(crashSig));
    }

    // Test set NPTL reserved signals handler.
    #if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_PrintLn("%s", "");
    static constexpr int nptlReservedSigs[] = LLBC_CFG_OS_NPTL_RESERVED_SIGNALS;
    for (auto &nptlReservedSig : nptlReservedSigs)
    {
        LLBC_Expect(LLBC_SetSignalHandler(nptlReservedSig, _TestSigHandler) == LLBC_FAILED &&
                        LLBC_GetLastError() == LLBC_ERROR_NOT_ALLOW,
                    "  - Set NPTL reserved signal handler should failed, signal:%d(%s)",
                    nptlReservedSig,
                strsignal(nptlReservedSig));
    }
    // Test get NPTL reserved signal's handler.
    LLBC_PrintLn("%s", "");
    for (auto &nptlSig : nptlReservedSigs)
    {
        LLBC_Expect(LLBC_GetSignalHandler(nptlSig) == nullptr &&
                        LLBC_GetLastError() == LLBC_ERROR_NOT_ALLOW,
                    "  - Get NPTL reserved signal's handler should failed, signal:%d(%s)",
                    nptlSig,
                    strsignal(nptlSig));
    }
    #endif

    // Finalize: Reset signal handlers.
    LLBC_PrintLn("\nFinally, reset signal handlers");
    for (auto &sig : testSignals)
        LLBC_Expect(LLBC_SetSignalHandler(sig, nullptr) == LLBC_OK);

    return LLBC_OK;
}

int TestCase_Core_OS_Signal::SignalTriggerTest()
{
    LLBC_PrintLn("\nSignalTrigger test:");

    // Setup signal handler behavior.
    _SetupSigHandlerBehavior(5, 300, true);

    // Create Multi-Thread test environment.
    ForUsedTestTasks task(100000, 2);
    LLBC_Expect(task.Activate(10) == LLBC_OK);
    LLBC_Sleep(100);

    // Install signal handler.
    #if LLBC_TARGET_PLATFORM_WIN32
    const int testSignals[] = {SIGSEGV, SIGFPE};
    #elif LLBC_TARGET_PLATFORM_MAC || LLBC_TARGET_PLATFORM_IPHONE
    const auto testSignals = {SIGUSR1, SIGUSR2};
    #else // Non-Win32 && Non-Mac && Non-iPhone
    const int testSignals[] = {SIGUSR1, SIGUSR2, SIGRTMIN, SIGRTMIN + 1, SIGRTMIN + 2, SIGRTMAX - 1, SIGRTMAX};
    #endif
    LLBC_PrintLn("- Install test signal handlers, signals:%s", LLBC_Variant(testSignals).ToString().c_str());
    for (auto &sig : testSignals)
    {
        LLBC_Expect(LLBC_SetSignalHandler(sig, _TestSigHandler) == LLBC_OK);
        LLBC_PrintLn("  - Installed signal %d(%s) handler...", sig, strsignal(sig));
    }

    // Trigger signals.
    const int maxTriggeredTimes = 20;
    LLBC_PrintLn("- All signals handler installed, begin trigger signals(max triggered times:%d)", maxTriggeredTimes);
    while (triggeredTimes < 20)
    {
        LLBC_ProcessReceivedSignals();
        LLBC_Sleep(10);
    }

    // Remove signal handlers.
    LLBC_PrintLn("- Signal trigger finished, remove signal handlers");
    for (auto &sig : testSignals)
        LLBC_Expect(LLBC_SetSignalHandler(sig, nullptr) == LLBC_OK);

    return LLBC_OK;
}