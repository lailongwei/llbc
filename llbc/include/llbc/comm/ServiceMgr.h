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

#include "llbc/core/Core.h"

__LLBC_NS_BEGIN

/**
 * Previous declare some classes.
 */
class LLBC_Service;

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
    typedef std::map<int, LLBC_Service *> Id2Services;
    typedef std::map<LLBC_CString, LLBC_Service *> Name2Services;

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
     * @return LLBC_Service * - the service, if not found, return nullptr.
     */
    LLBC_Service *GetService(int id);

    /**
     * Get service by service name.
     * @param[in] name - the service name.
     * @return LLBC_Service * - the service, if not found, return nullptr.
     */
    LLBC_Service *GetService(const LLBC_CString &name);

public:
    /**
     * Stop specific id service.
     * Note:
     *  1. Not allow to remove self thread drive's service, if try to call, 
     *     it will return -1 and LLBC_GetLastError() return LLBC_ERROR_PERM.
     * @param[in] id  - the service Id.
     * @param[in] del - delete service or not, default is true.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Stop(int id, bool del = true);

    /**
     * Stop specified name service, like Remove(int id) method.
     * Note:
     *  1. Not allow to remove self thread drive's service, if try to call, 
     *     it will return -1 and LLBC_GetLastError() return LLBC_ERROR_PERM.
     * @param[in] name - the service name.
     * @param[in] del  - delete service or not, default is true.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Stop(const LLBC_CString &name, bool del = true);

    /**
     * Stop all services.
     * Note:
     *  1. Not allow to remove self thread drive's service, if try to call, 
     *     it will return -1 and LLBC_GetLastError() return LLBC_ERROR_PERM.
     * @param[in] del - delete service or not, default is true.
     * @return int - return 0 if success, otherwise return -1.
     */
    int StopAll(bool del = true);

public:
    /**
     * Get all services(included not-start/starting/stoping status services).
     * @return const Id2Services & - the services collections(indexed by service id).
     */
    const Id2Services &GetAllIndexedByIdServices() const;

    /**
     * Get all services(included not-start/starting/stoping status services).
     * @return const Name2Services & - the services collections(indexed by service name).
     */
    const Name2Services &GetAllIndexedByNameServices() const;

private:
    /**
     * Friend class: LLBC_ServiceImpl.
     *  Access methods:
     *      OnServiceStart()
     *      OnServiceStop()
     */
    friend class LLBC_ServiceImpl;

    /**
     * Add service to service manager.
     * @param[in] svc - the service.
     */
    void OnServiceStart(LLBC_Service *svc);

    /**
     * Remove service from service manager.
     * @param[in] id - the service Id.
     */
    void OnServiceStop(LLBC_Service *svc);

private:
    int Stop(LLBC_Service *svc, bool del);

    LLBC_Service *GetServiceNonLock(int id);
    LLBC_Service *GetServiceNonLock(const LLBC_CString &name);

    static bool InTls(const LLBC_Service *svc);
    static bool InTls(const Id2Services &svcs);
    static bool InTls(const Name2Services &svcs);

private:
    LLBC_SpinLock _lock;

    std::vector<LLBC_Service *> _serviceList;
    Id2Services _id2Services;
    Name2Services _name2Services;
};

/**
 * Singleton macro define.
 */
template class LLBC_EXPORT LLBC_Singleton<LLBC_ServiceMgr>;
#define LLBC_ServiceMgrSingleton LLBC_Singleton<LLBC_ServiceMgr>::Instance()

__LLBC_NS_END

#include "llbc/comm/ServiceMgrInl.h"



