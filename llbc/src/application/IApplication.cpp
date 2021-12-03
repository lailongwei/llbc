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
#include "llbc/common/BeforeIncl.h"

#include "llbc.h" //! Include llbc header to use Startup/Cleanup function.
#include "llbc/application/IApplication.h"

#if LLBC_TARGET_PLATFORM_WIN32

__LLBC_INTERNAL_NS_BEGIN

static const char *__dumpFileName = nullptr;
static LLBC_NS LLBC_Delegate<void(const LLBC_NS LLBC_String &)> __crashHook = nullptr;

static void __GetExceptionBackTrace(PCONTEXT ctx, LLBC_NS LLBC_String &backTrace)
{
#if LLBC_TARGET_PROCESSOR_X86
    DWORD machineType = IMAGE_FILE_MACHINE_I386;
#elif LLBC_TARGET_PROCESSOR_X86_64
    DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
#else // Not supported dump processor types.
    return;
#endif

    STACKFRAME64 stackFrame64;
    ::memset(&stackFrame64, 0, sizeof(STACKFRAME64));
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

        DWORD symDisplacement = 0;
        IMAGEHLP_LINE64 lineInfo = { sizeof(IMAGEHLP_LINE64) };
        if (::SymGetLineFromAddr64(curProc, stackFrame64.AddrPC.Offset, &symDisplacement, &lineInfo))
        {
            backTraces.push_back(LLBC_NS LLBC_String().format("0x%x in %s at %s:%d",
                (void *)symbol->Address, symbol->Name, lineInfo.FileName, lineInfo.LineNumber));
        }
        else
        {
            backTraces.push_back(LLBC_NS LLBC_String().format("0x%x in %s at %s:%d",
                (void *)symbol->Address, symbol->Name, "", 0));
        }
    }

    for (size_t i = 0; i < backTraces.size(); ++i)
        backTrace.append_format("#%d %s\n", backTraces.size() - i - 1, backTraces[i].c_str());
}

