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

#include "llbc.h" //! Include llbc header to use Startup/Cleanup function.
#include "llbc/application/Application.h"

#if LLBC_TARGET_PLATFORM_WIN32

__LLBC_INTERNAL_NS_BEGIN

static const char *__dumpFilePath = nullptr;
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
        __crashHook(__dumpFilePath);

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

__LLBC_INTERNAL_NS_BEGIN

static const LLBC_NS LLBC_String __cfgSuffix_Ini = ".ini";
static const LLBC_NS LLBC_String __cfgSuffix_Xml = ".xml";
static const LLBC_NS LLBC_String __cfgSuffix_Property = ".cfg";
static const LLBC_NS LLBC_String __cfgSuffix_Unknown = "";

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

const LLBC_String &LLBC_ApplicationConfigType::GetConfigSuffix(int cfgType)
{
    if (cfgType == Ini)
        return LLBC_INL_NS __cfgSuffix_Ini;
    else if (cfgType == Xml)
        return LLBC_INL_NS __cfgSuffix_Xml;
    else if (cfgType == Property)
        return LLBC_INL_NS __cfgSuffix_Property;
    else
        return LLBC_INL_NS __cfgSuffix_Unknown;
}

auto LLBC_ApplicationConfigType::GetConfigType(const LLBC_String &cfgSuffix) -> ENUM
{
    auto nmlCfgSuffix = cfgSuffix.strip().tolower();
    if (nmlCfgSuffix == LLBC_INL_NS __cfgSuffix_Ini)
        return Ini;
    else if (nmlCfgSuffix == LLBC_INL_NS __cfgSuffix_Xml)
        return Xml;
    else if (nmlCfgSuffix == LLBC_INL_NS __cfgSuffix_Property)
        return Property;
    else
        return End;
}

LLBC_Application *LLBC_Application::_thisApp = nullptr;

LLBC_Application::LLBC_Application()
: _name()

, _llbcLibStartupInApp(false)

, _loadingCfg(false)
, _cfgType(LLBC_ApplicationConfigType::End)

, _services(*LLBC_ServiceMgrSingleton)

, _started(false)

#if LLBC_TARGET_PLATFORM_WIN32
, _crashHook(nullptr)
#endif // Win32
{
    LLBC_DoIf(_thisApp == nullptr, _thisApp = this);
}

LLBC_Application::~LLBC_Application()
{
    Stop();
}

