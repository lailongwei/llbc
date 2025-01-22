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


#include "llbc/common/Export.h"

#include "llbc/core/time/Time.h"
#include "llbc/core/os/OS_Process.h"
#if LLBC_SUPPORT_HANDLE_CRASH
#include "llbc/core/file/Directory.h"
#endif
#include "llbc/core/log/LoggerMgr.h"

#if LLBC_CUR_COMP == LLBC_COMP_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
#endif // GCC compiler

__LLBC_NS_BEGIN

int LLBC_GetCurrentProcessId()
{
#if LLBC_TARGET_PLATFORM_WIN32
    return static_cast<int>(::GetCurrentProcessId());
#else // Non-Win32
    return static_cast<int>(getpid());
#endif // Win32
}

__LLBC_NS_END

#if LLBC_SUPPORT_HANDLE_CRASH
__LLBC_INTERNAL_NS_BEGIN

static bool __hookedCrashSignals = false;
static char __stackBacktrace[128 * 1024 + 1] {'\0'};
static LLBC_NS LLBC_Delegate<void(const char *stackBacktrace, int sig)> __crashCallback = nullptr;

__LLBC_INTERNAL_NS_END

#if LLBC_TARGET_PLATFORM_WIN32
__LLBC_INTERNAL_NS_BEGIN

static char __dumpFilePath[MAX_PATH + 1];

static void __GetExceptionBackTrace(PCONTEXT ctx, char *stackBacktrace, size_t backtraceSize)
{
    // Dump backtrace head line.
    stackBacktrace[0] = '\0';
    auto fmtRet = snprintf(stackBacktrace, backtraceSize, "Stack BackTrace:\n");
    if (fmtRet < 0)
        return;
    stackBacktrace += fmtRet;
    backtraceSize -= fmtRet;

#if LLBC_TARGET_PROCESSOR_X86
    DWORD machineType = IMAGE_FILE_MACHINE_I386;
#elif LLBC_TARGET_PROCESSOR_X86_64
    DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
#else // Not supported dump processor types.
    return;
#endif

    STACKFRAME64 stackFrame64;
    memset(&stackFrame64, 0, sizeof(STACKFRAME64));
#if LLBC_TARGET_PROCESSOR_X86
    stackFrame64.AddrPC.Offset = ctx->Eip;
    stackFrame64.AddrPC.Mode = AddrModeFlat;
    stackFrame64.AddrStack.Offset = ctx->Esp;
    stackFrame64.AddrStack.Mode = AddrModeFlat;
    stackFrame64.AddrFrame.Offset = ctx->Ebp;
    stackFrame64.AddrFrame.Mode = AddrModeFlat;
#elif LLBC_TARGET_PROCESSOR_X86_64
    stackFrame64.AddrPC.Offset = ctx->Rip;
    stackFrame64.AddrPC.Mode = AddrModeFlat;
    stackFrame64.AddrStack.Offset = ctx->Rsp;
    stackFrame64.AddrStack.Mode = AddrModeFlat;
    stackFrame64.AddrFrame.Offset = ctx->Rbp;
    stackFrame64.AddrFrame.Mode = AddrModeFlat;
#endif

    int frameNo = 0;
    HANDLE curProc = ::GetCurrentProcess();
    HANDLE curThread = ::GetCurrentThread();
    LLBC_NS LLBC_Strings backTraces;
    while (true)
    {
        if (!::StackWalk64(machineType,
                           curProc,
                           curThread,
                           &stackFrame64,
                           ctx,
                           nullptr,
                           ::SymFunctionTableAccess64,
                           ::SymGetModuleBase64,
                           nullptr))
            break;

        if (stackFrame64.AddrFrame.Offset == 0)
            break;

        BYTE symbolBuffer[sizeof(SYMBOL_INFO) + 512];
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)(symbolBuffer);
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = 511;

        if (!::SymFromAddr(curProc, stackFrame64.AddrPC.Offset, nullptr, symbol))
            break;

        int lineNo = 0;
        const char *fileName = nullptr;
        DWORD symDisplacement = 0;
        IMAGEHLP_LINE64 lineInfo;
        memset(&lineInfo, 0, sizeof(IMAGEHLP_LINE64));
        lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        if (::SymGetLineFromAddr64(curProc, stackFrame64.AddrPC.Offset, &symDisplacement, &lineInfo))
        {
            fileName = lineInfo.FileName;
            lineNo = lineInfo.LineNumber;
        }

        const auto usedCap = snprintf(stackBacktrace,
                                      backtraceSize,
                                      "#%d 0x%p in %s at %s:%d\n",
                                      frameNo++,
                                      reinterpret_cast<void *>(symbol->Address),
                                      symbol->Name,
                                      fileName ? fileName : "",
                                      lineNo);
        if (usedCap <= 0 ||
            usedCap >= static_cast<int>(backtraceSize) - 1)
            return;

        backtraceSize -= usedCap;
        stackBacktrace += usedCap;
    }
}

