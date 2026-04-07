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

static const LLBC_NS LLBC_Strings __cfgSuffixes_Ini = { ".ini" };
static const LLBC_NS LLBC_Strings __cfgSuffixes_Xml = { ".xml" };
static const LLBC_NS LLBC_Strings __cfgSuffixes_Property = { ".cfg", ".properties" };
static const LLBC_NS LLBC_Strings __cfgSuffixes_Unknown = { "" };

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

const LLBC_Strings &LLBC_AppConfigType::GetConfigSuffixes(int cfgType)
{
    if (cfgType == Ini)
        return LLBC_INL_NS __cfgSuffixes_Ini;
    else if (cfgType == Xml)
        return LLBC_INL_NS __cfgSuffixes_Xml;
    else if (cfgType == Property)
        return LLBC_INL_NS __cfgSuffixes_Property;
    else
        return LLBC_INL_NS __cfgSuffixes_Unknown;
}

auto LLBC_AppConfigType::GetConfigType(const LLBC_String &cfgSuffix) -> ENUM
{
    const auto nmlCfgSuffix = cfgSuffix.strip().tolower();
    LLBC_Foreach(LLBC_INL_NS __cfgSuffixes_Ini, LLBC_ReturnIf(nmlCfgSuffix == item, Ini));
    LLBC_Foreach(LLBC_INL_NS __cfgSuffixes_Xml, LLBC_ReturnIf(nmlCfgSuffix == item, Xml));
    LLBC_Foreach(LLBC_INL_NS __cfgSuffixes_Property, LLBC_ReturnIf(nmlCfgSuffix == item, Property));

    return End;
}

LLBC_App *LLBC_App::_thisApp = nullptr;

LLBC_App::LLBC_App()
: _fps(LLBC_CFG_APP_DFT_FPS)
, _startPhase(LLBC_AppStartPhase::Stopped)
, _startThreadId(LLBC_INVALID_NATIVE_THREAD_ID)
, _llbcLibStartupInApp(false)
, _requireStop(false)
, _services(*LLBC_ServiceMgrSingleton)

, _loading(0)
, _cfgType(LLBC_AppConfigType::End)
{
    llbc_assert(!_thisApp && "Not allow create more than one application object");

    _thisApp = this;
    _libEventHandlers.emplace(LLBC_AppEventType::Stop,
        LLBC_Delegate<void(const LLBC_AppEvent &)>(this, &LLBC_App::HandleEvent_Stop));
    _libEventHandlers.emplace(LLBC_AppEventType::Reload,
        LLBC_Delegate<void(const LLBC_AppEvent &)>(this, &LLBC_App::HandleEvent_Reload));
}

LLBC_App::~LLBC_App()
{
    llbc_assert(!IsStarted() && "Please stop application before destruct");
    LLBC_DoIf(_llbcLibStartupInApp, LLBC_Cleanup());
    _thisApp = nullptr;
}

int LLBC_App::SetFPS(int fps)
{
    if (fps != static_cast<int>(LLBC_INFINITE) &&
        (fps < LLBC_CFG_APP_MIN_FPS || fps > LLBC_CFG_APP_MAX_FPS))
    {
        LLBC_SetLastError(LLBC_ERROR_LIMIT);
        return LLBC_FAILED;
    }

    _fps = fps;

    return LLBC_OK;
}

int LLBC_App::GetFrameInterval() const
{
    const auto fps = _fps;
    return fps != static_cast<int>(LLBC_INFINITE) ? 1000 / fps : 0;
}

bool LLBC_App::HasConfig() const
{
    LLBC_LockGuard guard(_loadLock);
    return _cfgType != LLBC_AppConfigType::End;
}

LLBC_Variant LLBC_App::GetConfig() const
{
    LLBC_LockGuard guard(_loadLock);
    LLBC_Variant cfg = _cfg;

    return cfg;
}

