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

#include <signal.h>

#include "llbc.h" //! Include llbc header to use Startup/Cleanup function.
#include "llbc/application/Application.h"

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

, _startThreadId(LLBC_INVALID_NATIVE_THREAD_ID)

, _requireStop(false)
{
    ASSERT(!_thisApp && "Not allow create more than one application object");

    _thisApp = this;
    _libEventHandlers.emplace(LLBC_ApplicationEventType::Stop,
        LLBC_Delegate<void(const LLBC_ApplicationEvent &)>(this, &LLBC_Application::HandleEvent_Stop));
    _libEventHandlers.emplace(LLBC_ApplicationEventType::ReloadApplicationConfig,
        LLBC_Delegate<void(const LLBC_ApplicationEvent &)>(this, &LLBC_Application::HandleEvent_ReloadAppCfg));
}

LLBC_Application::~LLBC_Application()
{
    ASSERT(!IsStarted() && "Please stop application before destruct");
    _thisApp = nullptr;
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

    // Check started flag.
    LLBC_SetErrAndReturnIf(IsStarted(), LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Lock and check _started flag again.
    LLBC_LockGuard guard(_cfgLock);
    LLBC_SetErrAndReturnIf(IsStarted(), LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Save config info.
    _cfgType = cfgType;
    _cfgPath = cfgPath;

    return LLBC_OK;
}

int LLBC_Application::ReloadConfig(bool callEvMeth)
{
    // Not allow reload config if application not start.
    LLBC_SetErrAndReturnIf(!IsStarted(), LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Lock and check again.
    LLBC_LockGuard guard(_cfgLock);
    LLBC_SetErrAndReturnIf(!IsStarted(), LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Config not found when application start.
    LLBC_SetErrAndReturnIf(_cfgType == LLBC_ApplicationConfigType::End, LLBC_ERROR_NOT_FOUND, LLBC_FAILED);

    // Reload.
    LLBC_ReturnIf(LoadConfig(false) != LLBC_OK, LLBC_FAILED);

    // Call config reload event method.
    if (callEvMeth)
    {
        OnConfigReload();
        for (auto &svcPair : this->_services.GetAllIndexedByIdServices())
            svcPair.second->ProcessAppConfigReload();
    }

    return LLBC_OK;
}

int LLBC_Application::Start(const LLBC_String &name, int argc, char *argv[])
{
    // Multi application check.
    LLBC_SetErrAndReturnIf(_thisApp != this, LLBC_ERROR_REPEAT, LLBC_FAILED);
    // Application name check.
    LLBC_SetErrAndReturnIf(name.empty(), LLBC_ERROR_INVALID, LLBC_FAILED);

    // Reentry check.
    LLBC_SetErrAndReturnIf(IsStarted(), LLBC_ERROR_REENTRY, LLBC_FAILED);

    // Parse startup arguments.
    LLBC_ReturnIf(_startArgs.Parse(argc, argv) != LLBC_OK, LLBC_FAILED);

    // Startup llbc library.
    int ret = LLBC_FAILED;
    _llbcLibStartupInApp = true;
    if (LLBC_Startup() != LLBC_OK)
    {
        LLBC_ReturnIf(LLBC_GetLastError() != LLBC_ERROR_REENTRY, LLBC_FAILED);

        _llbcLibStartupInApp = false;
        LLBC_SetLastError(LLBC_OK);
    }

    // Set name.
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
    LLBC_ReturnIf(!_cfgPath.empty() && LoadConfig(true) != LLBC_OK, LLBC_FAILED);

    // Hook process crash.
    if (LLBC_HookProcessCrash() != LLBC_OK)
        return LLBC_FAILED;

    // Install required signal handlers.
    // - App stop signals
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = LLBC_Application::HandleSignal_Stop;
    const int stopSigs[]LLBC_CFG_APP_STOP_SIGNALS;
    for (auto &stopSig : stopSigs)
        sigaction(stopSig, &sa, nullptr);
    // - App config reload signal
    const int cfgReloadSigs[]LLBC_CFG_APP_CFG_RELOAD_SIGNALS;
    sa.sa_handler = LLBC_Application::HandleSignal_ReloadAppCfg;
    for (auto &cfgReloadSig : cfgReloadSigs)
        sigaction(cfgReloadSig, &sa, nullptr);

    // Call OnWillStart event method.
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    if (OnWillStart(argc, argv) != LLBC_OK)
    {
        if (LLBC_GetLastError() == LLBC_ERROR_SUCCESS)
            LLBC_SetLastError(LLBC_ERROR_UNKNOWN);
        return LLBC_FAILED;
    }

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
    _startThreadId = LLBC_GetCurrentThreadId();

    // Call OnStartFinish event method.
    OnStartFinish(argc, argv);

    // Return ok.
    ret = LLBC_OK;
    return ret;
}

void LLBC_Application::Stop()
{
    // Not start judge.
    if (!IsStarted())
        return;

    // If caller thread is not application start thread, push AppStop event and return.
    if (LLBC_GetCurrentThreadId() != _startThreadId)
    {
        PushEvent(LLBC_ApplicationEventType::Stop);
        return;
    }

    // Call OnWillStop event method.
    OnWillStop();

    // Stop all services.
    _services.StopAll(true);

    // Stop app.
    while (true)
    {
        bool stopFinished = true;
        OnStop(stopFinished);
        LLBC_BreakIf(stopFinished);

        LLBC_Sleep(LLBC_CFG_APP_TRY_STOP_INTERVAL);
    }

    // Mask stopped.
    _startThreadId = LLBC_INVALID_NATIVE_THREAD_ID;

    // Call OnStopFinish event method.
    OnStopFinish();

    // Uninstall required signal handlers.
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_DFL;
    const int stopSigs[]LLBC_CFG_APP_STOP_SIGNALS;
    for (auto &stopSig : stopSigs)
        sigaction(stopSig, &sa, nullptr);
    const int cfgReloadSigs[]LLBC_CFG_APP_CFG_RELOAD_SIGNALS;
    for (auto &cfgReloadSig : cfgReloadSigs)
        sigaction(cfgReloadSig, &sa, nullptr);

    // Cleanup members.
    _cfgPath.clear();
    _cfgType = LLBC_ApplicationConfigType::End;
    _propCfg.RemoveAllProperties();
    _nonPropCfg.BecomeNil();

    for (auto &events : _events)
        LLBC_STLHelper::RecycleContainer(events);

    _requireStop = false;

    // Cleanup llbc.
    LLBC_DoIf(_llbcLibStartupInApp, LLBC_Cleanup(); _llbcLibStartupInApp = false);
}

int LLBC_Application::Run()
{
    // If not started, return failed.
    LLBC_SetErrAndReturnIf(!IsStarted(), LLBC_ERROR_NOT_INIT, LLBC_FAILED);
    // If not start thread, return failed.
    LLBC_SetErrAndReturnIf(LLBC_GetCurrentThreadId() != _startThreadId, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Run loop.
    while (true)
    {
        // Call OnRun event method.
        bool runDoNothing = true;
        OnRun(runDoNothing);
        // Handle events.
        bool handleEvsDoNothing = true;
        HandleEvents(handleEvsDoNothing);

        // If require stop, execute stop.
        if (_requireStop)
        {
            Stop();
            return LLBC_OK;
        }

        // Execute sleep, if not do anything.
        if (runDoNothing && handleEvsDoNothing)
            LLBC_Sleep(1);
    }
}

int LLBC_Application::PushEvent(LLBC_ApplicationEvent *ev)
{
    LLBC_LockGuard guard(_eventLock);
    if (!IsStarted() &&
        ev->evType != LLBC_ApplicationEventType::Stop)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    _events[0].push_back(ev);

    return LLBC_OK;
}

int LLBC_Application::SubscribeEvent(int evType, const LLBC_Delegate<void(const LLBC_ApplicationEvent &)> &evHandler)
{
    // Check event type & event handler.
    if (evType < LLBC_ApplicationEventType::LibBegin ||
        evType >= LLBC_ApplicationEventType::LogicEnd ||
        !evHandler)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    // Lock.
    LLBC_LockGuard guard(_eventLock);
    // Not allow subscribe event when app has been start.
    LLBC_SetErrAndReturnIf(IsStarted(), LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Repeat subscribe check.
    if (_logicEventHandlers.find(evType) != _logicEventHandlers.end())
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    _logicEventHandlers.emplace(evType, evHandler);

    return LLBC_OK;
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

int LLBC_Application::LoadConfig(bool lock)
{
    // Reentry check.
    LLBC_SetErrAndReturnIf(_loadingCfg, LLBC_ERROR_REENTRY, LLBC_FAILED);

    // Lock and check again.
    if (lock)
        _cfgLock.Lock();
    LLBC_Defer(if (lock) _cfgLock.Unlock());
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
    const auto xmlLoadRet = doc.LoadFile(_cfgPath.c_str());
    if (xmlLoadRet != ::llbc::tinyxml2::XML_SUCCESS)
    {
        LLBC_String customErrStr;
        customErrStr.format("load xml config file failed, file:%s, errno(tinyxml2):%d, error str:%s",
                            _cfgPath.c_str(), xmlLoadRet, doc.ErrorStr());
        LLBC_SetLastError(LLBC_ERROR_FORMAT, customErrStr.c_str());
        return LLBC_FAILED;
    }

    LLBC_VariantUtil::Xml2Variant(doc, _nonPropCfg);
    return LLBC_OK;
}

int LLBC_Application::LoadPropertyConfig()
{
    return _propCfg.LoadFromFile(_cfgPath);
}

void LLBC_Application::HandleEvents(bool &doNothing)
{
    _eventLock.Lock();
    _events[0].swap(_events[1]);
    _eventLock.Unlock();

    auto &events = _events[1];
    if (events.empty())
        return;

    LLBC_Defer(doNothing = false);
    LLBC_Defer(LLBC_STLHelper::RecycleContainer(events));
    for (auto &ev : events)
    {
        const auto &evType = ev->evType;
        if (evType >= LLBC_ApplicationEventType::LibBegin &&
            evType < LLBC_ApplicationEventType::LibEnd)
        {
            auto libHandlerIt = _libEventHandlers.find(evType);
            if (libHandlerIt != _libEventHandlers.end())
                libHandlerIt->second(*ev);
        }

        auto logicHandlerIt = _logicEventHandlers.find(evType);
        if (logicHandlerIt != _logicEventHandlers.end())
            logicHandlerIt->second(*ev);

        if (evType == LLBC_ApplicationEventType::Stop)
            return;
    }
}

void LLBC_Application::HandleEvent_ReloadAppCfg(const LLBC_ApplicationEvent &ev)
{
    ReloadConfig();
}

void LLBC_Application::HandleEvent_Stop(const LLBC_ApplicationEvent &ev)
{
    _requireStop = true;
}

void LLBC_Application::HandleSignal_Stop(int sig)
{
    ThisApp()->PushEvent(LLBC_ApplicationEventType::Stop);
}

void LLBC_Application::HandleSignal_ReloadAppCfg(int sig)
{
    ThisApp()->PushEvent(LLBC_ApplicationEventType::ReloadApplicationConfig);
}

__LLBC_NS_END