static LONG WINAPI __AppCrashHandler(::EXCEPTION_POINTERS *exception)
{
    HANDLE dmpFile = ::CreateFileA(__dumpFileName,
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
    errMsg.append_format("Mini dump file path:%s\n", __dumpFileName);

    ::MINIDUMP_EXCEPTION_INFORMATION dmpInfo;
    dmpInfo.ExceptionPointers = exception;
    dmpInfo.ThreadId = GetCurrentThreadId();
    dmpInfo.ClientPointers = TRUE;

    const ::BOOL writeDumpSucc = MiniDumpWriteDump(::GetCurrentProcess(),
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
    if (__crashHook)
        __crashHook(__dumpFileName);

    LLBC_NS LLBC_String backTrace;
    __GetExceptionBackTrace(exception->ContextRecord, backTrace);
    errMsg.append_format("Stack BackTrace:\n%s\n", backTrace.c_str());

    LLBC_NS LLBC_String mbTitle;
    mbTitle.format("Unhandled Exception(%s)", LLBC_NS LLBC_Directory::BaseName(LLBC_NS LLBC_Directory::ModuleFileName()).c_str());
    ::MessageBoxA(nullptr, errMsg.c_str(), mbTitle.c_str(), MB_ICONERROR | MB_OK);

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

#endif // Win32

__LLBC_NS_BEGIN

LLBC_IApplication *LLBC_IApplication::_thisApp = nullptr;

LLBC_IApplication::LLBC_IApplication()
: _name()

, _iniConfig()
, _propertyConfig()
, _loadingIniCfg(false)
, _loadingPropertyCfg(false)

, _services(*LLBC_ServiceMgrSingleton)

, _started(false)
, _waited(false)

#if LLBC_TARGET_PLATFORM_WIN32
, _crashHook(nullptr)
#endif // Win32
{
    if (_thisApp == nullptr)
        _thisApp = this;
}

LLBC_IApplication::~LLBC_IApplication()
{
    Wait();
    Stop();
}

    int LLBC_IApplication::Start(const LLBC_String &name, int argc, char *argv[])
{
    // Multi application check.
    if (_thisApp != this)
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    // Application name check.
    if (name.empty())
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    // Reentry check.
    if (_started)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    // Parse startup arguments.
    if (_startArgs.Parse(argc, argv) != LLBC_OK)
        return LLBC_FAILED;

    // Startup llbc library.
    if (LLBC_Startup() != LLBC_OK)
    {
        if (LLBC_Errno != LLBC_ERROR_REENTRY)
            return LLBC_FAILED;

        LLBC_SetLastError(LLBC_OK);
    }

    // Set application name.
    _name = name;

    // Try load config.
    if (TryLoadConfig() != LLBC_OK)
    {
        _name.clear();
        return LLBC_FAILED;
    }

    // Call OnStart event method.
    if (OnStart(argc, argv) != LLBC_OK)
    {
        LLBC_Cleanup();
        return LLBC_FAILED;
    }

    // Mark started.
    _started = true;

    return LLBC_OK;
}

bool LLBC_IApplication::IsStarted() const
{
    return _started;
}

void LLBC_IApplication::Wait()
{
    if (!_started || _waited)
        return;

    OnWait();
    _services.Wait();

    _waited = true;
}

void LLBC_IApplication::Stop()
{
    if (!_started)
        return;

    if (!_waited)
        Wait();

    OnStop();
    _services.Stop();

    LLBC_Cleanup();

    _started = false;
}

int LLBC_IApplication::SetDumpFile(const LLBC_String &dumpFileName)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
#else // Win32
    if (UNLIKELY(dumpFileName.empty()))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }
    else if (UNLIKELY(!_dumpFileName.empty()))
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    _dumpFileName = dumpFileName;
    LLBC_Strings dumpFileNameParts = LLBC_Directory::SplitExt(_dumpFileName);

    LLBC_Time now = LLBC_Time::Now();
    _dumpFileName = dumpFileNameParts[0];
    _dumpFileName.append_format("_%d%02d%02d_%02d%02d%02d_%06d%s",
        now.GetYear(), now.GetMonth(), now.GetDay(), now.GetHour(), now.GetMinute(),
        now.GetSecond(), now.GetMilliSecond() * 1000 + now.GetMicroSecond(), dumpFileNameParts[1].c_str());
    if (dumpFileNameParts[1] != ".dmp")
        _dumpFileName += ".dmp";

    _dumpFileName = LLBC_Directory::AbsPath(_dumpFileName);

    LLBC_INL_NS __dumpFileName = _dumpFileName.c_str();

    ::SetUnhandledExceptionFilter(LLBC_INL_NS __AppCrashHandler);

#ifdef LLBC_RELEASE
    LLBC_INL_NS __PreventSetUnhandledExceptionFilter();
#endif // Release

    return LLBC_OK;
#endif // Non Win32
}

int LLBC_IApplication::SetCrashHook(const LLBC_Delegate<void(const LLBC_String &)> &crashHook)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
#else // Win32
    _crashHook = crashHook;
    LLBC_INL_NS __crashHook = crashHook;
    return LLBC_OK;
#endif // Non Win32
}

const LLBC_String &LLBC_IApplication::GetName() const
{
    return _name;
}

const LLBC_StartArgs &LLBC_IApplication::GetStartArgs() const
{
    return _startArgs;
}

const LLBC_Ini &LLBC_IApplication::GetIniConfig() const
{
    return _iniConfig;
}

const LLBC_Property &LLBC_IApplication::GetPropertyConfig() const
{
    return _propertyConfig;
}

int LLBC_IApplication::ReloadIniConfig(bool callEvMeth)
{
    LLBC_LockGuard guard(_lock);

    if (_loadingIniCfg)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    _loadingIniCfg = true;

    bool loaded = false;
    if (TryLoadConfig(loaded, true, false) != LLBC_OK)
    {
        _loadingIniCfg = false;
        return LLBC_FAILED;
    }

    if (!loaded)
    {
        _loadingIniCfg = false;
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);

        return LLBC_FAILED;
    }

    _loadingIniCfg = false;

    AfterReloadConfig(true, false, callEvMeth);

    return LLBC_OK;
}