LLBC_String LLBC_App::GetConfigPath() const
{
    LLBC_LockGuard guard(_loadLock);
    LLBC_String cfgPath(_cfgPath.c_str(), _cfgPath.size());

    return cfgPath;
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
    LLBC_LockGuard guard(_loadLock);
    LLBC_SetErrAndReturnIf(IsStarted(), LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Save config info.
    _cfgType = cfgType;
    _cfgPath = cfgPath;

    return LLBC_OK;
}

int LLBC_App::Reload()
{
    return ReloadImpl(true, true);
}

int LLBC_App::PreventReload()
{
    // Not allow reload if application not start.
    LLBC_SetErrAndReturnIf(!IsStarted(), LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Increment loading flag.
    LLBC_AtomicFetchAndAdd(&_loading, 1);

    return LLBC_OK;
}

int LLBC_App::CancelPreventReload()
{
    // Not allow reload if application not start.
    LLBC_SetErrAndReturnIf(!IsStarted(), LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);

    // Decrement loading flag.
    LLBC_AtomicFetchAndSub(&_loading, 1);

    return LLBC_OK;
}

int LLBC_App::Start(int argc, char *argv[], const LLBC_String &name)
{
    // Multi application check.
    LLBC_SetErrAndReturnIf(_thisApp != this, LLBC_ERROR_REPEAT, LLBC_FAILED);
    // Reentry check.
    LLBC_SetErrAndReturnIf(!IsStopped(), LLBC_ERROR_REENTRY, LLBC_FAILED);

    // Startup llbc framework.
    // Note: _llbcLibStartupInApp flag is used to determine whether to call LLBC_Cleanup() in LLBC_App::~LLBC_App().
    //       This ensures that after LLBC_App::Start() is called(regardless of success or failure), developers can
    //       safely use other llbc framework abilities, such as LLBC_GetLastError().
    if (LLBC_Startup() != LLBC_OK)
    {
        LLBC_ReturnIf(LLBC_GetLastError() != LLBC_ERROR_REENTRY, LLBC_FAILED);

        LLBC_SetLastError(LLBC_OK);
        _llbcLibStartupInApp = false;
    }
    else
    {
        _llbcLibStartupInApp = true;
    }

    // Parse startup arguments.
    // Note: If App start failed, don't clear start argument member.
    LLBC_ReturnIf(_startArgs.Parse(argc, argv) != LLBC_OK, LLBC_FAILED);

    // Declare ret variable.
    int ret = LLBC_OK;

    // Normalize application name.
    _name = name;
    if (_name.empty())
        _name = LLBC_Directory::SplitExt(LLBC_Directory::ModuleFileName())[0];
    LLBC_Defer(LLBC_DoIf(ret != LLBC_OK, _name.clear()));

    // Set start phase to starting.
    _startPhase = LLBC_AppStartPhase::Starting;
    LLBC_Defer(LLBC_DoIf(ret != LLBC_OK, _startPhase = LLBC_AppStartPhase::Stopped));
    // Set start threadId.
    _startThreadId = LLBC_GetCurrentThreadId();
    LLBC_Defer(LLBC_DoIf(ret != LLBC_OK, _startThreadId = LLBC_INVALID_NATIVE_THREAD_ID));

    // Locate config path & Load config.
    if (_cfgPath.empty())
    {
        int cfgType;
        if (!(_cfgPath = LocateConfigPath(_name, cfgType)).empty())
            _cfgType = static_cast<LLBC_AppConfigType::ENUM>(cfgType);
    }
    LLBC_ReturnIf(_cfgType != LLBC_AppConfigType::End && (ret = ReloadImpl(false, false)) != LLBC_OK, ret);
    LLBC_Defer(
        LLBC_DoIf(
            ret != LLBC_OK,
            _cfg.BecomeNil()
            // , _cfgPath.clear() // Don't clear config path if application start failed.
            // , _cfgType = LLBC_AppConfigType::End // Don't reset config type if application start failed.
        )
    );

    // Enable crash handle(Ignore Not-Impl error).
    ret = LLBC_EnableCrashHandle();
    if (ret != LLBC_OK)
    {
        if (LLBC_GetLastError() == LLBC_ERROR_NOT_IMPL)
            ret = LLBC_OK;
        else
            return ret;
    }
    LLBC_Defer(LLBC_DoIf(ret != LLBC_OK, LLBC_DisableCrashHandle()));

    // Install required signal handlers.
    // - App stop signals.
    static constexpr int stopSigs[]LLBC_CFG_APP_STOP_SIGNALS;
    for (int sigIdx = 0; sigIdx < static_cast<int>(std::size(stopSigs)); ++sigIdx)
    {
        LLBC_ContinueIf((ret = LLBC_SetSignalHandler(stopSigs[sigIdx], LLBC_App::HandleSignal_Stop)) == LLBC_OK);

        for (int rollbackSigIdx = sigIdx - 1; rollbackSigIdx >= 0; --rollbackSigIdx)
            LLBC_SetSignalHandler(stopSigs[rollbackSigIdx], nullptr);
        return ret;
    }
    LLBC_Defer(LLBC_DoIf(ret != LLBC_OK, LLBC_Foreach(stopSigs, LLBC_SetSignalHandler(item, nullptr))));
    #if LLBC_TARGET_PLATFORM_NON_WIN32
    // - App reload signals.
    static constexpr int cfgReloadSigs[]LLBC_CFG_APP_RELOAD_SIGNALS;
    for (int sigIdx = 0; sigIdx < static_cast<int>(std::size(cfgReloadSigs)); ++sigIdx)
    {
        LLBC_ContinueIf((ret = LLBC_SetSignalHandler(cfgReloadSigs[sigIdx], LLBC_App::HandleSignal_Reload)) == LLBC_OK);

        for (int rollbackSigIdx = sigIdx - 1; rollbackSigIdx >= 0; --rollbackSigIdx)
            LLBC_SetSignalHandler(cfgReloadSigs[rollbackSigIdx], nullptr);
        return ret;
    }
    LLBC_Defer(LLBC_DoIf(ret != LLBC_OK, LLBC_Foreach(cfgReloadSigs, LLBC_SetSignalHandler(item, nullptr))));
    #endif // Non-Win32

    // Call OnEarlyStart event method.
    LLBC_TimerScheduler *timerScheduler =
        reinterpret_cast<LLBC_TimerScheduler *>(__LLBC_GetLibTls()->coreTls.timerScheduler);
    while (true)
    {
        bool earlyStartFinished = true;
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);
        if (OnEarlyStart(argc, argv, earlyStartFinished) != LLBC_OK)
        {
            if (LLBC_GetLastError() == LLBC_ERROR_SUCCESS)
                LLBC_SetLastError(LLBC_ERROR_APP_EARLY_START_FAILED);

            ret = LLBC_FAILED;
            return ret;
        }

        LLBC_BreakIf(earlyStartFinished);

        LLBC_Sleep(LLBC_CFG_APP_TRY_START_INTERVAL);
        timerScheduler->Update();
    }
    LLBC_Defer(LLBC_DoIf(ret != LLBC_OK,  OnLateStop()));

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
                LLBC_SetLastError(LLBC_ERROR_APP_START_FAILED);

            // Fire App-StartFail event to all service(s).
            FireAppPhaseChangeEvToServices(false, true, false, false);

            ret = LLBC_FAILED;
            return ret;
        }

        LLBC_BreakIf(startFinished);

        LLBC_Sleep(LLBC_CFG_APP_TRY_START_INTERVAL);
        timerScheduler->Update();
    }

    // Update start phase to Started.
    _startPhase = LLBC_AppStartPhase::Started;

    // Call OnLateStart event method.
    OnLateStart(argc, argv);

    // Fire App-StartFinish event to all service(s).
    FireAppPhaseChangeEvToServices(false, false, true, false);

    // Set ret to OK.
    ret = LLBC_OK;

    // Enter app loop.
    while (true)
    {
        sint64 begRunTime = LLBC_GetMilliseconds();

        // Process received signals.
        LLBC_ProcessReceivedSignals();

        // Call OnUpdate event method.
        OnUpdate();
        // Handle events.
        HandleEvents();
        // Update timer scheduler.
        timerScheduler->Update();

        // If require stop, execute stop.
        if (_requireStop)
        {
            Stop();
            return ret;
        }

        // Execute sleep, if need.
        const auto frameInterval = GetFrameInterval();
        const sint64 elapsed = LLBC_GetMilliseconds() - begRunTime;
        if (elapsed >= 0 && elapsed < frameInterval)
            LLBC_Sleep(static_cast<int>(frameInterval - elapsed));
    }
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
    LLBC_TimerScheduler *timerScheduler =
        reinterpret_cast<LLBC_TimerScheduler *>(__LLBC_GetLibTls()->coreTls.timerScheduler);
    while (true)
    {
        bool earlyStopFinished = true;
        OnEarlyStop(earlyStopFinished);
        LLBC_BreakIf(earlyStopFinished);

        LLBC_Sleep(LLBC_CFG_APP_TRY_STOP_INTERVAL);
        timerScheduler->Update();
    }

    // Fire App-WillStop event to all service(s).
    FireAppPhaseChangeEvToServices(false, false, false, true);

    // Stop all services.
    _services.StopAll(true, true);

    // Stop app.
    while (true)
    {
        bool stopFinished = true;
        OnStop(stopFinished);
        LLBC_BreakIf(stopFinished);

        LLBC_Sleep(LLBC_CFG_APP_TRY_STOP_INTERVAL);
        timerScheduler->Update();
    }

    // Call OnLateStop event method.
    OnLateStop();

    // Set phase to Stopped.
    _startPhase = LLBC_AppStartPhase::Stopped;

    // Uninstall required signal handlers.
    constexpr int stopSigs[]LLBC_CFG_APP_STOP_SIGNALS;
    LLBC_Foreach(stopSigs, LLBC_SetSignalHandler(item, nullptr));
    #if LLBC_TARGET_PLATFORM_NON_WIN32
    constexpr int cfgReloadSigs[]LLBC_CFG_APP_RELOAD_SIGNALS;
    LLBC_Foreach(cfgReloadSigs, LLBC_SetSignalHandler(item, nullptr));
    #endif // Non-Win32

    // Disable handle crash.
    LLBC_DisableCrashHandle();

    // Cleanup members.
    _cfgPath.clear();
    _cfgType = LLBC_AppConfigType::End;
    _cfg.BecomeNil();

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
    // -> <module file directory[readLink == false]>
    // -> <module file directory[readLink == true ]>
    //    -> <cwd>/Config -> config -> Conf -> conf -> Cfg -> cfg
    //    -> <module file directory[readLink == false]>/Config -> config -> Conf -> conf -> Cfg -> cfg
    //    -> <module file directory[readLink == true ]>/Config -> config -> Conf -> conf -> Cfg -> cfg
    //       -> <cwd>/../Config -> config -> Conf -> conf -> Cfg -> cfg
    //       -> <module file directory[readLink == false]>/../Config -> config -> Conf -> conf -> Cfg -> cfg
    //       -> <module file directory[readLink == true ]>/../Config -> config -> Conf -> conf -> Cfg -> cfg
    //          -> <cwd>/..
    //          -> <module file directory[readLink == false]>/..
    //          -> <module file directory[readLink == true ]>/..
    //
    // Application config filename locate order:
    // -> app_name
    //    -> stripped _d/_debug suffix app_name
    //       -> executable_name[readLink == false]
    //          -> stripped _d/_debug suffix executable_name
    //             -> executable_name[readLink == true]
    //                -> stripped _d/_debug suffix executable_name
    //
    // Application config type locate order:
    // -> .ini
    //    -> .xml
    //       -> .cfg

    // Build config directories.
    // -> <cwd>
    // -> <module file directory>
    LLBC_Strings cfgFileDirs{ LLBC_Directory::CurDir(),
                              LLBC_Directory::ModuleFileDir(false) };
    if (LLBC_Directory::ModuleFileDir(true) != LLBC_Directory::ModuleFileDir(false))
        cfgFileDirs.push_back(LLBC_Directory::ModuleFileDir(true));

    // -> <cwd>/xxxxx
    // -> <module file directory>/xxxxx
    const char *interDirs[] = {"Config", "config", "Conf", "conf", "Cfg", "cfg"};
    for (auto &cfgDir : interDirs)
        cfgFileDirs.push_back(LLBC_Directory::Join(LLBC_Directory::CurDir(), cfgDir));
    for (auto &cfgDir : interDirs)
        cfgFileDirs.push_back(LLBC_Directory::Join(LLBC_Directory::ModuleFileDir(false), cfgDir));
    if (LLBC_Directory::ModuleFileDir(true) != LLBC_Directory::ModuleFileDir(false))
    {
        for (auto &cfgDir : interDirs)
            cfgFileDirs.push_back(LLBC_Directory::Join(LLBC_Directory::ModuleFileDir(true), cfgDir));
    }
    // -> <cwd>/../xxxxx
    // -> <module file directory>/../xxxxx
    for (auto &cfgDir : interDirs)
        cfgFileDirs.push_back(LLBC_Directory::Join(LLBC_Directory::CurDir(), "..", cfgDir));
    for (auto &cfgDir : interDirs)
        cfgFileDirs.push_back(LLBC_Directory::Join(LLBC_Directory::ModuleFileDir(false), "..", cfgDir));
    if (LLBC_Directory::ModuleFileDir(true) != LLBC_Directory::ModuleFileDir(false))
    {
        for (auto &cfgDir : interDirs)
            cfgFileDirs.push_back(LLBC_Directory::Join(LLBC_Directory::ModuleFileDir(true), "..", cfgDir));
    }
    // -> <cwd>/..
    // -> <module file directory>/..
    cfgFileDirs.push_back(LLBC_Directory::Join(LLBC_Directory::CurDir(), ".."));
    cfgFileDirs.push_back(LLBC_Directory::Join(LLBC_Directory::ModuleFileDir(false), ".."));
    if (LLBC_Directory::ModuleFileDir(true) != LLBC_Directory::ModuleFileDir(false))
        cfgFileDirs.push_back(LLBC_Directory::Join(LLBC_Directory::ModuleFileDir(true), ".."));

    // Building config filenames.
    LLBC_Strings cfgFileNames{appName};
    LLBC_String strippedDbgSuffixAppName = appName;
    if (appName.endswith("_d") || appName.endswith("_debug"))
    {
        strippedDbgSuffixAppName = appName.substr(0, appName.rfind('_'));
        cfgFileNames.push_back(strippedDbgSuffixAppName);
    }
    for (auto &readLinkFlag : { false, true })
    {
        const auto execName =
            LLBC_Directory::SplitExt(LLBC_Directory::ModuleFileName(readLinkFlag))[0];
        if (execName != appName &&
            execName != strippedDbgSuffixAppName)
            cfgFileNames.push_back(execName);
        if (execName.endswith("_d") || execName.endswith("_debug"))
        {
            const auto strippedDbgSuffixExecName = execName.substr(0, execName.rfind('_'));
            if (strippedDbgSuffixExecName != strippedDbgSuffixAppName)
                cfgFileNames.push_back(strippedDbgSuffixExecName);
        }
    }

    // Execute config file locate.
    for (auto &cfgFileDir : cfgFileDirs)
    {
        for (auto &cfgFileName : cfgFileNames)
        {
            for (cfgType = LLBC_AppConfigType::Begin;
                 cfgType != LLBC_AppConfigType::End;
                 ++cfgType)
            {
                for (auto &cfgSuffix : LLBC_AppConfigType::GetConfigSuffixes(cfgType))
                {
                    auto cfgPath = LLBC_Directory::Join(cfgFileDir, cfgFileName + cfgSuffix);
                    LLBC_ReturnIf(LLBC_File::Exists(cfgPath), cfgPath);
                }
            }
        }
    }

    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
    return "";
}

