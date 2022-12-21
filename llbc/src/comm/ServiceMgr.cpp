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

#include "llbc/comm/Service.h"
#include "llbc/comm/ServiceMgr.h"

namespace
{
    typedef LLBC_NS LLBC_ServiceMgr This;
}

__LLBC_NS_BEGIN

LLBC_ServiceMgr::LLBC_ServiceMgr()
{
}

LLBC_ServiceMgr::~LLBC_ServiceMgr()
{
    // Do nothing.
}

LLBC_Service *LLBC_ServiceMgr::GetService(int id)
{
    LLBC_LockGuard guard(_lock);
    return GetServiceNonLock(id);
}

LLBC_Service *LLBC_ServiceMgr::GetService(const LLBC_CString &name)
{
    LLBC_LockGuard guard(_lock);
    return GetServiceNonLock(name);
}

int LLBC_ServiceMgr::Stop(int id, bool del)
{
    // Find service.
    _lock.Lock();
    LLBC_Service *svc = GetServiceNonLock(id);
    if (!svc)
    {
        _lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);

        return LLBC_FAILED;
    }
    _lock.Unlock();

    // Exec service stop.
    return Stop(svc, del);
}

int LLBC_ServiceMgr::Stop(const LLBC_CString &name, bool del)
{
    // Find service.
    _lock.Lock();
    LLBC_Service *svc = GetServiceNonLock(name);
    if (!svc)
    {
        _lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);

        return LLBC_FAILED;
    }
    _lock.Unlock();

    // Exec service stop.
    return Stop(svc, del);
}

int LLBC_ServiceMgr::StopAll(bool del)
{
    // Fetch all services.
    _lock.Lock();
    Id2Services svcs = _id2Services;
    _lock.Unlock();

    // Foreach stop services.
    for (Id2Services::iterator it = svcs.begin();
         it != svcs.end();
         ++it)
        Stop(it->first, del);

    return LLBC_OK;
}

int LLBC_ServiceMgr::Stop(LLBC_Service *svc, bool del)
{
    svc->Stop();
    if (del)
        delete svc;

    return LLBC_OK;
}

bool LLBC_ServiceMgr::InTls(const LLBC_Service *svc)
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    void **svcs = tls->commTls.services;
    for (int i = 0;
         i <= LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT;
         ++i)
        if (reinterpret_cast<LLBC_Service *>(svcs[i]) == svc)
            return true;

    return false;
}

bool LLBC_ServiceMgr::InTls(const Id2Services &svcs)
{
    for (Id2Services::const_iterator it = svcs.begin();
         it != svcs.end();
         ++it)
        if (InTls(it->second))
            return true;

    return false;
}

bool LLBC_ServiceMgr::InTls(const Name2Services &svcs)
{
    for (Name2Services::const_iterator it = svcs.begin();
         it != svcs.end();
         ++it)
        if (InTls(it->second))
            return true;

    return false;
}

void LLBC_ServiceMgr::OnServiceStart(LLBC_Service *svc)
{
    LLBC_LockGuard guard(_lock);

    _serviceList.push_back(svc);
    _id2Services.insert(std::make_pair(svc->GetId(), svc));
    _name2Services.insert(std::make_pair(svc->GetName(), svc));
}

void LLBC_ServiceMgr::OnServiceStop(LLBC_Service *svc)
{
    LLBC_LockGuard guard(_lock);

    _serviceList.erase(
        std::find(_serviceList.begin(), _serviceList.end(), svc));
    _id2Services.erase(svc->GetId());
    _name2Services.erase(svc->GetName());
}

__LLBC_NS_END