static LONG WINAPI __Win32CrashHandler(::EXCEPTION_POINTERS *exception)
{
    HANDLE dmpFile = ::CreateFileA(__dumpFilePath,
                                   GENERIC_WRITE,
                                   FILE_SHARE_READ,
                                   nullptr,
                                   CREATE_ALWAYS,
                                   FILE_ATTRIBUTE_NORMAL,
                                   nullptr);
    if (UNLIKELY(dmpFile == INVALID_HANDLE_VALUE))
        return EXCEPTION_CONTINUE_SEARCH;

    LLBC_NS LLBC_String errMsg;
    errMsg.append("Unhandled exception!\n");
    errMsg.append_format("Mini dump file path:%s\n", __dumpFilePath);

    ::MINIDUMP_EXCEPTION_INFORMATION dmpInfo;
    dmpInfo.ExceptionPointers = exception;
    dmpInfo.ThreadId = GetCurrentThreadId();
    dmpInfo.ClientPointers = TRUE;

    const BOOL writeDumpSucc = ::MiniDumpWriteDump(::GetCurrentProcess(),
                                                   ::GetCurrentProcessId(),
                                                   dmpFile,
                                                   (MINIDUMP_TYPE)LLBC_CFG_APP_DUMPFILE_DUMPTYPES,
                                                   &dmpInfo,
                                                   nullptr,
                                                   nullptr);
    if (UNLIKELY(!writeDumpSucc))
    {
        LLBC_NS LLBC_SetLastError(LLBC_ERROR_OSAPI);
        errMsg.append_format("Write dump failed, error:%s\n", LLBC_NS LLBC_FormatLastError());
    }

    ::CloseHandle(dmpFile);
    __GetExceptionBackTrace(exception->ContextRecord, __stackBacktrace, sizeof(__stackBacktrace));
    errMsg.append_format("%s\n", __stackBacktrace);

    LLBC_NS LLBC_String mbTitle;
    mbTitle.format("Unhandled Exception(%s)", LLBC_NS LLBC_Directory::ModuleFileName().c_str());
    ::MessageBoxA(nullptr, errMsg.c_str(), mbTitle.c_str(), MB_ICONERROR | MB_OK);

    if (__crashCallback)
        __crashCallback(__stackBacktrace, 0);

    LLBC_LoggerMgrSingleton->Finalize();

    return EXCEPTION_EXECUTE_HANDLER;
}

static BOOL __PreventSetUnhandledExceptionFilter()
{
    HMODULE kernel32 = ::LoadLibraryA("kernel32.dll");
    if (kernel32 == nullptr)
        return FALSE;

    void *orgEntry = (void *)::GetProcAddress(kernel32, "SetUnhandledExceptionFilter");
    if (orgEntry == nullptr)
        return FALSE;

#ifdef _M_IX86
    // Code for x86:
    // 33 C0    xor eax, eax
    // C2 04 00 ret 4
    unsigned char execute[] = { 0x33, 0xc0, 0xc2, 0x04, 0x00 };
#elif _M_X64
    // Code for x64
    // 33 c0    xor eax, eax
    // c3       ret
    unsigned char execute[] = { 0x33, 0xc0, 0xc3 };
#else
 #error "Unsupported architecture(on windows platform)!"
#endif

    SIZE_T bytesWritten = 0;
    return ::WriteProcessMemory(GetCurrentProcess(),
                                orgEntry,
                                execute,
                                sizeof(execute),
                                &bytesWritten);
}

__LLBC_INTERNAL_NS_END
#else // Linux or Macosx
#include <fcntl.h>
#include <libgen.h>
#include <signal.h>
#include <execinfo.h>

#include "llbc/core/file/File.h"

