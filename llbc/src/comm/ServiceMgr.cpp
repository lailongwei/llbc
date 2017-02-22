/**
 * @file    ServiceMgr.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/24
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

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
    LLBC_Guard guard(_lock);
    _Services::iterator it = _id2Services.find(id);
    if (it == _id2Services.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }

    return it->second;
}

LLBC_IService *LLBC_ServiceMgr::GetService(const LLBC_String &name)
{
    LLBC_Guard guard(_lock);
    _Services2::iterator it = _name2Services.find(name);
    if (it == _name2Services.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }

    return it->second;
}

int LLBC_ServiceMgr::RemoveService(int id)
{
    LLBC_Guard guard(_lock);
    LLBC_IService *svc = GetService(id);
    if (!svc)
    {
        return LLBC_FAILED;
    }
    else if (This::InTls(svc))
    {
        LLBC_SetLastError(LLBC_ERROR_PERM);
        return LLBC_FAILED;
    }

    LLBC_Delete(svc);
    return LLBC_OK;
}

int LLBC_ServiceMgr::RemoveService(const LLBC_String &name)
{
    LLBC_Guard guard(_lock);
    LLBC_IService *svc = GetService(name);
    if (!svc)
    {
        return LLBC_FAILED;
    }

    return RemoveService(svc->GetId());
}

int LLBC_ServiceMgr::Wait()
{
    LLBC_Guard guard(_lock);
    if (This::InTls(_id2Services))
    {
        LLBC_SetLastError(LLBC_ERROR_PERM);
        return LLBC_FAILED;
    }

    _Services svcs = _id2Services;

    for (_Services::iterator it = svcs.begin();
         it != svcs.end();
         it++)
    {
        if (_id2Services.find(it->first) == _id2Services.end())
            continue;

        // Ignore return value.
        it->second->Wait();
    }

    return LLBC_OK;
}

int LLBC_ServiceMgr::Stop()
{
    LLBC_Guard guard(_lock);
    if (This::InTls(_id2Services))
    {
        LLBC_SetLastError(LLBC_ERROR_PERM);
        return LLBC_FAILED;
    }

    _Services svcs = _id2Services;

    for (_Services::iterator it = svcs.begin();
         it != svcs.end();
         it++)
    {
        if (_id2Services.find(it->first) == _id2Services.end())
            continue;
        
        it->second->Stop();
    }

    return LLBC_OK;
}

bool LLBC_ServiceMgr::InTls(const LLBC_IService *svc)
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    void **svcs = tls->commTls.services;
    for (int i = 0;
         i <= LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT;
         i++)
        if (reinterpret_cast<LLBC_IService *>(svcs[i]) == svc)
            return true;

    return false;
}

bool LLBC_ServiceMgr::InTls(const This::_Services &svcs)
{
    for (_Services::const_iterator it = svcs.begin();
         it != svcs.end();
         it++)
        if (This::InTls(it->second))
            return true;

    return false;
}

bool LLBC_ServiceMgr::InTls(const This::_Services2 &svcs)
{
    for (_Services2::const_iterator it = svcs.begin();
         it != svcs.end();
         it++)
        if (This::InTls(it->second))
            return true;

    return false;
}

void LLBC_ServiceMgr::OnServiceStart(LLBC_IService *svc)
{
    LLBC_Guard guard(_lock);
    _id2Services.insert(std::make_pair(svc->GetId(), svc));
    _name2Services.insert(std::make_pair(svc->GetName(), svc));
}

void LLBC_ServiceMgr::OnServiceStop(LLBC_IService *svc)
{
    LLBC_Guard guard(_lock);
    _id2Services.erase(svc->GetId());
    _name2Services.erase(svc->GetName());
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