int LLBC_IApplication::ReloadIniConfig(const LLBC_String &configPath, bool callEvMeth)
{
    LLBC_LockGuard guard(_lock);

    if (_loadingIniCfg)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    _loadingIniCfg = true;

    bool loaded = false;
    LLBC_Strings splited = LLBC_Directory::SplitExt(configPath);
    if (TryLoadConfig(splited[0], loaded, true, false) != LLBC_OK)
    {
        _loadingIniCfg = false;
        return LLBC_FAILED;
    }

    if (!loaded)
    {
        _loadingIniCfg = false;

        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _loadingIniCfg = false;

    AfterReloadConfig(true, false, callEvMeth);

    return LLBC_OK;
}

int LLBC_IApplication::ReloadPropertyConfig(bool callEvMeth)
{
    LLBC_LockGuard guard(_lock);

    if (_loadingPropertyCfg)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    _loadingPropertyCfg = true;

    bool loaded = false;
    if (TryLoadConfig(loaded, false, true) != LLBC_OK)
    {
        _loadingPropertyCfg = false;
        return LLBC_FAILED;
    }

    if (!loaded)
    {
        _loadingPropertyCfg = false;

        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _loadingPropertyCfg = false;

    AfterReloadConfig(false, true, callEvMeth);

    return LLBC_OK;
}

int LLBC_IApplication::ReloadPropertyConfig(const LLBC_String &configPath, bool callEvMeth)
{
    LLBC_LockGuard guard(_lock);

    if (_loadingPropertyCfg)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    _loadingPropertyCfg = true;

    bool loaded = false;
    LLBC_Strings splited = LLBC_Directory::SplitExt(configPath);
    if (TryLoadConfig(splited[0], loaded, false, true) != LLBC_OK)
    {
        _loadingPropertyCfg = false;
        return LLBC_FAILED;
    }

    if (!loaded)
    {
        _loadingPropertyCfg = false;

        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _loadingPropertyCfg = false;

    AfterReloadConfig(false, true, callEvMeth);

    return LLBC_OK;
}

LLBC_IService *LLBC_IApplication::GetService(int id) const
{
    return _services.GetService(id);
}

int LLBC_IApplication::RemoveService(int id)
{
    return _services.RemoveService(id);
}

int LLBC_IApplication::Send(LLBC_Packet *packet)
{
    LLBC_IService *service = _services.GetService(packet->GetSenderServiceId());
    if(!service)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    return service->Send(packet);
}

int LLBC_IApplication::TryLoadConfig(bool tryIni, bool tryPropCfg)
{
    bool loaded = false;
    return TryLoadConfig(loaded, tryIni, tryPropCfg);
}

int LLBC_IApplication::TryLoadConfig(bool &loaded, bool tryIni, bool tryPropCfg)
{
    loaded = false;

    // Build all try paths.
    LLBC_Strings tryPaths;
    tryPaths.push_back("Config/" + _name);
    tryPaths.push_back("config/" + _name);
    tryPaths.push_back("Conf/" + _name);
    tryPaths.push_back("conf/" + _name);
    tryPaths.push_back("Cfg/" + _name);
    tryPaths.push_back("cfg/" + _name);
    tryPaths.push_back(_name);

    const size_t tryPathsCount = tryPaths.size();
    for (size_t i = 0; i < tryPathsCount; ++i)
        tryPaths.push_back("../" + tryPaths[i]);

    // Try load.
    for (LLBC_Strings::const_iterator iter = tryPaths.begin();
        iter != tryPaths.end();
        ++iter)
    {
        if (TryLoadConfig(*iter, loaded, tryIni, tryPropCfg) != LLBC_OK)
            return LLBC_FAILED;

        if (loaded)
            break;
    }

    return LLBC_OK;
}

int LLBC_IApplication::TryLoadConfig(const LLBC_String &path, bool &loaded, bool tryIni, bool tryPropCfg)
{
    loaded = false;

    // Try load ini config file.
    if (tryIni)
    {
        const LLBC_String iniPath = path + ".ini";
        if (LLBC_File::Exists(iniPath))
        {
            if (_iniConfig.LoadFromFile(iniPath) != LLBC_OK)
                return LLBC_FAILED;

            loaded = true;
        }
    }

    // Try load property config file.
    if (tryPropCfg)
    {
        const LLBC_String propPath = path + ".cfg";
        if (LLBC_File::Exists(propPath))
        {
            if (_propertyConfig.LoadFromFile(propPath) != LLBC_OK)
                return LLBC_FAILED;

            loaded = true;
        }
    }

    // Finally, not found any llbc library supported config format file, return OK.
    return LLBC_OK;
}

void LLBC_IApplication::AfterReloadConfig(bool iniReloaded, bool propReloaded, bool callEvMeth)
{
    if (!callEvMeth)
        return;

    if (iniReloaded)
        OnIniConfigReloaded();
    if (propReloaded)
        OnPropertyConfigReloaded();

    typedef LLBC_ServiceMgr::Id2Services _Services;
    const _Services &services = _services.GetAllIndexedByIdServices();
    for (_Services::const_iterator it = services.begin();
         it != services.end();
         ++it)
        it->second->NtyApplicationConfigReloaded(iniReloaded, propReloaded);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
