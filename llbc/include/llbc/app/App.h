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

#pragma once

#include "llbc/comm/Comm.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_Packet;
class LLBC_Component;
class LLBC_Service;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The application config type enumeration.
 */
class LLBC_EXPORT LLBC_AppConfigType
{
public:
    enum ENUM
    {
        Begin = 0,
        Ini = Begin,
        Xml,
        Property,

        End
    };

    /**
     * Get appliation config suffixes.
     * @param[in] cfgType - the application config type.
     * @return const LLBC_Strings & - the config suffixex.
     */
    static const LLBC_Strings &GetConfigSuffixes(int cfgType);

    /**
     * Get config type. 
     * @param cfgSuffix - the config suffix, case insensitive.
     * @return ENUM - the config type, if unsupported application config, return End.
     */
    static ENUM GetConfigType(const LLBC_String &cfgSuffix);
};

/**
 * \brief The application event type enumeration.
 */
class LLBC_AppEventType
{
public:
    enum ENUM
    {
        // llbc library event enumeration range[0,100).
        LibBegin = 0,
        Stop = LibBegin, // Application stop event.
        Reload, // Application reload event.
        LibEnd = 100,

        // Logic event enumeration range[100, 100000).
        LogicBegin = LibEnd,
        LogicEnd = 100000
    };
};

/**
 * \brief The application event class encapsulation.
 */
struct LLBC_AppEvent
{
    int evType;
    LLBC_Variant evData;

    explicit LLBC_AppEvent(int evType);
};

/**
 * \brief The application start phase encapsulation.
 */
class LLBC_AppStartPhase
{
public:
    enum ENUM
    {
        Begin = 0,
        Stopped = Begin, // Application stopped.
        Starting, // Application starting.
        Started, // Application started.
        Stopping, // Application stopping.

        End,
    };
};

/**
 * \brief The application interface class encapsulation.
 *        Note: Please call Start/Wait/Stop method at main thread.
 */
class LLBC_EXPORT LLBC_App
{
public:
    LLBC_App();
    virtual ~LLBC_App();

public:
    /**
     * Application will start event method, when application will start, will call this event method.
     * @param[in] argc - the application startup arguments count.
     * @param[in] argv - the application startup arguments.
     * @return int - return 0 if start success, otherwise return -1.
     */
    virtual int OnEarlyStart(int argc, char *argv[], bool &earlyStartFinished) { return LLBC_OK; }

    /**
     * Application start event method, please override this method in your project.
     * @param[in] argc           - the application startup arguments count.
     * @param[in] argv           - the application startup arguments.
     * @param[out] startFinished - if startup finished set true, otherwise set false, default is true.
     * @return int - return 0 if start success, otherwise return -1.
     */
    virtual int OnStart(int argc, char *argv[], bool &startFinished) = 0;

    /**
     * Application start finish event method, when application start finish, will call this event method.
     * @param[in] argc - the application startup arguments count.
     * @param[in] argv - the application startup arguments.
     */
    virtual void OnLateStart(int argc, char *argv[]) {  }

    /**
     * Application will stop event method, when application will stop, will call this event method.
     */
    virtual void OnEarlyStop(bool &earlyStopFinished) {  }

    /**
     * Application stop event method, please override this method in your project.
     * @param[in] stopFinished - application stop finished flag, if stop finished, set to true, otherwise set to false.
     */
    virtual void OnStop(bool &stopFinished) = 0;

    /**
     * Application stop finish event method, when application stop finish, will call this event method.
     */
    virtual void OnLateStop() {  }

    /**
     * Application main-loop event method, when application running, will call this event method per-tick.
     */
    virtual void OnUpdate() {  }

    /**
     * Application reloaded event method, please override this method in your project.
     */
    virtual void OnReload() {  }

public:
    /**
     * Get this application.
     * @return App * - this application.
     */
    template <typename App>
    static App *ThisApp() { return static_cast<App *>(_thisApp); }
    static LLBC_App *ThisApp() { return _thisApp; }

public:
    /**
     * Start application.
     * @param[in] argc - the application startup argument number.
     * @param[in] argv - the application startup arguments.
     * @param[in] name - the application name, default is executable file name(has been trim extension name).
     * @return int - return 0 if start success, otherwise return -1.
     */
    virtual int Start(int argc, char *argv[], const LLBC_String &name = "");

    /**
     * Stop application.
     */
    virtual void Stop();

    /**
     * Check application start phase.
     */
    bool IsStarting() const;
    bool IsStarted() const;
    bool IsStopping() const;
    bool IsStopped() const;

    /**
     * Get application start phase.
     * @return int - the application start phase.
     */
    int GetStartPhase() const { return _startPhase; }

    /**
     * Get application FPS.
     * @return int - the application FPS.
     */
    int GetFPS() const;

    /**
     * Set application FPS.
     * @return int - return 0 if success, ohterwise return -1.
     */
    int SetFPS(int fps);

    /**
     * Get application frame interval, in milli-seconds.
     * @return int - return 0 if success, otherwise return -1.
     */
    int GetFrameInterval() const;

public:
    /**
     * Check have application config or not.
     * @return bool - return true if has config, otherwise return false.
     */
    bool HasConfig() const;

    /**
     * Get application config.
     * @return LLBC_Variant - the application config.
     */
    LLBC_Variant GetConfig() const;

    /**
     * Get application config(thread unsafety).
     * @return const LLBC_Variant & - the application config.
     */
    const LLBC_Variant &GetConfigUnsafe() const { return _cfg; }

