/**
 * @file    ServiceMgr.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/24
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_SERVICE_MGR_H__
#define __LLBC_COMM_SERVICE_MGR_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_IService;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The service manager class encapsulation.
 *  Note: The service manager will not take over <ExternalDrive> type service,
 *        about the DriveType enumeration, see IService.h header file.
 */
class LLBC_EXPORT LLBC_ServiceMgr
{
public:
    /**
     * Constructor & Destructor.
     */
    LLBC_ServiceMgr();
    ~LLBC_ServiceMgr();

public:
    /**
     * Get service by service Id.
     * @param[in] id - the service Id.
     * @return LLBC_IService * - the service, if not found, return NULL.
     */
    LLBC_IService *GetService(int id);

    /**
     * Get service by service name.
     * @param[in] name - the service name.
     * @return LLBC_IService * - the service, if not found, return NULL.
     */
    LLBC_IService *GetService(const LLBC_String &name);

    /**
     * Remove specified id service.
     * Note:
     *  1. Not allow to remove self thread drive's service, if try to call, 
     *     it will return -1 and LLBC_GetLastError() return LLBC_ERROR_PERM.
     * @param[in] id - the service Id.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveService(int id);

    /**
     * Remove specified name service, like Remove(int id) method.
     * @param[in] id - the service Id.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveService(const LLBC_String &name);

    /**
     * Wait all services.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Wait();

    /**
     * Stop all services.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Stop();

private:
    /**
     * Friend class: LLBC_Service.
     *  Access methods:
     *      OnServiceStart()
     *      OnServiceStop()
     */
    friend class LLBC_Service;

    /**
     * Add service to service manager.
     * @param[in] svc - the service.
     */
    void OnServiceStart(LLBC_IService *svc);

    /**
     * Remove service from service manager.
     * @param[in] id - the service Id.
     */
    void OnServiceStop(LLBC_IService *svc);

private:
    LLBC_IService *GetServiceNonLock(int id);
    LLBC_IService *GetServiceNonLock(const LLBC_String &name);

private:
    static bool InTls(const LLBC_IService *svc);

    typedef std::map<int, LLBC_IService *> _Services;
    static bool InTls(const _Services &svcs);

    typedef std::map<LLBC_String, LLBC_IService *> _Services2;
    static bool InTls(const _Services2 &svcs);

private:
    LLBC_SpinLock _lock;

    _Services _id2Services;
    _Services2 _name2Services;
};

/**
 * Singleton macro define.
 */
template class LLBC_EXPORT LLBC_Singleton<LLBC_ServiceMgr>;
#define LLBC_ServiceMgrSingleton LLBC_Singleton<LLBC_ServiceMgr>::Instance()

__LLBC_NS_END

#include "llbc/comm/ServiceMgrImpl.h"

#endif // !__LLBC_COMM_SERVICE_MGR_H__

