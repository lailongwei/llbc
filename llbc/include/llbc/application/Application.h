/**
 * @file    Application.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2103/06/17
 * @version 1.0
 *
 * @brief
 */
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
 */
class LLBC_EXPORT LLBC_BaseApplication
{
public:
    LLBC_BaseApplication();
    virtual ~LLBC_BaseApplication();

public:
    /**
     * Get application name.
     * @return const LLBC_String & - application name.
     */
    virtual const LLBC_String &GetName() const;

    /**
     * Get application config.
     * return const LLBC_Config & - application config.
     */
    virtual const LLBC_Config &GetConfig() const;

public:
    /**
     * Initialize application.
     * @param[in] name - application name.
     * @param[in] arg  - user data.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Initialize(const LLBC_String &name, void *arg);

    /**
     * Start application.
     */
    virtual void Start();

    /**
     * Wait application.
     */
    virtual void Wait();

    /**
     * Stop application.
     */
    virtual void Stop();

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

protected:
    LLBC_String _name;
    LLBC_Config _config;

    LLBC_ServiceMgr &_services;
};

__LLBC_NS_END

#endif // !__LLBC_COMM_APPLICATION_H__