    /**
     * Get application config type.
     * @return LLBC_AppConfigType::ENUM - application config type.
     */
    LLBC_AppConfigType::ENUM GetConfigType() const { return _cfgType; }

    /**
     * Get application config path.
     * @return const LLBC_String & - the application config path.
     */
    LLBC_String GetConfigPath() const;

    /**
     * Set application config path.
     * @param[in] cfgPath - the config path.
     * @return int - return 0 if success, other return -1.
     */
    int SetConfigPath(const LLBC_String &cfgPath);

    /**
     * Reload application.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Reload();

    /**
     * Prevent application reload.
     * @return int - return 0 if success, otherwise return -1.
     */
    int PreventReload();

    /**
     * Cancel prevent application reload.
     * @return int - return 0 if success, otherwise return -1.
     */
    int CancelPreventReload();

public:
    /**
     * Push application event by event type.
     * @param[in] evType - the event type.
     * @return int - return 0 if success, otherwise return -1.
     */
    int PushEvent(int evType);

    /**
     * Push application event by event object.
     * @param[in] ev - the event object.
     * @return int - return 0 if success, otherwise return -1.
     */
    int PushEvent(LLBC_AppEvent *ev);

    /**
     * Subscribe application event handler.
     * @param[in] evType    - will subscribe event type.
     * @param[in] evHandler - event handler.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SubscribeEvent(int evType, const LLBC_Delegate<void(const LLBC_AppEvent &)> &evHandler);

public:
    /**
     * Get application name.
     * @return const LLBC_String & - application name.
     */
    const LLBC_String &GetName() const { return _name; }

    /**
     * Get startup arguments.
     * @return const LLBC_StartArgs & - the startup arguments(llbc library wrapped object const reference).
     */
    const LLBC_StartArgs &GetStartArgs() const { return _startArgs; }

public:
    /**
     * Get service by service Id.
     * @param[in] id - service Id.
     * @return LLBC_Service * - service.
     */
    LLBC_Service *GetService(int id) const;
    /**
     * Get service by service name.
     * @param[in] name - service name.
     * @return LLBC_Service * - service.
     */
    LLBC_Service *GetService(const LLBC_CString &name) const;

    /**
     * Stop service by service Id.
     * @param[in] id          - service Id.
     * @param[in] del         - delete service or not, default is true.
     * @param[in] destroyComp - destroy service components or not, default is false.
     * @return int - return 0 if success, otherwise return -1.
     */
    int StopService(int id, bool del = true, bool destroyComp = false);
    /**
     * Stop service by service name.
     * @param[in] name        - service name.
     * @param[in] del         - delete service or not, default is true.
     * @param[in] destroyComp - destroy service components or not, default is false.
     * @return int - return 0 if success, otherwise return -1.
     */
    int StopService(const LLBC_CString &name, bool del = true, bool destroyComp = false);

private:
    /**
     * Locate application config path.
     * @param[in] appName  - the application name.
     * @param[out] cfgType - the application config type.
     * @return LLBC_String - the application config path, return empty string if failed.
     */
    static LLBC_String LocateConfigPath(const LLBC_String &appName, int &cfgType);

    /**
     * Application reload implement method.
     * @param[in] checkAppStarted - Check Application started flag.
     * @param[in] callEvMeth      - Call event method flag.
     * @return int - return 0 if success, otherwise return -1.
     */
    int ReloadImpl(bool checkAppStarted, bool callEvMeth);

    /**
     * Load/Reload application config.
     * @return int - return 0 if success, otherwise return -1.
     */
    int ReloadConfig();
    int ReloadIniConfig();
    int ReloadXmlConfig();
    int ReloadPropertyConfig();

private:
    void HandleEvents();
    void HandleEvent_Stop(const LLBC_AppEvent &ev);
    void HandleEvent_Reload(const LLBC_AppEvent &ev);

    static void HandleSignal_Stop(int sig);
    static void HandleSignal_Reload(int sig);

private:
    void FireAppPhaseChangeEvToServices(bool willStart,
                                        bool startFailed,
                                        bool startFinished,
                                        bool willStop);

private:
    // Application core data members.
    static LLBC_App *_thisApp; //!!! application singleton instance.
    LLBC_String _name; // Application name.
    volatile int _fps; // Application FPS.
    volatile int _startPhase; // Application start phase.
    LLBC_StartArgs _startArgs; // Application start args.
    volatile LLBC_ThreadId _startThreadId; // Call Start() thread Id.
    bool _llbcLibStartupInApp; // llbc library startup in App flag.
    bool _requireStop; // Stop flag, when App processed Stop event, will set to true.
    LLBC_ServiceMgr &_services; // Service manager.

    // Load/Reload data members.
    volatile int _loading; // Loading flag.
    mutable LLBC_SpinLock _loadLock; // Load lock.
    LLBC_Variant _cfg; // Config.
    LLBC_String _cfgPath; // Application config path.
    LLBC_AppConfigType::ENUM _cfgType; // Application config type.

    // Event & Event handle data members.
    LLBC_SpinLock _eventLock; // event lock.
    std::vector<LLBC_AppEvent *> _events[2]; // Waiting for handle events.
    std::map<int, LLBC_Delegate<void(const LLBC_AppEvent &)> > _libEventHandlers; // Library level event handlers.
    std::map<int, LLBC_Delegate<void(const LLBC_AppEvent &)> > _logicEventHandlers; // Logic level event handlers.
};

__LLBC_NS_END

#include "llbc/app/AppInl.h"