int LLBC_App::ReloadImpl(bool checkAppStarted, bool callEvMeth)
{
    // Execute reload logic.
    {
        // Lock and check.
        LLBC_LockGuard guard(_loadLock);
        LLBC_SetErrAndReturnIf(checkAppStarted && !IsStarted(), LLBC_ERROR_NOT_ALLOW, LLBC_FAILED);
        LLBC_SetErrAndReturnIf(_loading > 0, LLBC_ERROR_REENTRY, LLBC_FAILED);

        // - Reload detail: Reload logger mgr(ignore error).
        if (LLBC_LoggerMgrSingleton->IsInited())
            LLBC_LoggerMgrSingleton->Reload();

        // - Reload detail: Reload config.
        LLBC_AtomicFetchAndAdd(&_loading, 1);
        LLBC_Defer(LLBC_AtomicFetchAndSub(&_loading, 1));
        if (_cfgType != LLBC_AppConfigType::End && ReloadConfig() != LLBC_OK)
        {
            OnReloadFailed();
            if (callEvMeth)
            {
                const auto reloadErrNo = LLBC_GetLastError();
                const auto reloadSubErrNo = LLBC_GetSubErrorNo();
                for (auto &svcId : _services.GetAllServiceIds())
                {
                    auto svc = _services.GetService(svcId);
                    LLBC_ContinueIf(!svc);

                    svc->Push(LLBC_SvcEvUtil::BuildAppReloadFailedEv(reloadErrNo, reloadSubErrNo));
                }
            }
            
            return LLBC_FAILED;
        }

        // - Reload application fps.
        for (auto cfgItem : _cfg.AsDict())
        {
            if (_cfgType == LLBC_AppConfigType::Ini)
            {
                auto sectionName = cfgItem.first.AsStr().tolower();
                if (sectionName != "app" && sectionName != "application")
                    break;

                for (auto cfgSecItem : cfgItem.second.AsDict())
                {
                    if (cfgSecItem.first.AsStr().tolower() == "fps")
                    {
                        SetFPS(cfgSecItem.second);
                        break;
                    }
                }

                break;
            }

            if (cfgItem.first.AsStr().tolower() == "fps")
            {
                const auto &fps =
                    _cfgType == LLBC_AppConfigType::Xml ?
                        cfgItem.second[LLBC_XMLKeys::Value] : cfgItem.second;
                SetFPS(fps);
                break;
            }
        }

        // Note: Execute other application level resource(s) reload.
        // ... ...
    }

    // Call reload event method(released _loadLock).
    if (callEvMeth)
    {
        OnReload();
        for (auto &svcId : _services.GetAllServiceIds())
        {
            auto svc = _services.GetService(svcId);
            LLBC_ContinueIf(!svc);

            svc->Push(LLBC_SvcEvUtil::BuildAppReloadedEv(_cfgType, _cfg));
        }
    }

    return LLBC_OK;
}

