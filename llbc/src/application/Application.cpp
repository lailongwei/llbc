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
#include "llbc/application/Application.h"

__LLBC_NS_BEGIN

LLBC_BaseApplication *LLBC_BaseApplication::_thisApp = NULL;

LLBC_BaseApplication::LLBC_BaseApplication()
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

LLBC_BaseApplication::~LLBC_BaseApplication()
{
    Wait();
    Stop();
}

int LLBC_BaseApplication::OnStart(int argc, char *argv[])
{
    return LLBC_OK;
}

void LLBC_BaseApplication::OnWait()
{
}

void LLBC_BaseApplication::OnStop()
{
}

LLBC_BaseApplication *LLBC_BaseApplication::ThisApp()
{
    return _thisApp;
}

int LLBC_BaseApplication::Start(const LLBC_String &name, int argc, char *argv[])
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

bool LLBC_BaseApplication::IsStarted() const
{
    return _started;
}

void LLBC_BaseApplication::Wait()
{
    if (!_started || _waited)
        return;

    OnWait();
    _services.Wait();

    _waited = true;
}

void LLBC_BaseApplication::Stop()
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

const LLBC_String &LLBC_BaseApplication::GetName() const
{
    return _name;
}

const LLBC_Ini &LLBC_BaseApplication::GetIniConfig() const
{
    return _iniConfig;
}

const LLBC_Config &LLBC_BaseApplication::GetJsonConfig() const
{
    return _jsonConfig;
}

const LLBC_Property &LLBC_BaseApplication::GetPropertyConfig() const
{
    return _propertyConfig;
}

LLBC_IService *LLBC_BaseApplication::GetService(int id) const
{
    return _services.GetService(id);
}

int LLBC_BaseApplication::RemoveService(int id)
{
    return _services.RemoveService(id);
}

int LLBC_BaseApplication::Send(LLBC_Packet *packet)
{
    LLBC_IService *service = _services.GetService(packet->GetSenderServiceId());
    if(!service)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    return service->Send(packet);
}

int LLBC_BaseApplication::TryLoadConfig()
{
    // Build all try paths.
    LLBC_Strings tryPaths;
    tryPaths.push_back("Config/" + _name);
    tryPaths.push_back("config/" + _name);
    tryPaths.push_back("Conf/" + _name);
    tryPaths.push_back("conf/" + _name);
    tryPaths.push_back("cfg/" + _name);
    tryPaths.push_back("cfg/" + _name);
    tryPaths.push_back(_name);

    const size_t tryPathsCount = tryPaths.size();
    for (size_t i = 0; i < tryPathsCount; i++)
        tryPaths.push_back("../" + tryPaths[i]);

    // Try load.
    bool loaded = false;
    for (LLBC_Strings::const_iterator iter = tryPaths.begin();
        iter != tryPaths.end();
        iter++)
    {
        if (TryLoadConfig(*iter, loaded) != LLBC_OK)
            return LLBC_FAILED;

        if (loaded)
            break;
    }

    return LLBC_OK;
}

int LLBC_BaseApplication::TryLoadConfig(const LLBC_String &path, bool &loaded)
{
    loaded = false;

    // Try load ini config file.
    const LLBC_String iniPath = path + ".ini";
    if (LLBC_File::Exists(iniPath))
    {
        if (_iniConfig.LoadFromFile(iniPath) != LLBC_OK)
            return LLBC_FAILED;

        loaded = true;
        return LLBC_OK;
    }

    // Try load json config file.
    const LLBC_String jsonPath = path + ".json";
    if (LLBC_File::Exists(jsonPath))
    {
        _jsonConfig.AddFile(jsonPath);
        if (_jsonConfig.Initialize() != LLBC_OK)
            return LLBC_FAILED;

        loaded = true;
        return LLBC_OK;
    }

    // Try load property config file.
    const LLBC_String propPath = path + ".cfg";
    if (LLBC_File::Exists(propPath))
    {
        if (_propertyConfig.LoadFromFile(propPath) != LLBC_OK)
            return LLBC_FAILED;

        loaded = true;
        return LLBC_OK;
    }

    // Finally, not found any llbc library supported config format file, return OK.
    return LLBC_OK;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
