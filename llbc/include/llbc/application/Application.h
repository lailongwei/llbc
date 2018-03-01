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

#ifndef __LLBC_COMM_APPLICATION_H__
#define __LLBC_COMM_APPLICATION_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"
#include "llbc/comm/Comm.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_Packet;
class LLBC_IFacade;
class LLBC_IService;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The application class encapsulation.
 *        Note: Please call Start/Wait/Stop method at main thread.
 */
class LLBC_EXPORT LLBC_BaseApplication
{
public:
    LLBC_BaseApplication();
    virtual ~LLBC_BaseApplication();

public:
    /**
     * Application start event method, please override this method at your project.
     * @param[in] name - the application name.
     * @param[in] argv - the application startup arguments.
     * @return int - return 0 if start success, otherwise return -1.
     */
    virtual int OnStart(int argc, char *argv[]);

    /**
     * Application wait event method, please override this method at your project.
     */
    virtual void OnWait();

    /**
     * Application stop event method, please override this method at your project.
     */
    virtual void OnStop();

public:
    /**
     * Get this application.
     * @return LLBC_BaseApplication * - this application.
     */
    static LLBC_BaseApplication *ThisApp();

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
     * Get application json format config.
     * return const LLBC_Config & - json config.
     */
    const LLBC_Config &GetJsonConfig() const;

    /**
     * Get application property format config.
     * @return const LLBC_Property & - property config.
     */
    const LLBC_Property &GetPropertyConfig() const;

    /**
     * Reload application ini format config.
     * @param[in] configPath - the config file path.
     * @return int - return 0 if success, otherwise return -1.
     */
    int ReloadIniConfig();
    int ReloadIniConfig(const LLBC_String &configPath);

    /**
     * Reload application json format config.
     * @param[in] configPath - the config file path.
     * @return int - return 0 if success, otherwise return -1.
     */
    int ReloadJsonConfig();
    int ReloadJsonConfig(const LLBC_String &configPath);

    /**
     * Reload application property format config.
     * @param[in] configPath - the config file path.
     * @return int - return 0 if success, otherwise return -1.
     */
    int ReloadPropertyConfig();
    int ReloadPropertyConfig(const LLBC_String &configPath);

public:
    /**
     * Get service.
     * @param[in] serviceId - service Id.
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
    int TryLoadConfig(bool tryIni = true, bool tryJson = true, bool tryCfg = true);
    int TryLoadConfig(bool &loaded, bool tryIni = true, bool tryJson = true, bool tryCfg = true);
    int TryLoadConfig(const LLBC_String &cfgPath, bool &loaded, bool tryIni = true, bool tryJson = true, bool tryCfg = true);

protected:
    LLBC_String _name;

    LLBC_Ini _iniConfig;
    LLBC_Config _jsonConfig;
    LLBC_Property _propertyConfig;

    LLBC_ServiceMgr &_services;

private:
    volatile bool _started;
    bool _waited;
    LLBC_StartArgs _startArgs;

    static LLBC_BaseApplication *_thisApp;
};

__LLBC_NS_END

#endif // !__LLBC_COMM_APPLICATION_H__