int LLBC_App::ReloadConfig()
{
    // Check config file exist or not.
    LLBC_SetErrAndReturnIf(!LLBC_File::Exists(_cfgPath), LLBC_ERROR_NOT_FOUND, LLBC_FAILED);

    // Reload llbc framework supported config type files.
    LLBC_DoIf(_cfgType == LLBC_AppConfigType::Ini, return ReloadIniConfig());
    LLBC_DoIf(_cfgType == LLBC_AppConfigType::Xml, return ReloadXmlConfig());
    LLBC_DoIf(_cfgType == LLBC_AppConfigType::Property, return ReloadPropertyConfig());
    
    LLBC_SetLastError(LLBC_ERROR_NOT_SUPPORT);
    return LLBC_FAILED;
}

int LLBC_App::ReloadIniConfig()
{
    LLBC_Ini ini;
    LLBC_ReturnIf(ini.LoadFromFile(_cfgPath) != LLBC_OK, LLBC_FAILED);

    LLBC_VariantUtil::Ini2Variant(ini, _cfg);
    return LLBC_OK;
}

int LLBC_App::ReloadXmlConfig()
{
    LLBC_TINYXML2_NS XMLDocument doc;
    const auto xmlLoadRet = doc.LoadFile(_cfgPath.c_str());
    if (xmlLoadRet != LLBC_TINYXML2_NS XML_SUCCESS)
    {
        LLBC_String customErrStr;
        customErrStr.format("load xml config file failed, file:%s, errno(tinyxml2):%d, error str:%s",
                            _cfgPath.c_str(), xmlLoadRet, doc.ErrorStr());
        LLBC_SetLastError(LLBC_ERROR_FORMAT, customErrStr.c_str());
        return LLBC_FAILED;
    }

    LLBC_VariantUtil::Xml2Variant(doc, _cfg);
    return LLBC_OK;
}