__LLBC_INTERNAL_NS_BEGIN

static char __exeFilePath[PATH_MAX + 1];
static char __corePattern[PATH_MAX + 1];
static char __coreDescFilePath[PATH_MAX + 1];
static char __shellCmd[PATH_MAX * 2 + 256 + 1];
static void *__frames[LLBC_CFG_OS_SYMBOL_MAX_CAPTURE_FRAMES] {nullptr};
static int __catchSignals[] LLBC_CFG_OS_CRASH_SIGNALS;

static volatile bool __handlingCrashSignals = false;
static const char *__corePatternPath = "/proc/sys/kernel/core_pattern";

static void __NonWin32CrashHandler(int sig)
{
    // Uninstall this signal's hook.
    signal(sig, SIG_DFL);

    // If handling another crash signal, raise the signal again, otherwise mask hanlding signal.
    if (__handlingCrashSignals)
    {
        raise(sig);
        return;
    }
    else
    {
        __handlingCrashSignals = true;
    }

    // Get executable file path.
    ssize_t readLinkRet = readlink("/proc/self/exe", __exeFilePath, PATH_MAX);
    LLBC_DoIf(readLinkRet == -1, raise(sig));

    __exeFilePath[readLinkRet] = '\0';
    const char *exeFileName = basename(__exeFilePath);
    LLBC_DoIf(!exeFileName, raise(sig));

    // Get core pattern.
    auto corePatternFd = open(__corePatternPath, O_RDONLY);
    LLBC_DoIf(corePatternFd == -1, raise(sig));

    ssize_t readRet = read(corePatternFd, __corePattern, sizeof(__corePattern) - 1);
    LLBC_DoIf(readRet == -1, close(corePatternFd); raise(sig));
   
    close(corePatternFd);
    __corePattern[readRet >= 0 ? readRet : 0] = '\0';

    // Copy executable to core pattern directory(ignore error).
    const char *corePatternDir = nullptr;
    if (readRet > 0 && __corePattern[0] != '|')
        corePatternDir = dirname(__corePattern);
    if (!corePatternDir)
        corePatternDir = ".";

    const int pid = getpid();
    const LLBC_NS uint32 now = time(nullptr);
    int fmtRet = snprintf(__shellCmd,
                          sizeof(__shellCmd),
                          "\\cp -rf \"%s\" \"%s/%s_%d_%u\"",
                          __exeFilePath,
                          corePatternDir,
                          exeFileName,
                          pid,
                          now);
    LLBC_DoIf(fmtRet < 0, raise(sig));

    system(__shellCmd);

    // Generate stack backtrace describe file(to txt).
    fmtRet = snprintf(__coreDescFilePath,
                      sizeof(__coreDescFilePath),
                      "%s/%s_%d_%u_stack_backtrace.txt",
                      corePatternDir,
                      exeFileName,
                      pid,
                      now);
    LLBC_DoIf(fmtRet < 0, raise(sig));

    int coreDescFileFd = open(__coreDescFilePath, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    LLBC_DoIf(coreDescFileFd == -1, raise(sig));

    char descFileHead[64];
    fmtRet = snprintf(descFileHead,
                      sizeof(descFileHead),
                      "Stack BackTrace(signal:%d):\n",
                      sig);
    if (fmtRet > 0)
    {
        write(coreDescFileFd, descFileHead, fmtRet);
        fsync(coreDescFileFd);
    }

    const auto framesCnt = backtrace(__frames, LLBC_CFG_OS_SYMBOL_MAX_CAPTURE_FRAMES);
    backtrace_symbols_fd(__frames, framesCnt, coreDescFileFd);
    fsync(coreDescFileFd);

    // Call callback delegate.
    if (__crashCallback)
    {
        if (lseek(coreDescFileFd, 0, SEEK_SET) == -1)
        {
            close(coreDescFileFd);
            raise(sig);
        }

        auto readRet = read(coreDescFileFd, __stackBacktrace, sizeof(__stackBacktrace) - 1);
        if (readRet >= 0)
            __stackBacktrace[readRet] = '\0';

        close(coreDescFileFd);

        __crashCallback(__stackBacktrace, sig);
    }
    else
    {
        close(coreDescFileFd);
    }

    // Finalize logger manager.
    LLBC_LoggerMgrSingleton->Finalize();

    // Reraise signal.
    raise(sig);
}

__LLBC_INTERNAL_NS_END
#endif // Linux or Macosx

__LLBC_NS_BEGIN

int LLBC_HandleCrash(const LLBC_String &dumpFilePath,
                     const LLBC_Delegate<void(const char *stackBacktrace,
                                              int sig)> &crashCallback)
{
#if LLBC_TARGET_PLATFORM_WIN32
    LLBC_String nmlDumpFilePath = dumpFilePath;
    if (nmlDumpFilePath.empty())
    {
        const auto now = LLBC_Time::Now();
        nmlDumpFilePath = LLBC_Directory::SplitExt(LLBC_Directory::ModuleFilePath())[0];
        nmlDumpFilePath.append_format("_%d_%s.dmp",
                                      LLBC_GetCurrentProcessId(),
                                      now.Format("%Y%m%d_%H%M%S").c_str());
    }

    if (nmlDumpFilePath.size() >= sizeof(LLBC_INL_NS __dumpFilePath))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    memcpy(LLBC_INL_NS __dumpFilePath, nmlDumpFilePath.c_str(), nmlDumpFilePath.size());
    LLBC_INL_NS __dumpFilePath[nmlDumpFilePath.size()] = '\0';

    if (!LLBC_INL_NS __hookedCrashSignals)
    {
        ::SetUnhandledExceptionFilter(LLBC_INL_NS __Win32CrashHandler);
        #ifdef LLBC_RELEASE
        LLBC_INL_NS __PreventSetUnhandledExceptionFilter();
        #endif

        LLBC_INL_NS __hookedCrashSignals = true;
    }

    // Set crash callback.
    LLBC_INL_NS __crashCallback = crashCallback;

    return LLBC_OK;
#elif LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_MAC
    // Use system default core pattern if dumpFilePath is empty.
    // if (dumpFilePath.empty())
    //    return LLBC_OK;

    if (!dumpFilePath.empty())
    {
        // Save old core pattern.
        const auto oldCorePattern = LLBC_File::ReadToEnd(LLBC_INL_NS __corePatternPath);
        if (LLBC_GetLastError() != LLBC_ERROR_SUCCESS)
            return LLBC_FAILED;

        // Write new core pattern(may not have permission to open core_pattern file, ignore error).
        LLBC_File corePatternFile;
        if (corePatternFile.Open(LLBC_INL_NS __corePatternPath, LLBC_FileMode::Write) == LLBC_OK)
        {
            // If failed, try write old core pattern.
            if (corePatternFile.Write(dumpFilePath) != LLBC_OK)
            {
                corePatternFile.Seek(LLBC_FileSeekOrigin::Begin, 0);
                corePatternFile.Write(oldCorePattern);

                return LLBC_FAILED;
            }
        }
    }

    // Set signals handler.
    if (!LLBC_INL_NS __hookedCrashSignals)
    {
        // Set signals handler.
        sigset_t ss;
        sigemptyset(&ss);
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = LLBC_INL_NS __NonWin32CrashHandler;
        for (auto sig : LLBC_INL_NS __catchSignals)
        {
            sigaddset(&ss, sig);
            sigaction(sig, &sa, nullptr);
        }

        // Make signals unblock.
        sigprocmask(SIG_UNBLOCK, &ss, nullptr);

        LLBC_INL_NS __hookedCrashSignals = true;
    }

    // Set crash callback.
    LLBC_INL_NS __crashCallback = crashCallback;

    return LLBC_OK;
#else // Unsupported platforms
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
#endif // Win32
}

__LLBC_NS_END
#endif // Supp hook process crash

#if LLBC_SUPPORT_SET_EXCLUSIVE
#include <stdio.h>
#if LLBC_TARGET_PLATFORM_WIN32
#include <psapi.h>
#endif
#if LLBC_TARGET_PLATFORM_MAC
#include <libproc.h>
#endif
__LLBC_NS_BEGIN
int LLBC_SetExclusive(const LLBC_String &pidFilePath)
{
#if LLBC_TARGET_PLATFORM_WIN32
    static char pidBuf[MAX_FORM_KEYWORD_LENGTH + 1] = { 0 };
    static char runningExeFilePath[MAX_PATH + 1] = { 0 };

    // Get current execute file path
    LLBC_String selfExeFilePath = LLBC_Directory::ModuleFilePath();

    // Set .pid file path
    LLBC_String pidFile = LLBC_Directory::Join(pidFilePath, LLBC_Directory::ModuleFileName());
    if (pidFilePath == "")
    {
        pidFile = selfExeFilePath;
    }
    pidFile.append(".pid");

    // Create or read-lock .pid file
    HANDLE file = CreateFileA(pidFile.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
    LLBC_ReturnIf(file == INVALID_HANDLE_VALUE, LLBC_FAILED);
    LLBC_Defer(CloseHandle(file));

    // Read pid from .pid file
    DWORD fileSize = GetFileSize(file, NULL);
    bool readSucc = ReadFile(file, pidBuf, fileSize, &fileSize, NULL);
    LLBC_ReturnIf(!readSucc, LLBC_FAILED);
    pidBuf[fileSize] = '\0';

    HANDLE processHandle = NULL;
    // Open executable file by pid.
    processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, atoi(pidBuf));
    if (processHandle != NULL)
    {
        // Get executable file path refer to pid.
        DWORD getModuleFileNameRet = GetModuleFileNameExA(processHandle, NULL, runningExeFilePath, MAX_PATH);
        CloseHandle(processHandle);
        LLBC_ReturnIf(getModuleFileNameRet == 0, LLBC_FAILED);
        runningExeFilePath[getModuleFileNameRet] = '\0';
    }

    // Compare self name and running process path
    LLBC_ReturnIf(runningExeFilePath == selfExeFilePath, LLBC_FAILED);

    // Clear .pid file and write pid into it
    SetFilePointer(file, 0, NULL, FILE_BEGIN);
    SetEndOfFile(file);
    int printRet = snprintf(pidBuf, MAX_FORM_KEYWORD_LENGTH, "%ld", GetCurrentProcessId());
    pidBuf[printRet] = '\0';
    LPDWORD writeSize = 0;
    WriteFile(file, pidBuf, strlen(pidBuf), writeSize, NULL);
#else // linux and macos
    static char pidBuf[MAX_INPUT];
    static char runningExeFilePath[PATH_MAX + 1];

    // Get current execute file path
    LLBC_String selfExeFilePath = LLBC_Directory::ModuleFilePath();

    // Set .pid file path
    LLBC_String pidFile = LLBC_Directory::Join(pidFilePath, LLBC_Directory::ModuleFileName());
    if (pidFilePath == "")
    {
        pidFile = selfExeFilePath;
    }
    pidFile.append(".pid");

    // Open .pid file and read pid.
    int fd = open(pidFile.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR); // 可读可写，不存在时创建
    LLBC_ReturnIf(fd < 0, LLBC_FAILED);
    LLBC_Defer(close(fd););

    int readRet = read(fd, pidBuf, MAX_INPUT);
    LLBC_ReturnIf(readRet == -1, LLBC_FAILED);
    pidBuf[readRet] = '\0';

    // Open executable file by pid.
#if LLBC_TARGET_PLATFORM_MAC
    int pidPathRet = proc_pidpath(atoi(pidBuf), runningExeFilePath, PATH_MAX);
    LLBC_DoIf(pidPathRet != -1, runningExeFilePath[pidPathRet] = '\0');
#else
    llbc::LLBC_String runingExe;
    runingExe.format("/proc/%s/exe", pidBuf);
    ssize_t readLinkRet = readlink(runingExe.c_str(), runningExeFilePath, PATH_MAX);
    LLBC_DoIf(readLinkRet != -1, runningExeFilePath[readLinkRet] = '\0');
#endif
    LLBC_ReturnIf(strcmp(selfExeFilePath.c_str(), runningExeFilePath) == 0, LLBC_FAILED);

    // Try to lock pid file
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    fl.l_pid = -1;
    LLBC_ReturnIf(fcntl(fd, F_SETLK, &fl) == -1, LLBC_FAILED);

    // Clear .pid file and write pid into it
    ftruncate(fd, 0);
    lseek(fd, 0, SEEK_SET);
    snprintf(pidBuf, MAX_INPUT, "%ld", (long)getpid());
    write(fd, pidBuf, strlen(pidBuf) + 1);
#endif
    return LLBC_OK;
}
__LLBC_NS_END
#endif // Supp set process exclusive

#if LLBC_CUR_COMP == LLBC_COMP_GCC
#pragma GCC diagnostic pop
#endif // GCC compiler
