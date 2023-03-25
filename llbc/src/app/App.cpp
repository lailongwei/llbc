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
#include "llbc/comm/ServiceEvent.h"
#include "llbc/app/App.h"

__LLBC_INTERNAL_NS_BEGIN

static const LLBC_NS LLBC_String __cfgSuffix_Ini = ".ini";
static const LLBC_NS LLBC_String __cfgSuffix_Xml = ".xml";
static const LLBC_NS LLBC_String __cfgSuffix_Property = ".cfg";
static const LLBC_NS LLBC_String __cfgSuffix_Unknown = "";

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

const LLBC_String &LLBC_AppConfigType::GetConfigSuffix(int cfgType)
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

auto LLBC_AppConfigType::GetConfigType(const LLBC_String &cfgSuffix) -> ENUM
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

LLBC_App *LLBC_App::_thisApp = nullptr;

LLBC_App::LLBC_App()
: _name()
, _startPhase(LLBC_AppStartPhase::Stopped)

, _llbcLibStartupInApp(false)

, _loadingCfg(false)
, _preventCfgLoad(0)
, _cfgType(LLBC_AppConfigType::End)

, _services(*LLBC_ServiceMgrSingleton)

, _startThreadId(LLBC_INVALID_NATIVE_THREAD_ID)

, _requireStop(false)
{
    ASSERT(!_thisApp && "Not allow create more than one application object");

    _thisApp = this;
    _libEventHandlers.emplace(LLBC_AppEventType::Stop,
        LLBC_Delegate<void(const LLBC_AppEvent &)>(this, &LLBC_App::HandleEvent_Stop));
    _libEventHandlers.emplace(LLBC_AppEventType::ReloadApplicationConfig,
        LLBC_Delegate<void(const LLBC_AppEvent &)>(this, &LLBC_App::HandleEvent_ReloadAppCfg));
}

LLBC_App::~LLBC_App()
{
    ASSERT(!IsStarted() && "Please stop application before destruct");
    LLBC_DoIf(_llbcLibStartupInApp, LLBC_Cleanup());
    _thisApp = nullptr;
}