int LLBC_App::ReloadPropertyConfig()
{
    _cfg.BecomeNil();
    return LLBC_Properties::LoadFromFile(_cfgPath, _cfg);
}

void LLBC_App::HandleEvents()
{
    _eventLock.Lock();
    _events[0].swap(_events[1]);
    _eventLock.Unlock();

    auto &events = _events[1];
    if (events.empty())
        return;

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

void LLBC_App::HandleEvent_Reload(const LLBC_AppEvent &ev)
{
    Reload();
}

void LLBC_App::HandleEvent_Stop(const LLBC_AppEvent &ev)
{
    _requireStop = true;
}

void LLBC_App::HandleSignal_Stop(int recvThreadId, int sig, int sigVal)
{
    LLBC_UNUSED_PARAM(recvThreadId);
    LLBC_UNUSED_PARAM(sigVal);

    ThisApp()->PushEvent(LLBC_AppEventType::Stop);
}

void LLBC_App::HandleSignal_Reload(int recvThreadId, int sig, int sigVal)
{
    LLBC_UNUSED_PARAM(recvThreadId);
    LLBC_UNUSED_PARAM(sigVal);

    ThisApp()->PushEvent(LLBC_AppEventType::Reload);
}

void LLBC_App::FireAppPhaseChangeEvToServices(bool willStart,
                                              bool startFailed,
                                              bool startFinished,
                                              bool willStop)
{
    LLBC_Variant cfg;
    int cfgType = LLBC_AppConfigType::End;
    if (startFinished)
    {
        cfg = _cfg;
        cfgType = _cfgType;
    }

    for (auto &svcId : _services.GetAllServiceIds())
    {
        auto svc = _services.GetService(svcId);
        LLBC_ContinueIf(!svc);

        svc->Push(
            LLBC_SvcEvUtil::BuildAppPhaseEv(
                willStart, startFailed, startFinished, willStop, cfgType, cfg));
    }
}

__LLBC_NS_END

