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

#ifndef __LLBC_APP_IAPPLICATION_H__
#define __LLBC_APP_IAPPLICATION_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/comm/Comm.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_Packet;
class LLBC_IComponent;
class LLBC_IService;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The application interface class encapsulation.
 *        Note: Please call Start/Wait/Stop method at main thread.
 */
class LLBC_EXPORT LLBC_IApplication
{
public:
    LLBC_IApplication();
    virtual ~LLBC_IApplication();

public:
    /**
     * Application start event method, please override this method in your project.
     * @param[in] argc - the application startup arguments count.
     * @param[in] argv - the application startup arguments.
     * @return int - return 0 if start success, otherwise return -1.
     */
    virtual int OnStart(int argc, char *argv[]) = 0;

    /**
     * Application wait event method, please override this method in your project.
     */
    virtual void OnWait() = 0;

    /**
     * Application stop event method, please override this method in your project.
     */
    virtual void OnStop() = 0;

    /**
     * Application ini config reloaded event method, please override this method in your project.
     */
    virtual void OnIniConfigReloaded();

    /**
     * Application property config reloaded event method, please override this method in your project.
     */
    virtual void OnPropertyConfigReloaded();

public:
    /**
     * Get this application.
     * @return App * - this application.
     */
    template <typename App>
    static App *ThisApp();
    static LLBC_IApplication *ThisApp();

public:
    /**
     * Start application.
     * @param[in] name - the application name.
     * @param[in] argv - the application startup arguments.
     * @return int - return 0 if start success, otherwise return -1.
     */
    int Start(const LLBC_String &name, int argc, char *argv[]);

    /**
     * Check application started or not.
     */
    bool IsStarted() const;

    /**
     * Wait application.
     */
    void Wait();

    /**
     * Stop application.
     */
    void Stop();

    /**
     * Set dump file when application dump.
     * @param[in] dumpFileName - the dump file name.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetDumpFile(const LLBC_String &dumpFileName);

    /**
     * Set crash hook, invoke after crashed.
     * @param[in] crashHook - the crash hook.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetCrashHook(const std::function<void(const LLBC_String &)> &crashHook);

public:
    /**
     * Get application name.
     * @return const LLBC_String & - application name.
     */
    const LLBC_String &GetName() const;

    /**
     * Get startup arguments.
     * @return const LLBC_StartArgs & - the startup arguments(llbc library wrapped object const reference).
     */
    const LLBC_StartArgs &GetStartArgs() const;

public:
    /**
     * Get application ini format config.
     * @return const LLBC_Ini & - ini config.
     */
    const LLBC_Ini &GetIniConfig() const;

    /**
     * Get application property format config.
     * @return const LLBC_Property & - property config.
     */
    const LLBC_Property &GetPropertyConfig() const;

    /**
     * Reload application ini format config.
     * @param[in] configPath - the config file path.
     * @param[in] callEvMeth - specific call event method when reload success or not.
     * @return int - return 0 if success, otherwise return -1.
     */
    int ReloadIniConfig(bool callEvMeth = true);
    int ReloadIniConfig(const LLBC_String &configPath, bool callEvMeth = true);

    /**
     * Reload application property format config.
     * @param[in] configPath - the config file path.
     * @param[in] callEvMeth - specific call event method when reload success or not.
     * @return int - return 0 if success, otherwise return -1.
     */
    int ReloadPropertyConfig(bool callEvMeth = true);
    int ReloadPropertyConfig(const LLBC_String &configPath, bool callEvMeth = true);

public:
    /**
     * Get service.
     * @param[in] id - service Id.
     * @return LLBC_IService * - service.
     */
    LLBC_IService *GetService(int id) const;

    /**
     * Remove service.
     * @param[in] id - service Id.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveService(int id);

public:
    /**
     * Send packet.
     * @param[in] packet - packet.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Send(LLBC_Packet *packet);

private:
    int TryLoadConfig(bool tryIni = true, bool tryPropCfg = true);
    int TryLoadConfig(bool &loaded, bool tryIni = true, bool tryPropCfg = true);
    int TryLoadConfig(const LLBC_String &cfgPath, bool &loaded, bool tryIni = true, bool tryPropCfg = true);

    void AfterReloadConfig(bool iniReloaded, bool propReloaded, bool callEvMeth);

protected:
    LLBC_String _name;
    LLBC_SpinLock _lock;

    LLBC_Ini _iniConfig;
    LLBC_Property _propertyConfig;
    bool _loadingIniCfg;
    bool _loadingPropertyCfg;

    LLBC_ServiceMgr &_services;

private:
    volatile bool _started;
    bool _waited;
    LLBC_StartArgs _startArgs;

#if LLBC_TARGET_PLATFORM_WIN32
    LLBC_String _dumpFileName;
    std::function<void(const LLBC_String &)> _crashHook;
#endif // Win32

    static LLBC_IApplication *_thisApp;
};

__LLBC_NS_END

#include "llbc/application/IApplicationImpl.h"

#endif // !__LLBC_APP_IAPPLICATION_H__
