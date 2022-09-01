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

#include "llbc/comm/IService.h"
#include "llbc/comm/ServiceMgr.h"

namespace
{
    typedef LLBC_NS LLBC_ServiceMgr This;
}

__LLBC_NS_BEGIN

LLBC_ServiceMgr::LLBC_ServiceMgr()
: _lock()

, _id2Services()
, _name2Services()
{
}

LLBC_ServiceMgr::~LLBC_ServiceMgr()
{
    // Do nothing.
}

LLBC_IService *LLBC_ServiceMgr::GetService(int id)
{
    LLBC_LockGuard guard(_lock);
    return GetServiceNonLock(id);
}

LLBC_IService *LLBC_ServiceMgr::GetService(const LLBC_String &name)
{
    LLBC_LockGuard guard(_lock);
    return GetServiceNonLock(name);
}

int LLBC_ServiceMgr::Stop(int id, bool del)
{
    // Lock.
    _lock.Lock();
    // Find service.
    LLBC_IService *svc = GetServiceNonLock(id);
    if (!svc)
    {
        _lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);

        return LLBC_FAILED;
    }

    // Exec service stop.
    return Stop(svc, del);
}

int LLBC_ServiceMgr::Stop(const LLBC_String &name, bool del)
{
    // Lock.
    _lock.Lock();
    // Find service.
    LLBC_IService *svc = GetServiceNonLock(name);
    if (!svc)
    {
        _lock.Unlock();
        return LLBC_FAILED;
    }

    // Exec service stop.
    return Stop(svc, del);
}

int LLBC_ServiceMgr::StopAll(bool del)
{
    // Fetch all services.
    _lock.Lock();
    if (InTls(_id2Services))
    {
        _lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_PERM);

        return LLBC_FAILED;
    }

    Id2Services svcs = _id2Services;
    _lock.Unlock();

    // Foreach stop services.
    for (Id2Services::iterator it = svcs.begin();
         it != svcs.end();
         ++it)
        Stop(it->first, del);

    return LLBC_OK;
}

int LLBC_ServiceMgr::Stop(LLBC_IService *svc, bool del)
{
    // Not allow call in service self thread.
    if (InTls(svc))
    {
        _lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_PERM);
        return LLBC_FAILED;
    }

    // Unlock.
    _lock.Unlock();

    // Stop service.
    svc->Stop();
    if (del)
        delete svc;

    return LLBC_OK;
}

bool LLBC_ServiceMgr::InTls(const LLBC_IService *svc)
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    void **svcs = tls->commTls.services;
    for (int i = 0;
         i <= LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT;
         ++i)
        if (reinterpret_cast<LLBC_IService *>(svcs[i]) == svc)
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

void LLBC_ServiceMgr::OnServiceStart(LLBC_IService *svc)
{
    LLBC_LockGuard guard(_lock);
    _id2Services.insert(std::make_pair(svc->GetId(), svc));
    _name2Services.insert(std::make_pair(svc->GetName(), svc));
}

void LLBC_ServiceMgr::OnServiceStop(LLBC_IService *svc)
{
    LLBC_LockGuard guard(_lock);
    _id2Services.erase(svc->GetId());
    _name2Services.erase(svc->GetName());
}

__LLBC_NS_END