int LLBC_Application::SetConfigPath(const LLBC_String &cfgPath)
{
    // Check config file exists or not.
    LLBC_SetErrAndReturnIf(!LLBC_File::Exists(cfgPath), LLBC_ERROR_ARG, LLBC_FAILED);

    // Get config type.
    const auto ext = LLBC_Directory::SplitExt(cfgPath)[1];
    const auto cfgType = LLBC_ApplicationConfigType::GetConfigType(ext);
    LLBC_SetErrAndReturnIf(cfgType < LLBC_ApplicationConfigType::Begin || 
                                cfgType >= LLBC_ApplicationConfigType::End,
                           LLBC_ERROR_NOT_SUPPORT,
                           LLBC_FAILED);

    // Check _started flag.
    LLBC_SetErrAndReturnIf(_started, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Lock and check _started flag again.
    LLBC_LockGuard guard(_cfgLock);
    LLBC_SetErrAndReturnIf(_started, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Save config info.
    _cfgType = cfgType;
    _cfgPath = cfgPath;

    return LLBC_OK;
}

int LLBC_Application::ReloadConfig(bool callEvMeth)
{
    // Not allow reload config if application not start.
    LLBC_SetErrAndReturnIf(!_started, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Lock and check again.
    LLBC_LockGuard guard(_cfgLock);
    LLBC_SetErrAndReturnIf(!_started, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Config not found when application start.
    LLBC_SetErrAndReturnIf(_cfgType == LLBC_ApplicationConfigType::End, LLBC_ERROR_NOT_FOUND, LLBC_FAILED);

    // Reload.
    LLBC_ReturnIf(LoadConfig() != LLBC_OK, LLBC_FAILED);

    // Call config reload event method.
    LLBC_DoIf(callEvMeth, OnConfigReload());

    return LLBC_OK;
}

int LLBC_Application::Start(const LLBC_String &name, int argc, char *argv[])
{
    // Multi application check.
    LLBC_SetErrAndReturnIf(_thisApp != this, LLBC_ERROR_REPEAT, LLBC_FAILED);
    // Application name check.
    LLBC_SetErrAndReturnIf(name.empty(), LLBC_ERROR_INVALID, LLBC_FAILED);

    // Reentry check.
    LLBC_SetErrAndReturnIf(_started, LLBC_ERROR_REENTRY, LLBC_FAILED);

    // Parse startup arguments.
    LLBC_ReturnIf(_startArgs.Parse(argc, argv) != LLBC_OK, LLBC_FAILED);

    // Startup llbc library.
    int ret = LLBC_FAILED;
    _llbcLibStartupInApp = true;
    if (LLBC_Startup() != LLBC_OK)
    {
        LLBC_ReturnIf(LLBC_Errno != LLBC_ERROR_REENTRY, LLBC_FAILED);

        _llbcLibStartupInApp = false;
        LLBC_SetLastError(LLBC_OK);
    }

    // Set application name.
    _name = name;

    // Define app start failed defer.
    LLBC_Defer(if (ret != LLBC_OK) { 
        _cfgPath.clear();
        _cfgType = LLBC_ApplicationConfigType::End;
        _propCfg.RemoveAllProperties();
        _nonPropCfg.BecomeNil();

        _name.clear();

        LLBC_DoIf(_llbcLibStartupInApp, LLBC_Cleanup(); _llbcLibStartupInApp = false);
    });

    // Locate config path.
    if (_cfgPath.empty())
    {
        int intCfgType;
        if (!(_cfgPath = LocateConfigPath(name, intCfgType)).empty())
            _cfgType = static_cast<LLBC_ApplicationConfigType::ENUM>(intCfgType);
    }

    // Load config.
    LLBC_ReturnIf(!_cfgPath.empty() && LoadConfig() != LLBC_OK, LLBC_FAILED);

    // Call OnStart event method.
    while (true)
    {
        bool startFinished = true;

        LLBC_SetLastError(LLBC_ERROR_SUCCESS);
        if (OnStart(argc, argv, startFinished) != LLBC_OK)
        {
            if (LLBC_GetLastError() == LLBC_ERROR_SUCCESS)
                LLBC_SetLastError(LLBC_ERROR_UNKNOWN);
            return LLBC_FAILED;
        }

        LLBC_BreakIf(startFinished);
        LLBC_Sleep(LLBC_CFG_APP_TRY_START_INTERVAL);
    }

    // Mark started.
    _started = true;

    // Return ok.
    ret = LLBC_OK;
    return ret;
}

void LLBC_Application::Stop()
{
    if (!_started)
        return;

    while (true)
    {
        LLBC_BreakIf(OnStop());
        LLBC_Sleep(LLBC_CFG_APP_TRY_STOP_INTERVAL);
    }

    _services.Stop();

    _cfgPath.clear();
    _cfgType = LLBC_ApplicationConfigType::End;
    _propCfg.RemoveAllProperties();
    _nonPropCfg.BecomeNil();

    LLBC_DoIf(_llbcLibStartupInApp, LLBC_Cleanup(); _llbcLibStartupInApp = false);

    _started = false;
}

int LLBC_Application::SetDumpFile(const LLBC_String &dumpFilePath)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
#else // Win32
    LLBC_SetErrAndReturnIf(dumpFilePath.empty(), LLBC_ERROR_ARG, LLBC_FAILED);
    LLBC_SetErrAndReturnIf(!_dumpFilePath.empty(), LLBC_ERROR_REPEAT, LLBC_FAILED);

    _dumpFilePath = dumpFilePath;
    const LLBC_Strings dumpFileNameParts = LLBC_Directory::SplitExt(_dumpFilePath);

    LLBC_Time now = LLBC_Time::Now();
    _dumpFilePath = dumpFileNameParts[0];
    _dumpFilePath.append_format("_%d%02d%02d_%02d%02d%02d_%06d%s",
        now.GetYear(), now.GetMonth(), now.GetDay(), now.GetHour(), now.GetMinute(),
        now.GetSecond(), now.GetMilliSecond() * 1000 + now.GetMicroSecond(), dumpFileNameParts[1].c_str());
    if (dumpFileNameParts[1] != ".dmp")
        _dumpFilePath += ".dmp";

    _dumpFilePath = LLBC_Directory::AbsPath(_dumpFilePath);

    LLBC_INL_NS __dumpFilePath = _dumpFilePath.c_str();

    ::SetUnhandledExceptionFilter(LLBC_INL_NS __AppCrashHandler);

#ifdef LLBC_RELEASE
    LLBC_INL_NS __PreventSetUnhandledExceptionFilter();
#endif // Release

    return LLBC_OK;
#endif // Non Win32
}

int LLBC_Application::SetCrashHook(const LLBC_Delegate<void(const LLBC_String &)> &crashHook)
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

LLBC_String LLBC_Application::LocateConfigPath(const LLBC_String &appName, int &cfgType)
{
    // Application config directory locate order:
    // -> <cwd>
    // -> <module file directory>
    //    -> <cwd>/Config -> config -> Conf -> conf -> Cfg -> cfg
    //    -> <module file directory>/Config -> config -> Conf -> conf -> Cfg -> cfg
    //       -> <cwd>/../Config -> config -> Conf -> conf -> Cfg -> cfg
    //       -> <module file directory>/../Config -> config -> Conf -> conf -> Cfg -> cfg
    //          -> <cwd>/..
    //          -> <module file directory>/..
    //
    // Application config filename locate order:
    // -> app_name
    //    -> stripped _d/_debug suffix app_name
    //       -> executable_name
    //          -> stripped _d/_debug suffix executable_name
    //
    // Application config type locate order:
    // -> .ini
    //    -> .xml
    //       -> .cfg

    // Build config directories.
    // -> <cwd>
    //    -> <module file directory>
    LLBC_Strings cfgFileDirs{LLBC_Directory::CurDir(),
                             LLBC_Directory::ModuleFileDir()};

    // -> <cwd>/xxxxx
    //    -> <module file directory/xxxxx
    const char *interDirs[] = {"Config", "config", "Conf", "conf", "Cfg", "cfg"};
    for (auto &cfgDir : interDirs)
        cfgFileDirs.push_back(LLBC_Directory::Join(LLBC_Directory::CurDir(), cfgDir));
    for (auto &cfgDir : interDirs)
        cfgFileDirs.push_back(LLBC_Directory::Join(LLBC_Directory::ModuleFileDir(), cfgDir));
    // -> <cwd>/../xxxxx
    //    -> <module file directory>/../xxxxx
    for (auto &cfgDir : interDirs)
        cfgFileDirs.push_back(LLBC_Directory::Join(LLBC_Directory::CurDir(), "..", cfgDir));
    for (auto &cfgDir : interDirs)
        cfgFileDirs.push_back(LLBC_Directory::Join(LLBC_Directory::ModuleFileDir(), "..", cfgDir));
    // -> <cwd>/..
    //    -> <module file directory>/..
    cfgFileDirs.push_back(LLBC_Directory::Join(LLBC_Directory::CurDir(), ".."));
    cfgFileDirs.push_back(LLBC_Directory::Join(LLBC_Directory::ModuleFileDir(), ".."));

    // Building config filenames.
    LLBC_Strings cfgFileNames{appName};
    if (appName.endswith("_d") || appName.endswith("_debug"))
              cfgFileNames.push_back(appName.substr(0, appName.rfind('_')));
    const auto execName = LLBC_Directory::SplitExt(LLBC_Directory::BaseName(LLBC_Directory::ModuleFileName()))[0];
    cfgFileNames.push_back(execName);
    if (execName.endswith("_d") || execName.endswith("_debug"))
        cfgFileNames.push_back(execName.substr(0, execName.rfind('_')));

    // Execute config file locate.
    for (auto &cfgFileDir : cfgFileDirs)
    {
        for (auto &cfgFileName : cfgFileNames)
        {
            for (cfgType = LLBC_ApplicationConfigType::Begin;
                 cfgType != LLBC_ApplicationConfigType::End;
                 ++cfgType)
            {
                const auto cfgPath = LLBC_Directory::Join(
                    cfgFileDir, cfgFileName + LLBC_ApplicationConfigType::GetConfigSuffix(cfgType));
                LLBC_ReturnIf(LLBC_File::Exists(cfgPath), cfgPath);
            }
        }
    }

    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
    return "";
}

int LLBC_Application::LoadConfig()
{
    // Reentry check.
    LLBC_SetErrAndReturnIf(_loadingCfg, LLBC_ERROR_REENTRY, LLBC_FAILED);

    // Lock and check again.
    LLBC_LockGuard guard(_cfgLock);
    LLBC_SetErrAndReturnIf(_loadingCfg, LLBC_ERROR_REENTRY, LLBC_FAILED);

    // Check config file exist or not.
    LLBC_SetErrAndReturnIf(!LLBC_File::Exists(_cfgPath), LLBC_ERROR_NOT_FOUND, LLBC_FAILED);

    _loadingCfg = true;
    LLBC_Defer(_loadingCfg = false);

    LLBC_DoIf(_cfgType == LLBC_ApplicationConfigType::Ini, return LoadIniConfig());
    LLBC_DoIf(_cfgType == LLBC_ApplicationConfigType::Xml, return LoadXmlConfig());
    LLBC_DoIf(true, return LoadPropertyConfig());
}

int LLBC_Application::LoadIniConfig()
{
    LLBC_Ini ini;
    LLBC_ReturnIf(ini.LoadFromFile(_cfgPath) != LLBC_OK, LLBC_FAILED);

    LLBC_VariantUtil::Ini2Variant(ini, _nonPropCfg);
    return LLBC_OK;
}

int LLBC_Application::LoadXmlConfig()
{
    ::llbc::tinyxml2::XMLDocument doc;
    LLBC_SetErrAndReturnIf(doc.LoadFile(_cfgPath.c_str()) != ::llbc::tinyxml2::XML_SUCCESS,
                           LLBC_ERROR_FORMAT,
                           LLBC_FAILED);

    LLBC_VariantUtil::Xml2Variant(doc, _nonPropCfg);
    return LLBC_OK;
}

int LLBC_Application::LoadPropertyConfig()
{
    return _propCfg.LoadFromFile(_cfgPath);
}

__LLBC_NS_END

