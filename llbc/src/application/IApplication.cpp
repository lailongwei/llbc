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

static const char *__dumpFileName = NULL;

static LONG WINAPI __AppCrashHandler(EXCEPTION_POINTERS *exception)
{
    HANDLE dmpFile = ::CreateFileA(__dumpFileName,
                                   GENERIC_WRITE,
                                   FILE_SHARE_READ,
                                   NULL,
                                   CREATE_ALWAYS,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);
    if (UNLIKELY(dmpFile == INVALID_HANDLE_VALUE))
        return EXCEPTION_CONTINUE_SEARCH;

    MINIDUMP_EXCEPTION_INFORMATION dmpInfo;
    dmpInfo.ExceptionPointers = exception;
    dmpInfo.ThreadId = GetCurrentThreadId();
    dmpInfo.ClientPointers = TRUE;

    ::MiniDumpWriteDump(::GetCurrentProcess(),
                        ::GetCurrentProcessId(),
                        dmpFile,
                        MiniDumpNormal,
                        &dmpInfo,
                        NULL,
                        NULL);

    ::CloseHandle(dmpFile);

    LLBC_NS LLBC_String errMsg;
    errMsg.append("Unhandled exception!");
    errMsg.append_format("The mini dump file path: %s.", __dumpFileName);
    ::FatalAppExitA(0, errMsg.c_str());

    return EXCEPTION_EXECUTE_HANDLER;
}

__LLBC_INTERNAL_NS_END

#endif // Win32

__LLBC_NS_BEGIN

LLBC_IApplication *LLBC_IApplication::_thisApp = NULL;

LLBC_IApplication::LLBC_IApplication()
: _name()

, _iniConfig()
, _jsonConfig()
, _propertyConfig()

, _services(*LLBC_ServiceMgrSingleton)

, _started(false)
, _waited(false)
{
    if (_thisApp == NULL)
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
    if (LLBC_Directory::SplitExt(_dumpFileName)[1] != ".dmp")
        _dumpFileName += ".dmp";
    _dumpFileName = LLBC_Directory::AbsPath(_dumpFileName);

    LLBC_INL_NS __dumpFileName = _dumpFileName.c_str();

    ::SetUnhandledExceptionFilter(LLBC_INL_NS __AppCrashHandler);

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

const LLBC_Config &LLBC_IApplication::GetJsonConfig() const
{
    return _jsonConfig;
}

const LLBC_Property &LLBC_IApplication::GetPropertyConfig() const
{
    return _propertyConfig;
}

int LLBC_IApplication::ReloadIniConfig()
{
    bool loaded = false;
    if (TryLoadConfig(loaded, true, false, false) != LLBC_OK)
        return LLBC_FAILED;

    if (!loaded)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_IApplication::ReloadIniConfig(const LLBC_String &configPath)
{
    bool loaded = false;
    LLBC_Strings splited = LLBC_Directory::SplitExt(configPath);
    if (TryLoadConfig(splited[0], loaded, true, false, false) != LLBC_OK)
        return LLBC_FAILED;

    if (!loaded)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_IApplication::ReloadJsonConfig()
{
    bool loaded = false;
    if (TryLoadConfig(loaded, false, true, false) != LLBC_OK)
        return LLBC_FAILED;

    if (!loaded)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_IApplication::ReloadJsonConfig(const LLBC_String &configPath)
{
    bool loaded = false;
    LLBC_Strings splited = LLBC_Directory::SplitExt(configPath);
    if (TryLoadConfig(splited[0], loaded, false, true, false) != LLBC_OK)
        return LLBC_FAILED;

    if (!loaded)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_IApplication::ReloadPropertyConfig()
{
    bool loaded = false;
    if (TryLoadConfig(loaded, false, false, true) != LLBC_OK)
        return LLBC_FAILED;

    if (!loaded)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_IApplication::ReloadPropertyConfig(const LLBC_String &configPath)
{
    bool loaded = false;
    LLBC_Strings splited = LLBC_Directory::SplitExt(configPath);
    if (TryLoadConfig(splited[0], loaded, false, false, true) != LLBC_OK)
        return LLBC_FAILED;

    if (!loaded)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

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

int LLBC_IApplication::TryLoadConfig(bool tryIni, bool tryJson, bool tryCfg)
{
    bool loaded = false;
    return TryLoadConfig(loaded, tryIni, tryJson, tryCfg);
}

int LLBC_IApplication::TryLoadConfig(bool &loaded, bool tryIni, bool tryJson, bool tryCfg)
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
    for (size_t i = 0; i < tryPathsCount; i++)
        tryPaths.push_back("../" + tryPaths[i]);

    // Try load.
    for (LLBC_Strings::const_iterator iter = tryPaths.begin();
        iter != tryPaths.end();
        iter++)
    {
        if (TryLoadConfig(*iter, loaded, tryIni, tryJson, tryCfg) != LLBC_OK)
            return LLBC_FAILED;

        if (loaded)
            break;
    }

    return LLBC_OK;
}

int LLBC_IApplication::TryLoadConfig(const LLBC_String &path, bool &loaded, bool tryIni, bool tryJson, bool tryCfg)
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

    // Try load json config file.
    if (tryJson)
    {
        const LLBC_String jsonPath = path + ".json";
        if (LLBC_File::Exists(jsonPath))
        {
            _jsonConfig.AddFile(jsonPath);
            if (_jsonConfig.Initialize() != LLBC_OK)
                return LLBC_FAILED;

            loaded = true;
        }
    }

    // Try load property config file.
    if (tryCfg)
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

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