int LLBC_App::SetConfigPath(const LLBC_String &cfgPath)
{
    // Check config file exists or not.
    LLBC_SetErrAndReturnIf(!LLBC_File::Exists(cfgPath), LLBC_ERROR_ARG, LLBC_FAILED);

    // Get config type.
    const auto ext = LLBC_Directory::SplitExt(cfgPath)[1];
    const auto cfgType = LLBC_AppConfigType::GetConfigType(ext);
    LLBC_SetErrAndReturnIf(cfgType < LLBC_AppConfigType::Begin ||
                                cfgType >= LLBC_AppConfigType::End,
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

int LLBC_App::ReloadConfig(bool callEvMeth)
{
    // Not allow reload config if application not start.
    LLBC_SetErrAndReturnIf(!IsStarted(), LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Lock and check again.
    LLBC_LockGuard guard(_cfgLock);
    LLBC_SetErrAndReturnIf(!IsStarted(), LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Config not found when application start.
    LLBC_SetErrAndReturnIf(_cfgType == LLBC_AppConfigType::End, LLBC_ERROR_NOT_FOUND, LLBC_FAILED);

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

void LLBC_App::PreventConfigLoad()
{
    LLBC_LockGuard guard(_cfgLock);
    ++_preventCfgLoad;
}

void LLBC_App::CancelPreventConfigLoad()
{
    LLBC_LockGuard guard(_cfgLock);
    _preventCfgLoad = MAX(0, _preventCfgLoad - 1);
}

int LLBC_App::Start(int argc, char *argv[], const LLBC_String &name)
{
    // Multi application check.
    LLBC_SetErrAndReturnIf(_thisApp != this, LLBC_ERROR_REPEAT, LLBC_FAILED);

    // Reentry check.
    LLBC_SetErrAndReturnIf(!IsStopped(), LLBC_ERROR_REENTRY, LLBC_FAILED);

    // Parse startup arguments.
    LLBC_ReturnIf(_startArgs.Parse(argc, argv) != LLBC_OK, LLBC_FAILED);

    // Startup llbc library.
    int ret = LLBC_FAILED;
    _llbcLibStartupInApp = true;
    if (LLBC_Startup() != LLBC_OK)
    {
        LLBC_ReturnIf(LLBC_GetLastError() != LLBC_ERROR_REENTRY, LLBC_FAILED);
        LLBC_SetLastError(LLBC_OK);
    }

    // Normalize application name.
    _name = name;
    if (_name.empty())
        _name = LLBC_Directory::SplitExt(LLBC_Directory::ModuleFileName())[0];

    // Define app start failed defer.
    LLBC_Defer(if (ret != LLBC_OK) {
        _cfgPath.clear();
        _cfgType = LLBC_AppConfigType::End;
        _propCfg.RemoveAllProperties();
        _nonPropCfg.BecomeNil();

        _name.clear();

        _startThreadId = LLBC_INVALID_NATIVE_THREAD_ID;
        _startPhase = LLBC_AppStartPhase::Stopped;
    });

    // Set start phase to starting.
    _startPhase = LLBC_AppStartPhase::Starting;

    // Set start threadId.
    _startThreadId = LLBC_GetCurrentThreadId();

    // Locate config path.
    if (_cfgPath.empty())
    {
        int intCfgType;
        if (!(_cfgPath = LocateConfigPath(name, intCfgType)).empty())
            _cfgType = static_cast<LLBC_AppConfigType::ENUM>(intCfgType);
    }

    // Load config.
    LLBC_ReturnIf(!_cfgPath.empty() && LoadConfig(true) != LLBC_OK, LLBC_FAILED);

    // Hook process crash.
    if (LLBC_HookProcessCrash() != LLBC_OK)
        return LLBC_FAILED;

    // Install required signal handlers.
    // - App stop signals
    const int stopSigs[]LLBC_CFG_APP_STOP_SIGNALS;
#if LLBC_TARGET_PLATFORM_WIN32
    for (auto &stopSig : stopSigs)
        signal(stopSig, LLBC_App::HandleSignal_Stop);
#else // Non-Win32
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = LLBC_App::HandleSignal_Stop;
    for (auto &stopSig : stopSigs)
        sigaction(stopSig, &sa, nullptr);
#endif // Win32

#if LLBC_TARGET_PLATFORM_NON_WIN32
    // - App config reload signal
    const int cfgReloadSigs[]LLBC_CFG_APP_CFG_RELOAD_SIGNALS;
    sa.sa_handler = LLBC_App::HandleSignal_ReloadAppCfg;
    for (auto &cfgReloadSig : cfgReloadSigs)
        sigaction(cfgReloadSig, &sa, nullptr);
#endif // Non-Win32

    // Call OnEarlyStart event method.
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    if (OnEarlyStart(argc, argv) != LLBC_OK)
    {
        if (LLBC_GetLastError() == LLBC_ERROR_SUCCESS)
            LLBC_SetLastError(LLBC_ERROR_UNKNOWN);

        return LLBC_FAILED;
    }

    // Fire App-WillStart event to all service(s).
    FireAppPhaseChangeEvToServices(true, false, false, false);

    // Call OnStart event method.
    while (true)
    {
        bool startFinished = true;

        LLBC_SetLastError(LLBC_ERROR_SUCCESS);
        if (OnStart(argc, argv, startFinished) != LLBC_OK)
        {
            if (LLBC_GetLastError() == LLBC_ERROR_SUCCESS)
                LLBC_SetLastError(LLBC_ERROR_UNKNOWN);

            // Fire App-StartFail event to all service(s).
            FireAppPhaseChangeEvToServices(false, true, false, false);

            return LLBC_FAILED;
        }

        LLBC_BreakIf(startFinished);
        LLBC_Sleep(LLBC_CFG_APP_TRY_START_INTERVAL);
    }

    // Update start phase to Started.
    _startPhase = LLBC_AppStartPhase::Started;

    // Call OnLateStart event method.
    OnLateStart(argc, argv);

    // Fire App-StartFinish event to all service(s).
    FireAppPhaseChangeEvToServices(false, false, true, false);

    // Enter app loop.
    while (true)
    {
        // Call OnUpdate event method.
        bool runDoNothing = true;
        OnUpdate(runDoNothing);
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

    // Return ok.
    return ret = LLBC_OK;
}

void LLBC_App::Stop()
{
    // Not start judge.
    if (!IsStarted())
        return;

    // If caller thread is not application start thread, push AppStop event and return.
    if (LLBC_GetCurrentThreadId() != _startThreadId)
    {
        PushEvent(LLBC_AppEventType::Stop);
        return;
    }

    // Set start phase to Stopping.
    _startPhase = LLBC_AppStartPhase::Stopping;

    // Call OnEarlyStop event method.
    OnEarlyStop();

    // Fire App-WillStop event to all service(s).
    FireAppPhaseChangeEvToServices(false, false, false, true);

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

    // Set phase to Stopped.
    _startPhase = LLBC_AppStartPhase::Stopped;

    // Call OnLateStop event method.
    OnLateStop();

    // Uninstall required signal handlers.
    const int stopSigs[]LLBC_CFG_APP_STOP_SIGNALS;
#if LLBC_TARGET_PLATFORM_WIN32
    for (auto &stopSig : stopSigs)
        signal(stopSig, SIG_DFL);
#else // Non-Win32
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_DFL;
    for (auto &stopSig : stopSigs)
        sigaction(stopSig, &sa, nullptr);
#endif // Win32
#if LLBC_TARGET_PLATFORM_NON_WIN32
    const int cfgReloadSigs[]LLBC_CFG_APP_CFG_RELOAD_SIGNALS;
    for (auto &cfgReloadSig : cfgReloadSigs)
        sigaction(cfgReloadSig, &sa, nullptr);
#endif // Non-Win32

    // Cleanup members.
    _cfgPath.clear();
    _cfgType = LLBC_AppConfigType::End;
    _propCfg.RemoveAllProperties();
    _nonPropCfg.BecomeNil();

    _startThreadId = LLBC_INVALID_NATIVE_THREAD_ID;

    for (auto &events : _events)
        LLBC_STLHelper::RecycleContainer(events);

    _requireStop = false;

    // Cleanup llbc.
    LLBC_DoIf(_llbcLibStartupInApp, LLBC_Cleanup(); _llbcLibStartupInApp = false);
}

int LLBC_App::PushEvent(LLBC_AppEvent *ev)
{
    LLBC_LockGuard guard(_eventLock);
    if (!IsStarted() &&
        ev->evType != LLBC_AppEventType::Stop)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    _events[0].push_back(ev);

    return LLBC_OK;
}

int LLBC_App::SubscribeEvent(int evType, const LLBC_Delegate<void(const LLBC_AppEvent &)> &evHandler)
{
    // Check event type & event handler.
    if (evType < LLBC_AppEventType::LibBegin ||
        evType >= LLBC_AppEventType::LogicEnd ||
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

LLBC_String LLBC_App::LocateConfigPath(const LLBC_String &appName, int &cfgType)
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
    const auto execName = LLBC_Directory::SplitExt(LLBC_Directory::ModuleFileName())[0];
    cfgFileNames.push_back(execName);
    if (execName.endswith("_d") || execName.endswith("_debug"))
        cfgFileNames.push_back(execName.substr(0, execName.rfind('_')));

    // Execute config file locate.
    for (auto &cfgFileDir : cfgFileDirs)
    {
        for (auto &cfgFileName : cfgFileNames)
        {
            for (cfgType = LLBC_AppConfigType::Begin;
                 cfgType != LLBC_AppConfigType::End;
                 ++cfgType)
            {
                const auto cfgPath = LLBC_Directory::Join(
                    cfgFileDir, cfgFileName + LLBC_AppConfigType::GetConfigSuffix(cfgType));
                LLBC_ReturnIf(LLBC_File::Exists(cfgPath), cfgPath);
            }
        }
    }

    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
    return "";
}

int LLBC_App::LoadConfig(bool lock)
{
    // Reentry check.
    LLBC_SetErrAndReturnIf(_loadingCfg, LLBC_ERROR_REENTRY, LLBC_FAILED);

    // Lock and check again.
    if (lock)
        _cfgLock.Lock();
    LLBC_Defer(if (lock) _cfgLock.Unlock());

    // Not allow reentry.
    LLBC_SetErrAndReturnIf(_loadingCfg, LLBC_ERROR_REENTRY, LLBC_FAILED);
    // If prevent config load, return faled.
    LLBC_SetErrAndReturnIf(_preventCfgLoad != 0, LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Check config file exist or not.
    LLBC_SetErrAndReturnIf(!LLBC_File::Exists(_cfgPath), LLBC_ERROR_NOT_FOUND, LLBC_FAILED);

    _loadingCfg = true;
    LLBC_Defer(_loadingCfg = false);

    LLBC_DoIf(_cfgType == LLBC_AppConfigType::Ini, return LoadIniConfig());
    LLBC_DoIf(_cfgType == LLBC_AppConfigType::Xml, return LoadXmlConfig());
    LLBC_DoIf(_cfgType == LLBC_AppConfigType::Property, return LoadPropertyConfig());
    
    LLBC_SetLastError(LLBC_ERROR_NOT_SUPPORT);
    return LLBC_FAILED;
}

int LLBC_App::LoadIniConfig()
{
    LLBC_Ini ini;
    LLBC_ReturnIf(ini.LoadFromFile(_cfgPath) != LLBC_OK, LLBC_FAILED);

    LLBC_VariantUtil::Ini2Variant(ini, _nonPropCfg);
    return LLBC_OK;
}

int LLBC_App::LoadXmlConfig()
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

int LLBC_App::LoadPropertyConfig()
{
    return _propCfg.LoadFromFile(_cfgPath);
}

void LLBC_App::HandleEvents(bool &doNothing)
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
        if (evType >= LLBC_AppEventType::LibBegin &&
            evType < LLBC_AppEventType::LibEnd)
        {
            auto libHandlerIt = _libEventHandlers.find(evType);
            if (libHandlerIt != _libEventHandlers.end())
                libHandlerIt->second(*ev);
        }

        auto logicHandlerIt = _logicEventHandlers.find(evType);
        if (logicHandlerIt != _logicEventHandlers.end())
            logicHandlerIt->second(*ev);

        if (evType == LLBC_AppEventType::Stop)
            return;
    }
}

void LLBC_App::HandleEvent_ReloadAppCfg(const LLBC_AppEvent &ev)
{
    ReloadConfig();
}

void LLBC_App::HandleEvent_Stop(const LLBC_AppEvent &ev)
{
    _requireStop = true;
}

void LLBC_App::HandleSignal_Stop(int sig)
{
    ThisApp()->PushEvent(LLBC_AppEventType::Stop);
}

void LLBC_App::HandleSignal_ReloadAppCfg(int sig)
{
    ThisApp()->PushEvent(LLBC_AppEventType::ReloadApplicationConfig);
}

void LLBC_App::FireAppPhaseChangeEvToServices(bool earlyStart,
                                              bool startFail,
                                              bool startFinish,
                                              bool earlyStop)
{
    for (auto svcItem : _services.GetAllIndexedByIdServices())
        svcItem.second->Push(
            LLBC_SvcEvUtil::BuildAppPhaseEv(earlyStart, startFail, startFinish, earlyStop));
}

__LLBC_NS_END

